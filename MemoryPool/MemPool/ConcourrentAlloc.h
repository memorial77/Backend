#pragma once

#include "Common.h"
#include "ThreadCache.h"

// 申请内存
static void *con_current_alloc(size_t size)
{
    if (size > MAX_BYTES)
        // 超过最大内存块大小，直接调用系统接口申请内存
        return malloc(size);
    else
    {
        // 从线程缓存中获取内存
        if (tls_thread_cache == nullptr) 
            tls_thread_cache = new ThreadCache;

        return tls_thread_cache->allocate(size);
    }
}

// 释放内存
static void *con_current_free(size_t size, void *ptr)
{
    assert(tls_thread_cache);
    assert(ptr);
    
    if (size > MAX_BYTES)
        // 超过最大内存块大小，直接调用系统接口释放内存
        free(ptr);
    else
        // 从线程缓存中释放内存
        tls_thread_cache->deallocate(ptr, size);
}
