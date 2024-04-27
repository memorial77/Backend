#pragma once

#include "Common.h"

class ThreadCache
{
public:
	// ���̻߳����з����ڴ�
	void* alloc(size_t size);

	// ���ڴ���ͷŵ��̻߳���
	void dealloc(void* ptr, size_t size);

	// �����Ļ����ȡ�ڴ��
	void* fetch_from_central_cache(size_t index, size_t align_size);

private:
	FreeList thread_cache_free_list_[FREE_LIST_SIZE]; // ������������-��ϣͰ
};

extern _declspec(thread) ThreadCache* tls_thread_cache; // �̻߳���ָ��


