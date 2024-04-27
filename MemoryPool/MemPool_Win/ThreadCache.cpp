#include "ThreadCache.h"
#include "CentralCache.h"
#include <algorithm>

_declspec(thread) ThreadCache* tls_thread_cache = nullptr;

// 从线程缓存中分配内存
void* ThreadCache::alloc(size_t size)
{
	assert(size <= MAX_SIZES);						// 申请的内存大小不能超过MAX_SIZES = 256KB  
	size_t align_size = SizeClass::round_up(size);	// 内存对齐
	size_t index = SizeClass::index(align_size);	// 根据内存大小获取线程缓存中自由链表数组的下标

	if (thread_cache_free_list_[index].empty())
	{
		// 如果线程缓存中对应的自由链表为空则从中心缓存对应的SpanList获取内存块
		return fetch_from_central_cache(index, align_size);
	}
	else
	{
		// 线程缓存中对应的自由链表中有内存块，直接取出
		return thread_cache_free_list_[index].pop();
	}
}

// 从中心缓存对应的SpanList获取内存块
void* ThreadCache::fetch_from_central_cache(size_t index, size_t align_size)
{
	// 慢开始策略，每次从中心缓存获取的内存块数量逐渐增加
	size_t batch_size = SizeClass::num_move_size(align_size) > thread_cache_free_list_[index].get_count() ?
		thread_cache_free_list_[index].get_count() : SizeClass::num_move_size(align_size);

	if (batch_size == thread_cache_free_list_[index].get_count())
		thread_cache_free_list_[index].get_count()++;	// 逐渐增加获取的内存块数量
	else
		thread_cache_free_list_[index].get_count() = 1;	// 重置为1

	// 获取中心缓存的唯一实例
	CentralCache* central_cache = CentralCache::get_instance();
	assert(central_cache != nullptr);

	// 从中心缓存获取一定数量的对象给线程缓存
	void* start = nullptr;	// 获取的内存块的起始地址
	void* end = nullptr;	// 获取的内存块的结束地址

	// 获取的实际内存块的个数
	size_t actual_size = central_cache->fetch_range_objs(start, end, batch_size, align_size);
	assert(actual_size > 0); // 获取的内存块个数必须大于0

	// debug
	std::cout << "fetch_from_central_cache: " << actual_size << std::endl;

	if (actual_size == 1)
	{
		// 如果获取的内存块个数为1，则直接返回
		assert(start == end);
		return start;
	}
	else
	{
		// 如果获取的内存块个数大于1，则将获取的内存块插入到线程缓存的自由链表中
		thread_cache_free_list_[index].push_range(start, end, actual_size);
		return thread_cache_free_list_[index].pop();
	}
}


// 将内存块释放到线程缓存
void ThreadCache::dealloc(void* ptr, size_t size)
{
	assert(ptr != nullptr);
	assert(size <= MAX_SIZES);

	// 将要释放的内存块插入到线程缓存的自由链表中
	size_t index = SizeClass::index(size);
	thread_cache_free_list_[index].push(ptr);
}