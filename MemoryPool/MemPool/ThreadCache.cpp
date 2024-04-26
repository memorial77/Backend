#include "ThreadCache.h"
#include "Common.h"
#include "CentralCache.h"

// 申请内存
void *ThreadCache::allocate(size_t size)
{
    // 计算对齐内存块大小方便申请
    size_t align_size = SizeClass::round_up(size);
    // 计算内存块大小对应的线程缓存中自由链表数组下标
    size_t index = SizeClass::index(align_size);

    if (free_list_[index].empty())
    {
        std::cout << "[线程申请的内存块对应的线程缓存中的自由链表无剩余内存块，需要向中心缓存获取内存块]" << std::endl;
        // 从中心缓存获取内存块
        return fetch_from_central_cache(align_size, index);
    }
    else
        // 从自由链表头部获取内存块
        return free_list_[index].pop();
}

// 释放内存
void ThreadCache::deallocate(void *ptr, size_t size)
{
    // 计算内存块大小对应的自由链表下标
    size_t index = SizeClass::index(size);
    // 将内存块插入自由链表
    free_list_[index].push(ptr);
}

// 从中心缓存获取内存
void *ThreadCache::fetch_from_central_cache(size_t align_size, size_t index)
{
    // 从中心缓存获取一定数量的内存块存入自由链表，减少锁的竞争
    size_t batch_num = std::min(SizeClass::num_move_size(align_size),
                                free_list_[index].max_size());

    // 首次为1个内存块，之后逐渐增加
    // 慢开始反馈调节算法 - 适应性调节
    if (batch_num == free_list_[index].max_size())
        free_list_[index].max_size() += 1;

    void *start = nullptr, *end = nullptr;                      // 内存块起始地址和结束地址
    CentralCache *central_cache = CentralCache::get_instance(); // 获取中心缓存实例

    // 实际获取的内存块数量
    size_t actual_num = central_cache->fetch_range_objs(start, end, batch_num, align_size);
    assert(actual_num > 0);

    // 将内存块存入自由链表
    free_list_[index].push_range(start, end, actual_num);
    // 返回内存块
    return free_list_[index].pop();
}
