#pragma once

#include <cstring>
#include <cassert>

namespace memorial
{
    // 定义模板类 vector
    template <class T>
    class vector
    {
    public:
        typedef T val_type;                     // 定义值类型别名
        typedef val_type *iterator;             // 定义迭代器类型别名
        typedef const val_type *const_iterator; // 定义常量迭代器类型别名

    public:
        // 默认构造函数
        vector() : start_(0), finish_(0), end_of_storge_(0) {}

        // 范围构造函数
        template <class InputIterator>
        vector(InputIterator first, InputIterator last)
            : start_(0), finish_(0), end_of_storge_(0)
        {
            while (first != last)
            {
                push_back(*first);
                first++;
            }
        }

        // 拷贝构造函数
        vector(const val_type &other)
            : start_(0), finish_(0), end_of_storge_(0)
        {
            vector<val_type> temp(other.begin(), other.end());
            swap(temp);
        }

        // 重载构造函数，初始化为n个val
        vector(size_t n, const val_type &val = val_type())
        {
            reserve(n);
            for (size_t i = 0; i < n; i++)
                push_back(val);
        }

        // 重载构造函数，初始化为n个 val
        vector(int n, const val_type &val = val_type())
        {
            reserve(n);
            for (int i = 0; i < n; i++)
                push_back(val);
        }

        // 析构函数
        ~vector()
        {
            delete[] start_;
            start_ = finish_ = end_of_storge_ = 0;
        }

        // 返回起始迭代器
        iterator begin() { return start_; }

        // 返回结束迭代器
        iterator end() { return finish_; }

        // 返回常量起始迭代器
        const_iterator begin() const { return start_; }

        // 返回常量结束迭代器
        const_iterator end() const { return finish_; }

        // 返回元素个数
        size_t size() const { return finish_ - start_; }

        // 返回容量
        size_t capacity() const { return end_of_storge_ - start_; }

        // 判断是否为空
        bool empty() const { return start_ == finish_; }

        // 清空容器
        void clear() { finish_ = start_; }

        // 扩容函数
        void reserve(size_t n)
        {
            // 不进行缩容
            if (n > capacity())
            {
                val_type *temp = new val_type[n];
                // 若不是第一次扩容则需要释放空间并拷贝数据
                if (start_)
                {
                    for (int i = 0; i < size(); i++)
                        temp[i] = start_[i];
                    delete[] start_;
                }
                // 更新容器信息，以下写法有bug
                // 更新start_后，使用size()时內部的finish_未更新导致错位
                // start_ = temp;
                // finish_ = start_ + size();
                // end_of_storge_ = start_ + n;

                // 正确写法（也可以先更新finish_）
                // 更新容器信息
                size_t count = size();
                start_ = temp;
                finish_ = start_ + count;
                end_of_storge_ = start_ + n;
            }
        }

        // 调整大小函数
        void resize(size_t n, const val_type &val = val_type())
        {
            // 判断是否需要进行扩容
            if (n > capacity())
                reserve(n);

            if (n > size())
            {
                while (finish_ != start_ + n)
                {
                    *finish_ = val;
                    finish_++;
                }
            }
            else
                finish_ = start_ + n;
        }

        // 尾插数据
        void push_back(const val_type &val)
        {
            // 容量满则进行扩容
            if (finish_ == end_of_storge_)
            {
                size_t new_capacity = (capacity() == 0) ? 4 : capacity() * 2;
                reserve(new_capacity);
            }

            // 添加数据并更新 finish_
            *finish_ = val;
            finish_++;
        }

        // 尾删数据
        void pop_back()
        {
            // 为空则不能进行删除
            assert(!empty());
            finish_--;
        }

        // 插入数据
        iterator insert(iterator pos, const val_type &val)
        {
            assert(pos >= start_);
            assert(pos <= finish_);

            if (start_ == end_of_storge_)
            {
                size_t distance = pos - start_;
                size_t new_capacity = (capacity() == 0) ? 4 : capacity() * 2;
                reserve(new_capacity);
                pos = start_ + distance; // 更新迭代器位置
            }

            iterator end = finish_ - 1;
            while (end >= pos)
            {
                *(end + 1) = *end;
                end--;
            }

            *pos = val;
            finish_++;

            return pos;
        }

        // 删除元素
        iterator erase(iterator pos)
        {
            assert(pos >= start_);
            assert(pos < finish_);
            assert(!empty());

            iterator start = pos;
            while (start < finish_ - 1)
            {
                *start = *(start + 1);
                start++;
            }
            finish_--;

            return pos;
        }

        // 交换容器内容
        void swap(vector<val_type> &other)
        {
            std::swap(start_, other.start_);
            std::swap(finish_, other.finish_);
            std::swap(end_of_storge_, other.end_of_storge_);
        }

        // 重载下标操作符
        val_type &operator[](size_t pos)
        {
            assert(pos < size());
            return start_[pos];
        }

        // 重载下标操作符
        const val_type &operator[](size_t pos) const
        {
            assert(pos < size());
            return start_[pos];
        }

        // 赋值运算符重载
        vector<val_type> &operator=(vector<val_type> other)
        {
            swap(other);
            return *this;
        }

    private:
        iterator start_;         // 起始迭代器
        iterator finish_;        // 结束迭代器
        iterator end_of_storge_; // 容量结束迭代器
    };
};
