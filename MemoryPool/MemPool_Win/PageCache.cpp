#include "PageCache.h"

// 获取页缓存的唯一实例 饿汉模式
PageCache* PageCache::get_instance()
{
	static PageCache* instance = new PageCache();
	return instance;
}

// 获取一个K页的Span
Span* PageCache::new_span(size_t k)
{
	assert(k >= 1 && k <= PAGE_LIST_SIZE - 1);
	// 从page_cache获取一个k页span

	// 检查对应的span_list中是否有k页的span
	if (!page_cache_span_list_[k].empty())
	{
		// 如果有 直接返回
		return page_cache_span_list_[k].pop_front();
	}
	
	// 检查一下后面的span_list中是否有span，有的话进行切割
	for (size_t i = k + 1; i < PAGE_LIST_SIZE; ++i)
	{
		// 对应的span_list中有span
		if (!page_cache_span_list_[i].empty())
		{
			Span* span = page_cache_span_list_[i].pop_front();
			// 切割
			Span* new_span = new Span();
			new_span->page_id_ = span->page_id_;
			new_span->page_num_ = k;

			span->page_id_ += k;
			span->page_num_ -= k;
			// 将剩余的span插入到对应的span_list中
			page_cache_span_list_[span->page_num_].push_front(span);
			return new_span;
		}
	}

	// 没有可用的span，从系统中获取
	Span* big_span = new Span();
	void* ptr = system_alloc(PAGE_LIST_SIZE - 1);
	big_span->page_id_ = (page_id_type)ptr >> PAGE_SHIFT; // 根据指针获取页号
	big_span->page_num_ = PAGE_LIST_SIZE - 1;

	page_cache_span_list_[k].push_front(big_span);

	return new_span(k);	// 递归调用
}
