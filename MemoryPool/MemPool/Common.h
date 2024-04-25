#pragma once

#include <sys/mman.h>
#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <cstdint>
#include <algorithm>

static const size_t MAX_BYTES = 256 * 1024; // 最大内存块大小
static const size_t NUM_LISTS = 208;        // 链表数组大小

// 返回当前内存块中存储的下一内存块的指针
static void *&next_obj(void *obj) { return *((void **)obj); }

// 自由链表类
class FreeList
{
public:
    // 默认构造函数
    FreeList() = default;

    // 插入内存块 - 头插
    void push(void *obj)
    {
        assert(obj);

        next_obj(obj) = free_list_;
        free_list_ = obj;
    }

    // 弹出内存块 - 头删
    void *pop()
    {
        assert(free_list_ != nullptr);

        void *obj = free_list_;
        free_list_ = next_obj(free_list_);
        return obj;
    }

    // 判断自由链表是否为空
    bool empty() { return free_list_ == nullptr; }

    // 将从中心缓存获取的内存块存入自由链表
    void push_range(void *start, void *end, size_t num)
    {
        assert(start != nullptr);
        assert(end != nullptr);

        next_obj(end) = free_list_;
        free_list_ = start;
    }

    size_t &max_size() { return max_size_; }

private:
    void *free_list_ = nullptr; // 自由链表头指针
    size_t max_size_ = 1;       // 获取内存块数量的最大值
};

// 内存块相关功能类
class SizeClass
{
public:
    // 整体控制在最多10%左右的内碎片浪费
    // [1,128]              8byte对齐           freelist[0,16)
    // [128+1,1024]         16byte对齐          freelist[16,72)
    // [1024+1,8*1024]      128byte对齐         freelist[72,128)
    // [8*1024+1,64*1024]   1024byte对齐        freelist[128,184)
    // [64*1024+1,256*1024] 8*1024byte对齐      freelist[184,208)

    // 内存块分区界限
    static const size_t SMALL_SIZE = 128;             // 对应自由链表下标0-15 对齐大小8
    static const size_t MEDIUM_SIZE = 1024;           // 对应自由链表下标16-71 对齐大小16
    static const size_t LARGE_SIZE = 8 * 1024;        // 对应自由链表下标72-127 对齐大小128
    static const size_t EXTRA_LARGE_SIZE = 64 * 1024; // 对应自由链表下标128-183 对齐大小1024
    static const size_t HUGE_SIZE = 256 * 1024;       // 对应自由链表下标184-207 对齐大小8*1024

    // 对齐大小的常量
    static const size_t SMALL_ALIGN = 8;
    static const size_t MEDIUM_ALIGN = 16;
    static const size_t LARGE_ALIGN = 128;
    static const size_t EXTRA_LARGE_ALIGN = 1024;
    static const size_t HUGE_ALIGN = 8 * 1024;

    // 自由链表起始下标常量
    static const size_t SMALL_START_INDEX = 0;
    static const size_t MEDIUM_START_INDEX = 16;
    static const size_t LARGE_START_INDEX = 72;
    static const size_t EXTRA_LARGE_START_INDEX = 128;
    static const size_t HUGE_START_INDEX = 184;

    // 计算内存块大小对应的自由链表下标
    static size_t index(size_t size)
    {
        if (size <= SMALL_SIZE)
            return (size + SMALL_ALIGN - 1) / SMALL_ALIGN - 1;
        else if (size <= MEDIUM_SIZE)
            return (size - SMALL_SIZE + MEDIUM_ALIGN - 1) / MEDIUM_ALIGN + 16 - 1;
        else if (size <= LARGE_SIZE)
            return (size - MEDIUM_SIZE + LARGE_ALIGN - 1) / LARGE_ALIGN + 72 - 1;
        else if (size <= EXTRA_LARGE_SIZE)
            return (size - LARGE_SIZE + EXTRA_LARGE_ALIGN - 1) / EXTRA_LARGE_ALIGN + 128 - 1;
        else
            return (size - EXTRA_LARGE_SIZE + HUGE_ALIGN - 1) / HUGE_ALIGN + 184 - 1;
    }

