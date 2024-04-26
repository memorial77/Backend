#include "CentralCache.h"

// 获取中心缓存实例
CentralCache *CentralCache::get_instance()
{
    // C++11标准保证局部静态变量的初始化是线程安全的
    static CentralCache instance; // 局部静态变量
    return &instance;
}

// 从中心缓存取出内存块存入线程缓存
size_t CentralCache::fetch_range_objs(void *&start, void *&end, size_t batch_num, size_t align_size)
{
    // 计算内存块大小对应的中心缓存中span链表数组下标
    size_t index = SizeClass::index(align_size);

    // 桶锁 - 保护span_list_[index]用于线程安全
    // span_list_[index].get_mutex().lock();

    std::cout << "[已经进入中心缓存部分，需要从对应的span链表中取出一个非空span]" << std::endl;

    Span *span = get_one_span(span_list_[index], align_size);
    std::cout << "span: " << span << std::endl;
    assert(span); // span不能为空
    // 这里的问题 返回span为nullptr TODO
    std::cout << "span->free_list_: " << span->free_list_ << std::endl;
    assert(span->free_list_); // free_list不能为空

    start = end = span->free_list_; // 第一块内存块的起始地址
    size_t count = 1;               // 实际获取的内存块数量
    while (count < batch_num && next_obj(end) != nullptr)
    {
        end = next_obj(end); // 下一内存块地址
        count++;
    }
    span->free_list_ = next_obj(end); // 更新span的free_list
    next_obj(end) = nullptr;          // 断开内存块之间的连接
    span->used_count_ += count;       // 更新span的使用计数

    span_list_[index].get_mutex().unlock(); // 解锁

    return count;
}

// align_size: 内存块大小 span_list: 存储不同大小的内存块的链表
Span *CentralCache::get_one_span(SpanList &span_list, size_t align_size)
{
    std::cout << "[已经进入中心缓存对应的span链表]" << std::endl;
    // 查看span_list中是否有非空的span
    Span *it = span_list.begin();
    while (it != span_list.end())
    {
        // 从span_list中找到一个非空的span
        if (it->free_list_ != nullptr)
        {
            std::cout << "[从span_list中找到一个非空的span，返回span]" << std::endl;
            return it;
        }
        it = it->next_;
    }

    // 先把span_list的互斥锁解锁，便于其他线程释放内存时获取span_list的互斥锁
    // span_list.get_mutex().unlock(); // 解锁

    // 没有找到非空的span则从page_cache中获取一个span
    std::cout << "[span链表中没有找到非空的span，将从page_cache中获取一个span]" << std::endl;
    PageCache *page_cache = PageCache::get_instance();                       // 获取PageCache实例
    Span *span = page_cache->new_span(SizeClass::num_move_page(align_size)); // 从page_cache中获取一个span
    std::cout << "[从page_cache中获取一个span]" << "span: " << span << std::endl;
    // page_cache->page_mutex_.unlock(); // 解锁page_cache的互斥锁

    void *start_address = (void *)(span->page_id_ << PAGE_SHIFT); // span起始地址
    std::cout << "start_address: " << start_address << std::endl;
    size_t span_bytes = span->page_nums_ << PAGE_SHIFT; // span字节数
    std::cout << "span_bytes: " << span_bytes << std::endl;
    void *end_address = (void *)((char *)start_address + span_bytes); // span结束地址

    // 将新获取的span切割成多个内存块并存入中心缓存span的自由链表span_list中
    void *cur = start_address;
    while ((char *)cur + align_size <= (char *)end_address)
    {
        void *next = (char *)cur + align_size; // 下一个内存块地址
        next_obj(cur) = next;                  // 将下一个内存块地址存入当前内存块
        cur = next;                            // 移动到下一个内存块
    }
    next_obj(cur) = nullptr; // 最后一个内存块的next指针置空

    span_list.get_mutex().lock(); // 加锁
    // 更新span的free_list
    span->free_list_ = start_address;
    // 将span存入span_list
    span_list.push_front(span);
    span_list.get_mutex().unlock(); // 解锁

    return span;
}
