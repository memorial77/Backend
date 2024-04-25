#include "CentralCache.h"

// 获取中心缓存实例
CentralCache *CentralCache::get_instance()
{
    // C++11标准保证局部静态变量的初始化是线程安全的
    static CentralCache instance; // 局部静态变量
    return &instance;
}

// 从中心缓存取出内存块存入线程缓存
size_t CentralCache::fetch_range_objs(void *&start, void *&end, size_t batch_num, size_t size)
{
    // 计算内存块大小对应的自由链表下标
    size_t index = SizeClass::index(size);

    // 桶锁 - 保护span_list_[index]用于线程安全
    span_list_[index].get_mutex().lock();

    Span *span = get_one_span(span_list_[index], size);
    assert(span);             // span不能为空
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
    // TODO
    return nullptr;
}
