#pragma once

#include "Common.h"

class ThreadCache
{
public:
    // 申请内存
    void *allocate(size_t size);

    // 释放内存
    void deallocate(void *ptr, size_t size);

    // 从中心缓存获取内存块
    void *fetch_from_central_cache(size_t size, size_t index);

private:
    FreeList free_list_[NUM_LISTS]; // 自由链表数组 - 哈希桶
};

static thread_local ThreadCache* tls_thread_cache;