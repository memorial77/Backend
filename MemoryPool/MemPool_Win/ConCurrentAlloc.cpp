#include "ConCurrentAlloc.h"
#include "PageCache.h"
#include "ObjectPool.h"

// 分配内存
void* con_current_alloc(size_t size)
{
	if (size > MAX_SIZES)
	{
		size_t align_size = SizeClass::round_up(size); // 对齐大小
		size_t pages = align_size >> PAGE_SHIFT; // 计算需要的页数
		PageCache* page_cache = PageCache::get_instance(); // 获取页缓存的唯一实例

		page_cache->get_mutex().lock(); // 加锁
		Span* span = PageCache::get_instance()->new_span(pages); // 从页缓存获取一个Span
		span->obj_size_ = align_size; // 设置Span的内存块大小
		page_cache->get_mutex().unlock(); // 解锁

		void* ptr = (void*)(span->page_id_ << PAGE_SHIFT); // 页号转换为地址 
		return ptr;
	}
	else
	{
		// 通过tls方式无锁获取自身线程缓存
		if (tls_thread_cache == nullptr)
		{
			//tls_thread_cache = new ThreadCache;
			static ObjectPool<ThreadCache> pool;
			tls_thread_cache = pool.New();
		}

		//std::cout << std::this_thread::get_id() << " alloc " << size << " bytes"
		//		  << " tls_thread_cache: " << tls_thread_cache << "\n";

		return tls_thread_cache->alloc(size);
	}

}

// 释放内存
void con_current_dealloc(void* ptr)
{
	PageCache* page_cache = PageCache::get_instance(); // 获取页缓存的唯一实例
	Span* span = page_cache->map_object_to_span(ptr); // 通过地址映射到对应的Span
	size_t obj_size = span->obj_size_; // 获取内存块大小

	if (obj_size > MAX_SIZES)
	{
		page_cache->get_mutex().lock(); // 加锁
		page_cache->release_span_to_page_cache(span); // 释放Span到页缓存
		page_cache->get_mutex().unlock(); // 解锁
		return;
	}
	else
	{
		assert(tls_thread_cache != nullptr);
		//std::cout << std::this_thread::get_id() << " dealloc " << size << " bytes"
		//		  << " tls_thread_cache: " << tls_thread_cache << "\n";

		tls_thread_cache->dealloc(ptr, obj_size);
	}
}
