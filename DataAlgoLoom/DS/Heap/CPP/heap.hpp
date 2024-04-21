#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cassert>

template <class T>
class Heap
{
    typedef T val_type;

public:
    // 默认构造函数
    Heap() : data_(nullptr), size_(0), capacity_(0) {}

    // 析构函数
    ~Heap()
    {
        delete[] data_;
        size_ = 0;
        capacity_ = 0;
    }

    // 判空
    bool empty() { return size_ == 0; }

    // 交换
    bool swap(val_type *val1_p, val_type *val2_p)
    {
        val_type temp = *val1_p;
        *val1_p = *val2_p;
        *val2_p = temp;
    }

    // 向上调整
    void adjust_up()
    {
        // 获取父子节点下标
        size_t child = size_ - 1;
        size_t parent = (child - 1) / 2;

        while (child > 0)
        {
            // 此处建立大根堆，符合条件则交换父子位置
            if (data_[child] > data_[parent])
            {
                swap(&data_[child], &data_[parent]);

                // 更新父子节点下标
                child = parent;
                parent = (child - 1) / 2;
            }
            else
                // 符合大根堆条件则退出
                break;
        }
    }

    // 向下调整
    void adjust_down()
    {
        size_t parent = 0;
        size_t max_child = parent * 2 + 1;

        while (max_child < size_)
        {
            // 若右孩子存在并且大于左孩子则更新最大子孩子
            if (max_child + 1 < size_ && data_[max_child + 1] > data_[max_child])
                max_child++;

            if (data_[max_child] > data_[parent])
            {
                swap(&data_[max_child], &data_[parent]);
                parent = max_child;
                max_child = parent * 2 + 1;
            }
            else
                break;
        }
    }

    // 插入数据
    void push(val_type val)
    {
        // 容量不足或者无容量
        if (size_ == capacity_)
        {
            size_t new_capacity = (capacity_ == 0) ? 4 : capacity_ * 2;
            auto *temp = new val_type[new_capacity];

            // 拷贝数据
            memcpy(temp, data_, sizeof(val_type) * new_capacity);
            delete[] data_;
            data_ = temp;
            capacity_ = new_capacity;
        }

        // 插入数据
        data_[size_++] = val;
        // 调整堆结构
        adjust_up();
    }

    // 删除数据
    void pop()
    {
        // 为空不能删除
        assert(!empty());

        // 交换首尾元素
        swap(&data_[0], &data_[size_ - 1]);
        size_--;

        // 向下调整结构
        adjust_down();
    }

    // 返回堆顶数据
    val_type top()
    {
        assert(!empty());
        return data_[size_ - 1];
    }

    // 数据个数
    size_t size() { return size_; }

    // 打印数据
    void print()
    {
        printf("Heap: [");
        for (size_t i = 0; i < size_; i++)
        {
            printf("%d", data_[i]);
            if (i < size_ - 1)
                printf(", ");
        }
        printf("]\n");
    }

private:
    val_type *data_;
    size_t size_;
    size_t capacity_;
};