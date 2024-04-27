#include "ConCurrentAlloc.h"

// 分配内存
void* con_current_alloc(size_t size)
{
	// 通过tls方式无锁获取自身线程缓存
	if (tls_thread_cache == nullptr)
		tls_thread_cache = new ThreadCache;

	//std::cout << std::this_thread::get_id() << " alloc " << size << " bytes"
	//		  << " tls_thread_cache: " << tls_thread_cache << "\n";

	return tls_thread_cache->alloc(size);
}

// 释放内存
void con_current_dealloc(void* ptr, size_t size)
{
	assert(tls_thread_cache != nullptr);
	//std::cout << std::this_thread::get_id() << " dealloc " << size << " bytes"
	//		  << " tls_thread_cache: " << tls_thread_cache << "\n";

	tls_thread_cache->dealloc(ptr, size);
}
