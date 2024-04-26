#pragma once

#include "Common.h"
#include "PageCache.h"

class CentralCache
{
public:
    // 获取中心缓存实例
    static CentralCache *get_instance();

    // 从中心缓存取出部分内存块存入线程缓存
    size_t fetch_range_objs(void *&start, void *&end, size_t batch_num, size_t size);

    // 获取非空Span
    Span* get_one_span(SpanList &span_list, size_t align_size);

private:
    SpanList span_list_[NUM_LISTS]; // 用于存储不同大小的内存块的链表

private:
    CentralCache() = default; // 构造函数私有化

    CentralCache(const CentralCache &) = delete; // 禁止拷贝构造
};
