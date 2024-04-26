#include "PageCache.h"

// 获取PageCache实例
PageCache *PageCache::get_instance()
{
    static PageCache page_cache;
    return &page_cache;
}

// 获取页数为pages的span
Span *PageCache::new_span(size_t pages)
{
    std::cout << "进入PageCache::new_span" << std::endl;
    assert(pages > 0 && pages < NUMS_PAGE); // 页数范围为1~128
    // 计算页数对应的span链表下标
    size_t index = pages;
    // 加锁
    // page_mutex_.lock();

    if (!span_lists_[index].empty())
        return span_lists_[index].pop_front(); // 非空从span链表中获取一个span

    // 检查后面的链表中有无非空span
    for (size_t i = index + 1; i < NUMS_PAGE; i++)
    {
        if (!span_lists_[i].empty())
        {
            // 从后面的链表中获取一个span
            Span *span = span_lists_[i].pop_front();
            // std::cout << "获得一个span:" << span << std::endl;

            Span *new_span = new Span;
            // 切割span
            new_span->page_id_ = span->page_id_;
            new_span->page_nums_ = pages;
            // 更新原span信息
            span->page_id_ += pages;
            span->page_nums_ -= pages;

            // 将新span插入到对应的链表中
            // span_lists_[pages].push_front(new_span);
            // 原来的span插入剩余对应页数的链表中
            span_lists_[span->page_nums_].push_front(span);

            // std::cout << "new_span = " << new_span << std::endl;
            std::cout << "离开PageCache::new_span" << std::endl;
            return new_span;
        }
    }

    // 找到最后依然没有找到合适的span，从系统中获取一块新的内存，页数为NUMS_PAGE - 1
    Span *big_span = new Span;
    // 从系统中获取一块新的内存
    void *ptr = system_alloc(NUMS_PAGE - 1);
    if (ptr == nullptr)
    {
        std::cout << "申请内存失败" << std::endl;
        // page_mutex_.unlock(); // 解锁
        exit(-1);
    }
    big_span->page_id_ = reinterpret_cast<size_t>(ptr) >> PAGE_SHIFT; // 起始页号
    big_span->page_nums_ = NUMS_PAGE - 1;                             // 页数为pages

    // 插入到对应的链表中
    span_lists_[big_span->page_nums_].push_front(big_span);
    // page_mutex_.unlock(); // 在递归调用前解锁

    // 递归调用new_span函数
    return new_span(pages);
}
