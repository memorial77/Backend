#include "PageCache.h"

// ��ȡҳ�����Ψһʵ�� ����ģʽ
PageCache* PageCache::get_instance()
{
	static PageCache* instance = new PageCache();
	return instance;
}

// ��ȡһ��Kҳ��Span
Span* PageCache::new_span(size_t k)
{
	assert(k >= 1 && k <= PAGE_LIST_SIZE - 1);
	// ��page_cache��ȡһ��kҳspan

	// ����Ӧ��span_list���Ƿ���kҳ��span
	if (!page_cache_span_list_[k].empty())
	{
		// ����� ֱ�ӷ���
		return page_cache_span_list_[k].pop_front();
	}
	
	// ���һ�º����span_list���Ƿ���span���еĻ������и�
	for (size_t i = k + 1; i < PAGE_LIST_SIZE; ++i)
	{
		// ��Ӧ��span_list����span
		if (!page_cache_span_list_[i].empty())
		{
			Span* span = page_cache_span_list_[i].pop_front();
			// �и�
			Span* new_span = new Span();
			new_span->page_id_ = span->page_id_;
			new_span->page_num_ = k;

			span->page_id_ += k;
			span->page_num_ -= k;
			// ��ʣ���span���뵽��Ӧ��span_list��
			page_cache_span_list_[span->page_num_].push_front(span);
			return new_span;
		}
	}

	// û�п��õ�span����ϵͳ�л�ȡ
	Span* big_span = new Span();
	void* ptr = system_alloc(PAGE_LIST_SIZE - 1);
	big_span->page_id_ = (page_id_type)ptr >> PAGE_SHIFT; // ����ָ���ȡҳ��
	big_span->page_num_ = PAGE_LIST_SIZE - 1;

	page_cache_span_list_[k].push_front(big_span);

	return new_span(k);	// �ݹ����
}
