#pragma once

#include "Common.h"
#include "ThreadCache.h"

// ���̻߳����з����ڴ�
void* con_current_alloc(size_t size);

// ���ڴ���ͷŵ��̻߳���
void con_current_dealloc(void* ptr, size_t size);