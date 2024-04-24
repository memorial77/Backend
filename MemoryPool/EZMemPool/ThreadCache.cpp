#include "ThreadCache.h"
#include "Common.h"

// 申请内存
void *ThreadCache::allocate(size_t size)
{
    assert(size <= MAX_BYTES); // 申请内存大小不能超过最大内存块大小
    size_t align_size = SizeClass::round_up(size); // 对齐
    size_t index = SizeClass::index(align_size); // 计算内存块大小对应的自由链表下标

    if(free_list_[index].empty())
        // 从中心缓存获取内存块
        return fetch_from_central_cache(align_size, index);
    else
        // 从自由链表头部获取内存块
        return free_list_[index].pop();
}

// 释放内存
void ThreadCache::deallocate(void *ptr, size_t size)
{
}

// 从中心缓存获取内存
void *ThreadCache::fetch_from_central_cache(size_t size, size_t index)
{
    return nullptr;
}
