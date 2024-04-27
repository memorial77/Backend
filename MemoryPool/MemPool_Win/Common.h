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

// �����ƽ̨ҳ������
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

static const size_t FREE_LIST_SIZE = 208;		// �̻߳�������������ĸ���
static const size_t SPAN_LIST_SIZE = 208;		// ���Ļ�����SpanList����ĸ���
static const size_t PAGE_LIST_SIZE = 129;		// ҳ������SpanList����ĸ���
static const size_t MAX_SIZES = 256 * 1024;		// ������ڴ��С���ܳ���256KB
static const size_t PAGE_SIZE = 8 * 1024;		// ҳ��С8KB
static const size_t PAGE_SHIFT = 13;			// 2^13 = 8KB


static void*& next_obj(void* ptr)
{
	return *((void**)ptr);
}

// ϵͳ���û�ȡ�ڴ� ��ҳΪ��λ
static void* system_alloc(size_t pages)
{
	size_t size = pages * PAGE_SIZE; // ������Ҫ���ڴ��С

#ifdef _WIN32
	// Windows
	void* ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (ptr == NULL) {
		// �������
		return nullptr;
	}
#else
	// Linux
	void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ptr == MAP_FAILED) {
		// �������
		return nullptr;
	}
#endif

	return ptr;
}


class FreeList
{
public:

	FreeList() : list_(nullptr) {} // ��ʼ������Ϊ��

	~FreeList() {} // ��������

	// ���ڴ����뵽����ͷ��
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

		// ��[start, end)������ڴ����뵽����������
		next_obj(end) = list_;
		list_ = start;
	}

	// ������ͷ��ȡ���ڴ��
	void* pop()
	{
		// ��������Ϊ�ղ��ܼ��������ڴ��
		assert(list_ != nullptr);
		void* obj = list_;
		list_ = next_obj(list_);
		return obj;
	}

	// �ж����������Ƿ�Ϊ��
	bool empty() const
	{
		return list_ == nullptr;
	}

	size_t& get_count()
	{
		return get_count_;
	}

private:
	void* list_;			// ��������ͷָ��
	size_t get_count_ = 1; // �����Ļ����ȡ���ڴ�����
};


// ������ҳΪ��λ�Ĵ��ڴ��
struct Span
{


	page_id_type page_id_ = 0;	// ����ڴ���ʼҳҳ��
	size_t page_num_ = 0;		// ����ڴ��ܹ�ҳ��

	// ˫������
	Span* next_ = nullptr;		// ָ����һ��Span
	Span* prev_ = nullptr;		// ָ����һ��Span

	void* list_ = nullptr;		// �����и���С���ڴ�������
	size_t use_count_ = 0;		// �и�õ�С���ڴ�����ThreadCache�ĸ���
};

// ��ͷ˫��span����
class SpanList
{
public:
	// ����ͷ�ڵ�
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

	// ��posλ�ò���span�ڵ�
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

	// ɾ��posλ�õ�span�ڵ�
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
	Span* head_;		// ͷָ��
	std::mutex mtx_;	// Ͱ�Ļ�����
};


// �ڴ����ع�����
class SizeClass
{
public:
	// ������������10%���ҵ�����Ƭ�˷�
	// [1,128]              8byte����           freelist[0,16)
	// [128+1,1024]         16byte����          freelist[16,72)
	// [1024+1,8*1024]      128byte����         freelist[72,128)
	// [8*1024+1,64*1024]   1024byte����        freelist[128,184)
	// [64*1024+1,256*1024] 8*1024byte����      freelist[184,208)

	// �ڴ���������
	static const size_t SMALL_SIZE = 128;             // ��Ӧ���������±�0-15 �����С8
	static const size_t MEDIUM_SIZE = 1024;           // ��Ӧ���������±�16-71 �����С16
	static const size_t LARGE_SIZE = 8 * 1024;        // ��Ӧ���������±�72-127 �����С128
	static const size_t EXTRA_LARGE_SIZE = 64 * 1024; // ��Ӧ���������±�128-183 �����С1024
	static const size_t HUGE_SIZE = 256 * 1024;       // ��Ӧ���������±�184-207 �����С8*1024

	// �����С�ĳ���
	static const size_t SMALL_ALIGN = 8;
	static const size_t MEDIUM_ALIGN = 16;
	static const size_t LARGE_ALIGN = 128;
	static const size_t EXTRA_LARGE_ALIGN = 1024;
	static const size_t HUGE_ALIGN = 8 * 1024;

	// �����ڴ���С��Ӧ�����������±�
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

	// �ڴ���븨������
	static size_t align_up(size_t size, size_t align)
	{
		return (size + align - 1) & ~(align - 1);
	}

	// �ڴ���뺯��
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
			// TODO: ����HUGE_SIZE���ڴ����ʱ������
			assert(false);
			return 0;
		}
	}

	// ����һ�δ����Ļ����ȡ�ڴ�������
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
			// TODO: ����HUGE_SIZE���ڴ����ʱ������
			return 0;
		}
	}

	// ����һ�δ�ҳ�����ȡҳ������
	static size_t num_move_page(size_t align_size)
	{
		size_t batch_num = num_move_size(align_size);
		size_t batch_size = batch_num * align_size;

		// ����ҳ��
		size_t page_nums = batch_size >> PAGE_SHIFT;
		// �����������ҳ�����1ҳ
		if (batch_size & (PAGE_SIZE - 1))
			++page_nums;

		//// ҳ�����ܳ���128ҳ
		//if (page_nums > 128)
		//	page_nums = 128;

		return page_nums;
	}
};