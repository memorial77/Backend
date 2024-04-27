#pragma once

#include "Common.h"

// 中心缓存
class CentralCache
{
public:
	// 获取中心缓存的唯一实例 饿汉模式
	static CentralCache* get_instance();

	// 从中心缓存获取一定数量的对象给线程缓存
	size_t fetch_range_objs(void*& start, void*& end, size_t batch_num, size_t align_size);

	// 从SpanList获取一个非空Span
	Span* get_one_span(SpanList& span_list, size_t align_size);

private:
	SpanList central_cache_span_list_[SPAN_LIST_SIZE];		// 中心缓存中SpanList数组

	CentralCache() {}										// 构造函数私有
	CentralCache(const CentralCache&) = delete;				// 禁止拷贝构造
	CentralCache& operator=(const CentralCache&) = delete;	// 禁止赋值拷贝
};