#pragma once

#include "Common.h"
#include "ObjectPool.h"

class PageCache
{
public:
	// 获取页缓存的唯一实例
	static PageCache* get_instance();

	// 获取一个K页的Span
	Span* new_span(size_t k);

	Span* map_object_to_span(void* start);

	void release_span_to_page_cache(Span* span);

	std::mutex& get_mutex() { return page_cache_mutex_; }	// 获取页缓存的互斥锁

private:
	// 按照页数映射到对应的SpanList下标 页数位于[1, 128]
	SpanList page_cache_span_list_[PAGE_LIST_SIZE];				// 页缓存中SpanList数组
	std::mutex page_cache_mutex_;								// 页缓存的互斥锁
	std::unordered_map<page_id_type, Span*> id_span_map_;		// 通过page_id找到对应的Span
	ObjectPool<Span> span_ptr_pool_;							// Span对象池

	PageCache() {}										// 构造函数私有
	PageCache(const PageCache&) = delete;				// 禁止拷贝构造
	PageCache& operator=(const PageCache&) = delete;	// 禁止赋值拷贝

};
