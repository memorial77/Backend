#pragma once

#include <sys/mman.h>
#include <iostream>
#include <cassert>
#include <thread>

static const size_t MAX_BYTES = 256 * 1024; // 最大内存块大小

// 自由链表类
class FreeList
{
public:
    // 默认构造函数
    FreeList() = default;

    // 返回当前内存块中存储的下一内存块的指针
    void *&next_obj(void *obj)
    {
        return *((void **)obj);
    }
    
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
    bool empty()
    {
        return free_list_ == nullptr;
    }

private:
    void *free_list_ = nullptr; // 自由链表头指针
};

// 计算内存块大小对应的自由链表下标
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
};