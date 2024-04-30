#pragma once

#include "Common.h"
#include "ThreadCache.h"

// 从线程缓存中分配内存
void* con_current_alloc(size_t size);

// 将内存块释放到线程缓存
void con_current_dealloc(void* ptr);