#pragma once

#include "Common.h"

class ThreadCache
{
public:
	// 从线程缓存中分配内存
	void* alloc(size_t size);

	// 将内存块释放到线程缓存
	void dealloc(void* ptr, size_t size);

	// 从中心缓存获取内存块
	void* fetch_from_central_cache(size_t index, size_t align_size);

private:
	FreeList thread_cache_free_list_[FREE_LIST_SIZE]; // 自由链表数组-哈希桶
};

extern _declspec(thread) ThreadCache* tls_thread_cache; // 线程缓存指针


