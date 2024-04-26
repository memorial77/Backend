#pragma once

#include "Common.h"
#include "CentralCache.h"

class PageCache
{
public:
    // 获取PageCache实例
    static PageCache *get_instance();

    Span *new_span(size_t pages); // 获取页数为pages的span

    std::mutex page_mutex_; // 互斥锁

private:
    SpanList span_lists_[NUMS_PAGE]; // 128个Span链表，对应页数为1~128页

private:
    PageCache() {}                                    // 构造函数私有化
    PageCache(const PageCache &) = delete;            // 删除拷贝构造函数
    PageCache &operator=(const PageCache &) = delete; // 删除赋值运算符
};