#pragma once

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <stdlib.h>	
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

// 处理跨平台页号类型
#ifdef _WIN32
	// Windows
#ifdef _WIN64
	// 64-bit
typedef uint64_t page_id_type;
#else
	// 32-bit
typedef uint32_t page_id_type;
#endif
#else
	// Linux
#ifdef __x86_64__
	// 64-bit
typedef uint64_t page_id_type;
#else
	// 32-bit
typedef uint32_t page_id_type;
#endif
#endif

static const size_t FREE_LIST_SIZE = 208;		// 线程缓存中自由链表的个数
static const size_t SPAN_LIST_SIZE = 208;		// 中心缓存中SpanList数组的个数
static const size_t PAGE_LIST_SIZE = 129;		// 页缓存中SpanList数组的个数
static const size_t MAX_SIZES = 256 * 1024;		// 申请的内存大小不能超过256KB
static const size_t PAGE_SIZE = 8 * 1024;		// 页大小8KB
static const size_t PAGE_SHIFT = 13;			// 2^13 = 8KB


static void*& next_obj(void* ptr)
{
	return *((void**)ptr);
}

// 系统调用获取内存 以页为单位
static void* system_alloc(size_t pages)
{
	size_t size = pages * PAGE_SIZE; // 计算需要的内存大小

#ifdef _WIN32
	// Windows
	void* ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (ptr == NULL) {
		// 处理错误
		return nullptr;
	}
#else
	// Linux
	void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ptr == MAP_FAILED) {
		// 处理错误
		return nullptr;
	}
#endif

	return ptr;
}


class FreeList
{
public:

	FreeList() : list_(nullptr) {} // 初始化链表为空

	~FreeList() {} // 析构函数

	// 将内存块插入到链表头部
	void push(void* obj)
	{
		if (list_ == nullptr)
		{
			list_ = obj;
			next_obj(obj) = nullptr;
		}
		else
		{
			next_obj(obj) = list_;
			list_ = obj;
		}
	}

	void push_range(void* start, void* end, size_t n)
	{
		assert(start != nullptr);
		assert(end != nullptr);
		assert(n > 0);

		// 将[start, end)区间的内存块插入到自由链表中
		next_obj(end) = list_;
		list_ = start;
	}

	// 从链表头部取出内存块
	void* pop()
	{
		// 自由链表为空不能继续分配内存块
		assert(list_ != nullptr);
		void* obj = list_;
		list_ = next_obj(list_);
		return obj;
	}

	// 判断自由链表是否为空
	bool empty() const
	{
		return list_ == nullptr;
	}

	size_t& get_count()
	{
		return get_count_;
	}

private:
	void* list_;			// 自由链表头指针
	size_t get_count_ = 1; // 从中心缓存获取的内存块个数
};


// 管理以页为单位的大内存块
struct Span
{


	page_id_type page_id_ = 0;	// 大块内存起始页页号
	size_t page_num_ = 0;		// 大块内存总共页数

	// 双向链表
	Span* next_ = nullptr;		// 指向下一个Span
	Span* prev_ = nullptr;		// 指向上一个Span

	void* list_ = nullptr;		// 保存切割后的小块内存块的链表
	size_t use_count_ = 0;		// 切割好的小块内存分配给ThreadCache的个数
};

// 带头双向span链表
class SpanList
{
public:
	// 构造头节点
	SpanList() : head_(nullptr)
	{
		head_ = new Span;
		head_->next_ = head_;
		head_->prev_ = head_;
	}

	Span* begin()
	{
		return head_->next_;
	}

	Span* end()
	{
		return head_;
	}

	// 在pos位置插入span节点
	void insert(Span* pos, Span* span)
	{
		assert(pos);
		assert(span);
		Span* prev = pos->prev_;
		span->next_ = pos;
		pos->prev_ = span;
		span->prev_ = prev;
		prev->next_ = span;
	}

	void push_front(Span* span)
	{
		insert(begin(), span);
	}

	// 删除pos位置的span节点
	void erase(Span* pos)
	{
		assert(pos);
		assert(pos != head_);
		Span* prev = pos->prev_;
		Span* next = pos->next_;
		prev->next_ = next;
		next->prev_ = prev;
	}

	Span* pop_front()
	{
		Span* first = begin();
		erase(first);
		return first;
	}

	bool empty() const
	{
		return head_->next_ == head_;
	}

