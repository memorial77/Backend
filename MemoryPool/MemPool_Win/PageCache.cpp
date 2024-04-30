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
	// 如果k大于PAGE_LIST_SIZE-1 = 128 则直接从系统中获取
	if (k > PAGE_LIST_SIZE - 1)
	{
		void* ptr = system_alloc(k);
		//Span* span = new Span();
		Span* span = span_ptr_pool_.New();
		span->page_id_ = (page_id_type)ptr >> PAGE_SHIFT;
		span->page_num_ = k;
		return span;
	}
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
			//Span* new_span = new Span();
			Span* new_span = span_ptr_pool_.New();
			new_span->page_id_ = span->page_id_;
			new_span->page_num_ = k;

			span->page_id_ += k;
			span->page_num_ -= k;

			// 建立page_id到span的映射，便于central_cache释放内存时查找对应的span
			for (page_id_type i = 0; i < k; ++i)
			{
				id_span_map_[new_span->page_id_ + i] = new_span;
			}

			// 存储span的起始页号以及对应的span
			id_span_map_[span->page_id_] = span;
			id_span_map_[span->page_id_ + span->page_num_ - 1] = span;

			// 将剩余的span插入到对应的span_list中
			page_cache_span_list_[span->page_num_].push_front(span);
			return new_span;
		}
	}

	// 没有可用的span，从系统中获取
	//Span* big_span = new Span();
	Span* big_span = span_ptr_pool_.New();
	void* ptr = system_alloc(PAGE_LIST_SIZE - 1);
	big_span->page_id_ = (page_id_type)ptr >> PAGE_SHIFT; // 根据指针获取页号
	big_span->page_num_ = PAGE_LIST_SIZE - 1;

	page_cache_span_list_[k].push_front(big_span);

	return new_span(k);	// 递归调用
}

Span* PageCache::map_object_to_span(void* start)
{
	// 根据对象获取其所在的span
	page_id_type page_id = (page_id_type)start >> PAGE_SHIFT;
	auto it = id_span_map_.find(page_id);
	// 如果找到则返回对应的span
	if (it != id_span_map_.end())
	{
		return it->second;
	}
	assert(false);
	return nullptr;
}

// 将span释放到page_cache
void PageCache::release_span_to_page_cache(Span* span)
{
	// 如果span的页数超过了PAGE_LIST_SIZE-1 = 128 则直接释放
	if (span->page_num_ > PAGE_LIST_SIZE - 1)
	{
		void* ptr = (void*)(span->page_id_ << PAGE_SHIFT);
		system_dealloc(ptr);
		//delete span;
		span_ptr_pool_.Delete(span);
		return;
	}
	page_cache_mutex_.lock();

	// 合并前面的span
	while (true)
	{
		page_id_type prev_page_id = span->page_id_ - 1;
		auto it = id_span_map_.find(prev_page_id);

		if (it == id_span_map_.end())
			break;

		Span* prev_span = it->second;
		// 如果前面的span被使用了 则不再合并
		if (prev_span->is_usud_)
			break;

		// 如果合并后的span的页数超过了PAGE_LIST_SIZE-1 = 128 则不再合并
		if (prev_span->page_num_ + span->page_num_ >= PAGE_LIST_SIZE)
			break;

		// 合并
		page_cache_span_list_[prev_span->page_num_].erase(prev_span);
		span->page_id_ = prev_span->page_id_;
		span->page_num_ += prev_span->page_num_;

		// 删除prev_span
		//delete prev_span;
		span_ptr_pool_.Delete(prev_span);
	}

	// 合并后面的span
	while (true)
	{
		page_id_type next_page_id = span->page_id_ + span->page_num_;
		auto it = id_span_map_.find(next_page_id);

		if (it == id_span_map_.end())
			break;

		Span* next_span = it->second;
		// 如果后面的span被使用了 则不再合并
		if (next_span->is_usud_)
			break;

		// 如果合并后的span的页数超过了PAGE_LIST_SIZE-1 = 128 则不再合并
		if (next_span->page_num_ + span->page_num_ >= PAGE_LIST_SIZE)
			break;

		// 合并
		page_cache_span_list_[next_span->page_num_].erase(next_span);
		span->page_num_ += next_span->page_num_;

		// 删除next_span
		//delete next_span;
		span_ptr_pool_.Delete(next_span);
	}

	// 合并之后的span插入到对应的span_list中
	page_cache_span_list_[span->page_num_].push_front(span);
	span->is_usud_ = false;

	// 更新id_span_map
	id_span_map_[span->page_id_] = span;
	id_span_map_[span->page_id_ + span->page_num_ - 1] = span;

	page_cache_mutex_.unlock();
}
