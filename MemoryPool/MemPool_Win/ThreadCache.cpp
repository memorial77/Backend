#include "ThreadCache.h"
#include "CentralCache.h"
#include <algorithm>

_declspec(thread) ThreadCache* tls_thread_cache = nullptr;

// ���̻߳����з����ڴ�
void* ThreadCache::alloc(size_t size)
{
	assert(size <= MAX_SIZES);						// ������ڴ��С���ܳ���MAX_SIZES = 256KB  
	size_t align_size = SizeClass::round_up(size);	// �ڴ����
	size_t index = SizeClass::index(align_size);	// �����ڴ��С��ȡ�̻߳�������������������±�

	if (thread_cache_free_list_[index].empty())
	{
		// ����̻߳����ж�Ӧ����������Ϊ��������Ļ����Ӧ��SpanList��ȡ�ڴ��
		return fetch_from_central_cache(index, align_size);
	}
	else
	{
		// �̻߳����ж�Ӧ���������������ڴ�飬ֱ��ȡ��
		return thread_cache_free_list_[index].pop();
	}
}

// �����Ļ����Ӧ��SpanList��ȡ�ڴ��
void* ThreadCache::fetch_from_central_cache(size_t index, size_t align_size)
{
	// ����ʼ���ԣ�ÿ�δ����Ļ����ȡ���ڴ������������
	size_t batch_size = SizeClass::num_move_size(align_size) > thread_cache_free_list_[index].get_count() ?
		thread_cache_free_list_[index].get_count() : SizeClass::num_move_size(align_size);

	if (batch_size == thread_cache_free_list_[index].get_count())
		thread_cache_free_list_[index].get_count()++;	// �����ӻ�ȡ���ڴ������
	else
		thread_cache_free_list_[index].get_count() = 1;	// ����Ϊ1

	// ��ȡ���Ļ����Ψһʵ��
	CentralCache* central_cache = CentralCache::get_instance();
	assert(central_cache != nullptr);

	// �����Ļ����ȡһ�������Ķ�����̻߳���
	void* start = nullptr;	// ��ȡ���ڴ�����ʼ��ַ
	void* end = nullptr;	// ��ȡ���ڴ��Ľ�����ַ

	// ��ȡ��ʵ���ڴ��ĸ���
	size_t actual_size = central_cache->fetch_range_objs(start, end, batch_size, align_size);
	assert(actual_size > 0); // ��ȡ���ڴ������������0

	// debug
	std::cout << "fetch_from_central_cache: " << actual_size << std::endl;

	if (actual_size == 1)
	{
		// �����ȡ���ڴ�����Ϊ1����ֱ�ӷ���
		assert(start == end);
		return start;
	}
	else
	{
		// �����ȡ���ڴ���������1���򽫻�ȡ���ڴ����뵽�̻߳��������������
		thread_cache_free_list_[index].push_range(start, end, actual_size);
		return thread_cache_free_list_[index].pop();
	}
}


// ���ڴ���ͷŵ��̻߳���
void ThreadCache::dealloc(void* ptr, size_t size)
{
	assert(ptr != nullptr);
	assert(size <= MAX_SIZES);

	// ��Ҫ�ͷŵ��ڴ����뵽�̻߳��������������
	size_t index = SizeClass::index(size);
	thread_cache_free_list_[index].push(ptr);
}