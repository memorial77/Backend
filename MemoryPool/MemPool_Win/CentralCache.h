#pragma once

#include "Common.h"

// ���Ļ���
class CentralCache
{
public:
	// ��ȡ���Ļ����Ψһʵ�� ����ģʽ
	static CentralCache* get_instance();

	// �����Ļ����ȡһ�������Ķ�����̻߳���
	size_t fetch_range_objs(void*& start, void*& end, size_t batch_num, size_t align_size);

	// ��SpanList��ȡһ���ǿ�Span
	Span* get_one_span(SpanList& span_list, size_t align_size);

private:
	SpanList central_cache_span_list_[SPAN_LIST_SIZE];		// ���Ļ�����SpanList����

	CentralCache() {}										// ���캯��˽��
	CentralCache(const CentralCache&) = delete;				// ��ֹ��������
	CentralCache& operator=(const CentralCache&) = delete;	// ��ֹ��ֵ����
};