	std::mutex& get_mutex()
	{
		return mtx_;
	}

private:
	Span* head_;		// 头指针
	std::mutex mtx_;	// 桶的互斥锁
};


// 内存块相关功能类
class SizeClass
{
public:
	// 整体控制在最多10%左右的内碎片浪费
	// [1,128]              8byte对齐           freelist[0,16)
	// [128+1,1024]         16byte对齐          freelist[16,72)
	// [1024+1,8*1024]      128byte对齐         freelist[72,128)
	// [8*1024+1,64*1024]   1024byte对齐        freelist[128,184)
	// [64*1024+1,256*1024] 8*1024byte对齐      freelist[184,208)

	// 内存块分区界限
	static const size_t SMALL_SIZE = 128;             // 对应自由链表下标0-15 对齐大小8
	static const size_t MEDIUM_SIZE = 1024;           // 对应自由链表下标16-71 对齐大小16
	static const size_t LARGE_SIZE = 8 * 1024;        // 对应自由链表下标72-127 对齐大小128
	static const size_t EXTRA_LARGE_SIZE = 64 * 1024; // 对应自由链表下标128-183 对齐大小1024
	static const size_t HUGE_SIZE = 256 * 1024;       // 对应自由链表下标184-207 对齐大小8*1024

	// 对齐大小的常量
	static const size_t SMALL_ALIGN = 8;
	static const size_t MEDIUM_ALIGN = 16;
	static const size_t LARGE_ALIGN = 128;
	static const size_t EXTRA_LARGE_ALIGN = 1024;
	static const size_t HUGE_ALIGN = 8 * 1024;

	// 计算内存块大小对应的自由链表下标
	static size_t index(size_t size)
	{
		if (size <= SMALL_SIZE)
			return (size + SMALL_ALIGN - 1) / SMALL_ALIGN - 1;
		else if (size <= MEDIUM_SIZE)
			return (size - SMALL_SIZE + MEDIUM_ALIGN - 1) / MEDIUM_ALIGN + 16 - 1;
		else if (size <= LARGE_SIZE)
			return (size - MEDIUM_SIZE + LARGE_ALIGN - 1) / LARGE_ALIGN + 72 - 1;
		else if (size <= EXTRA_LARGE_SIZE)
			return (size - LARGE_SIZE + EXTRA_LARGE_ALIGN - 1) / EXTRA_LARGE_ALIGN + 128 - 1;
		else
			return (size - EXTRA_LARGE_SIZE + HUGE_ALIGN - 1) / HUGE_ALIGN + 184 - 1;
	}

	// 内存对齐辅助函数
	static size_t align_up(size_t size, size_t align)
	{
		return (size + align - 1) & ~(align - 1);
	}

	// 内存对齐函数
	static size_t round_up(size_t size)
	{
		if (size <= SMALL_SIZE)
			return align_up(size, SMALL_ALIGN);
		else if (size <= MEDIUM_SIZE)
			return align_up(size, MEDIUM_ALIGN);
		else if (size <= LARGE_SIZE)
			return align_up(size, LARGE_ALIGN);
		else if (size <= EXTRA_LARGE_SIZE)
			return align_up(size, EXTRA_LARGE_ALIGN);
		else if (size <= HUGE_SIZE)
			return align_up(size, HUGE_ALIGN);
		else
		{
			// TODO: 超过HUGE_SIZE的内存块暂时不处理
			assert(false);
			return 0;
		}
	}

	// 计算一次从中心缓存获取内存块的数量
	static size_t num_move_size(size_t size)
	{
		if (size <= SMALL_SIZE)
			return 128;
		else if (size <= MEDIUM_SIZE)
			return 32;
		else if (size <= LARGE_SIZE)
			return 4;
		else if (size <= EXTRA_LARGE_SIZE)
			return 2;
		else if (size <= HUGE_SIZE)
			return 1;
		else
		{
			// TODO: 超过HUGE_SIZE的内存块暂时不处理
			return 0;
		}
	}

	// 计算一次从页缓存获取页的数量
	static size_t num_move_page(size_t align_size)
	{
		size_t batch_num = num_move_size(align_size);
		size_t batch_size = batch_num * align_size;

		// 计算页数
		size_t page_nums = batch_size >> PAGE_SHIFT;
		// 如果不是整数页数则加1页
		if (batch_size & (PAGE_SIZE - 1))
			++page_nums;

		//// 页数不能超过128页
		//if (page_nums > 128)
		//	page_nums = 128;

		return page_nums;
	}
};