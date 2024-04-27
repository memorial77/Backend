#include "CentralCache.h"
#include "PageCache.h"

// ��ȡ���Ļ����Ψһʵ��
CentralCache* CentralCache::get_instance()
{
	static CentralCache* instance = new CentralCache();
	return instance;
}

// �����Ļ����ȡһ�������Ķ�����̻߳���
size_t CentralCache::fetch_range_objs(void*& start, void*& end, size_t batch_num, size_t align_size)
{
	// �����ڴ���С��ȡ��Ӧ��SpanList
	size_t index = SizeClass::index(align_size);
	SpanList& span_list = central_cache_span_list_[index];

	//span_list.get_mutex().lock(); // ����

	Span* span = get_one_span(span_list, align_size);
	assert(span != nullptr);		// ��SpanList����Span
	assert(span->list_ != nullptr); // ��Span��list_�������ڴ��

	start = span->list_;	// ��ʼλ��
	end = start;			// ����λ��
	size_t actual_num = 1;	// ʵ��ȡ�����ڴ�����
	// ��span��ȡ���ڴ��
	while (next_obj(end) != nullptr && actual_num < batch_num - 1)
	{
		end = next_obj(end);
		++actual_num;
	}
	span->list_ = next_obj(end);	// ����span��list_ָ��
	next_obj(end) = nullptr;		// ��ȡ�����ڴ���span�жϿ�

	//span_list.get_mutex().unlock(); // ����

	return actual_num;
}

// ��SpanList��ȡһ���ǿ�Span
Span* CentralCache::get_one_span(SpanList& span_list, size_t align_size)
{
	Span* begin = span_list.begin();
	// ����SpanList �ҵ�һ���ǿյ�Span
	while (begin != span_list.end())
	{
		// �����Span�����ڴ�� ��ֱ�ӷ���
		if (begin->list_ != nullptr)
			return begin;
		begin = begin->next_;
	}

	// ���SpanList��û�зǿյ�Span ���PageCache�л�ȡһ��Span
	PageCache* page_cache = PageCache::get_instance(); // ��ȡҳ�����Ψһʵ��
	Span* span = page_cache->new_span(SizeClass::num_move_page(align_size)); // ��ҳ�����ȡһ��Span
	assert(span != nullptr);

	char* start = (char*)(span->page_id_ << PAGE_SHIFT);	// ҳ��ת��Ϊ��ʼ��ַ
	size_t bytes = span->page_num_ << PAGE_SHIFT;			// �ڴ���ܴ�С
	char* end = start + bytes;								// ������ַ

	// ������ڴ��зֳ�С���ڴ�
	size_t num = bytes / align_size;						// �ڴ�����
	char* cur = start;								// ��ǰλ��
	char* next = cur + align_size;						// ��һ��λ��
	for (size_t i = 0; i < num - 1; ++i)
	{
		next_obj(cur) = next;	// ����һ��λ�ùҵ���ǰλ��
		cur = next;				// �ƶ���ǰλ��
		next = cur + align_size;		// �ƶ���һ��λ��
	}
	next_obj(cur) = nullptr;		// ���һ��λ��ָ��nullptr

	span->list_ = start;	// span��list_ָ���ڴ������
	
	// ��span���뵽SpanList��
	//span_list.get_mutex().lock();	// ����
	span_list.push_front(span);		// ���뵽SpanList��
	//span_list.get_mutex().unlock(); // ����

	return span;
}

