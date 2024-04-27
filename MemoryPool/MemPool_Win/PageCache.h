#pragma once

#include "Common.h"

class PageCache
{
public:
	// ��ȡҳ�����Ψһʵ��
	static PageCache* get_instance();

	// ��ȡһ��Kҳ��Span
	Span* new_span(size_t k);

private:
	// ����ҳ��ӳ�䵽��Ӧ��SpanList�±� ҳ��λ��[1, 128]
	SpanList page_cache_span_list_[PAGE_LIST_SIZE];		// ҳ������SpanList����
	std::mutex page_cache_mutex_;						// ҳ����Ļ�����

	PageCache() {}										// ���캯��˽��
	PageCache(const PageCache&) = delete;				// ��ֹ��������
	PageCache& operator=(const PageCache&) = delete;	// ��ֹ��ֵ����
};