    // 内存对齐辅助函数
    static size_t align_up(size_t size, size_t align)
    {
        return (size + align - 1) & ~(align - 1);
    }

    // 内存对齐
    static size_t round_up(size_t size)
    {
        if (size <= SMALL_SIZE)
            return align_up(size, SMALL_ALIGN);
        else if (size <= MEDIUM_SIZE)
            return align_up(size, MEDIUM_ALIGN);
        else if (size <= LARGE_SIZE)
            return align_up(size, LARGE_ALIGN);
        else if (size <= EXTRA_LARGE_SIZE)
            return align_up(size, EXTRA_LARGE_ALIGN);
        else if (size <= HUGE_SIZE)
            return align_up(size, HUGE_ALIGN);
        else
        {
            std::cerr << "allocate size too large" << std::endl;
            return 0;
        }
    }

    // 自由链表下标对应的内存块大小
    static size_t size(size_t index)
    {
        if (index < MEDIUM_START_INDEX)
            return (index + 1) * SMALL_ALIGN;
        else if (index < LARGE_START_INDEX)
            return (index - MEDIUM_START_INDEX + 1) * MEDIUM_ALIGN + SMALL_SIZE;
        else if (index < EXTRA_LARGE_START_INDEX)
            return (index - LARGE_START_INDEX + 1) * LARGE_ALIGN + MEDIUM_SIZE;
        else if (index < HUGE_START_INDEX)
            return (index - EXTRA_LARGE_START_INDEX + 1) * EXTRA_LARGE_ALIGN + LARGE_SIZE;
        else
            return (index - HUGE_START_INDEX + 1) * HUGE_ALIGN + EXTRA_LARGE_SIZE;
    }

    // 从中心缓存获取内存块数量
    static size_t num_move_size(size_t size)
    {
        assert(size > 0);
        if (size <= SMALL_SIZE)
            return 512;
        else if (size <= MEDIUM_SIZE)
            return 64;
        else if (size <= LARGE_SIZE)
            return 8;
        else if (size <= EXTRA_LARGE_SIZE)
            return 4;
        else
            return 2;
    }
};

class Span
{
public:
    Span()
        : page_id_(0), num_(0), next_(nullptr),
          prev_(nullptr), used_count_(0), free_list_(nullptr) {}

public:
    // 页id - 用于标识页
#if UINTPTR_MAX == 0xffffffff // 条件编译
    // 32位系统使用size_t
    size_t page_id_;
#elif UINTPTR_MAX == 0xffffffffffffffff
    // 64位系统使用unsigned long
    unsigned long page_id_;
#else
#error "Platform not supported"
#endif

    size_t num_; // 页内内存块数量
    Span *next_; // 下一Span指针
    Span *prev_; // 上一Span指针

    size_t used_count_; // 已分配内存块数量
    void *free_list_;   // 内存切割后链接方便管理
};

// 带头双向循环链表 - 管理span
class SpanList
{
public:
    SpanList()
    {
        head_ = new Span;
        head_->next_ = head_;
        head_->prev_ = head_;
    }

    void insert(Span *pos, Span *span)
    {
        assert(pos != nullptr);
        assert(span != nullptr);

        Span *prev = pos->prev_;

        prev->next_ = span;
        span->prev_ = prev;
        span->next_ = pos;
        pos->prev_ = span;
    }

    void erase(Span *pos)
    {
        assert(pos != nullptr);
        assert(pos != head_); // 不允许删除头结点

        pos->prev_->next_ = pos->next_;
        pos->next_->prev_ = pos->prev_;
    }

    std::mutex &get_mutex() { return mutex_; }

private:
    Span *head_;       // 头指针
    std::mutex mutex_; // 互斥锁 - 桶锁
};