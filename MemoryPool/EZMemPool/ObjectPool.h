#pragma once

#include <sys/mman.h>
#include <iostream>

template <class T>
class ObjectPool
{
public:
    // 初始化构造函数（可指定内存块大小）
    ObjectPool(size_t block_size = 128 * 1024)
        : memory_(nullptr), remain_size_(0), free_list_(nullptr), block_size_(block_size) {}

    T *New()
    {
        T *obj = nullptr;

        // 表明存在申请后释放的空间可使用
        if (free_list_ != nullptr)
        {
            // 下一内存块地址
            void *next = *((void **)free_list_);
            obj = (T *)free_list_;
            free_list_ = next;
        }
        else
        {
            // 剩余空间不足以开辟新对象
            if (remain_size_ < sizeof(T) || remain_size_ < sizeof(void *))
            {
                // 使用mmap系统调用分配内存
                memory_ = (char *)mmap(nullptr, block_size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                if (memory_ == MAP_FAILED)
                    throw std::bad_alloc();

                remain_size_ = block_size_;
            }

            // 将内存块转化为对象指针
            obj = (T *)memory_;
            size_t obj_size = sizeof(T) < sizeof(void *) ? sizeof(void *) : sizeof(T);
            // 更新内存指针和内存带大小
            memory_ += obj_size;
            remain_size_ -= obj_size;
        }

        // 定位new
        new (obj) T;

        return obj;
    }

    void Delete(T *obj)
    {
        // 调用析构函数清理T对象内空间
        obj->~T();
        // 首次释放切出去的内存
        if (free_list_ == nullptr)
        {
            free_list_ = (void *)obj;
            // 利用二级指针解引用自适应32位或64位系统
            *(void **)obj = nullptr;
        }
        else
        {
            // 不是首次则进行头插提高效率
            *(void **)obj = free_list_;
            free_list_ = (void *)obj;
        }
    }

private:
    char *memory_;       // 内存块指针
    size_t remain_size_; // 剩余内存大小
    void *free_list_;    // 空闲对象链表
    size_t block_size_;  // 内存块大小
};
