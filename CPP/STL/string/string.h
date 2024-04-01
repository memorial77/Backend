#pragma once

#include <iostream>
#include <cstring>
#include <string.h>
#include <cassert>

namespace memorial
{
    class string
    {
    public:
        typedef char *iterator;             // 正向迭代器
        typedef const char *const_iterator; // 正向常迭代器（无法修改迭代器指向的内容）

    public:
        // 构造函数
        string(const char *str = "")
        {
            size_ = capacity_ = strlen(str);
            str_ = new char[capacity_ + 1];
            strcpy(str_, str);
        }

        // // 拷贝构造函数（传统写法）
        // string(const string &other)
        // {
        //     str_ = new char[other.capacity_ + 1];
        //     size_ = other.size_;
        //     capacity_ = other.capacity_;
        //     strcpy(str_, other.str_);
        // }

        // 拷贝构造函数（现代写法）
        string(const string &other)
            : str_(nullptr), size_(0), capacity_(0)
        {
            string temp(other.c_str());
            swap(temp);
        }

        // 析构函数
        ~string()
        {
            delete[] str_;
            str_ = nullptr;
            size_ = capacity_ = 0;
        }

        void swap(string &other)
        {
            std::swap(str_, other.str_);
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
        }

        /* ----------------------------普通对象---------------------------- */

        const char *c_str() { return str_; }

        size_t size() { return size_; }

        size_t length() { return size_; }

        size_t capacity() { return capacity_; }

        iterator begin() { return str_; }

        iterator end() { return str_ + size_; }

        void reserve(size_t n)
        {
            if (n > capacity_)
            {
                char *temp_str = new char[n + 1];
                strcpy(temp_str, str_);
                delete[] str_;
                str_ = temp_str;

                capacity_ = n;
            }
        }

        void resize(size_t n, char ch = '\0')
        {
            if (n <= size_)
            {
                size_ = n;
                str_[size_] = '\0';
            }
            else
            {
                reserve(n);
                for (int i = size_; i < n; i++)
                    str_[i] = ch;
                size_ = n;
                str_[size_] = '\0';
            }
        }

        void push_back(char ch)
        {
            if (size_ == capacity_)
            {
                if (capacity_ == 0)
                    reserve(4);
                else
                    reserve(capacity_ * 2);
            }

            str_[size_++] = ch;
            str_[size_] = '\0';
        }

        void append(const char *str)
        {
            size_t len = strlen(str);
            if (len + size_ > capacity_)
                reserve(len + size_);

            strcpy(str_ + size_, str);
            size_ += len;
        }

        string &operator+=(char ch)
        {
            push_back(ch);
            return *this;
        }

        string &operator+=(const char *str)
        {
            append(str);
            return *this;
        }

        string &insert(size_t pos, char ch)
        {
            assert(pos <= size_);
            if (size_ == capacity_)
            {
                if (capacity_ == 0)
                    reserve(4);
                else
                    reserve(capacity_ * 2);
            }

            for (size_t end = size_ + 1; end > pos; end--)
                str_[end] = str_[end - 1];

            str_[pos] = ch;
            size_++;
            return *this;
        }

        string &insert(size_t pos, const char *str)
        {
            assert(pos <= size_);
            size_t len = strlen(str);
            if (len + size_ > capacity_)
                reserve(len + size_);

            // 连带'\0'一同处理
            for (size_t end = size_ + 1; end > pos; end--)
                str_[end + len - 1] = str_[end - 1];

            strncpy(str_ + pos, str, len);
            size_ += len;
            return *this;
        }

        string &erase(size_t pos, size_t len = npos)
        {
            assert(pos < size_);
            if (len == npos || len + pos >= size_)
            {
                size_ = pos;
                str_[size_] = '\0';
            }
            else
            {
                strcpy(str_ + pos, str_ + pos + len);
                size_ -= len;
            }

            return *this;
        }

        size_t find(const char ch, size_t pos = 0) const
        {
            assert(pos < size_);
            while (pos < size_)
            {
                if (str_[pos] == ch)
                    return pos;
            }

            return npos;
        }

        size_t find(const char *str, size_t pos = 0) const
        {
            assert(pos < size_);
            const char *ptr = strstr(str_ + pos, str);
            if (ptr != nullptr)
                return ptr - str_;
            else
                return npos;
        }

        char &operator[](size_t pos)
        {
            assert(pos < size_);
            return str_[pos];
        }

        // // 赋值重载（传统写法）
        // string &operator=(const string &other)
        // {
        //     if (this != &other)
        //     {
        //         delete[] str_;
        //         str_ = new char[other.capacity_ + 1];
        //         size_ = other.size_;
        //         capacity_ = other.capacity_;
        //         strcpy(str_, other.str_);
        //     }
        //     return *this;
        // }

        // // 赋值重载（现代写法）
        // string &operator=(const string &other)
        // {
        //     if (this != &other)
        //     {
        //         string temp(other.c_str());
        //         swap(temp);
        //     }
        //     return *this;
        // }

        string &operator=(string other)
        {
            swap(other);
            return *this;
        }

        /* ----------------------------const对象---------------------------- */

        const char *c_str() const { return str_; }

        size_t size() const { return size_; }

        size_t length() const { return size_; }

        size_t capacity() const { return capacity_; }

        const_iterator cbegin() const { return str_; }

        const_iterator cend() const { return str_ + size_; }

        const char &operator[](size_t pos) const
        {
            assert(pos < size_);
            return str_[pos];
        }

    private:
        char *str_;
        size_t size_;
        size_t capacity_;

        const static size_t npos = -1;
    };

    std::ostream &operator<<(std::ostream &out, const string &str)
    {
        for (size_t i = 0; i < str.size(); i++)
            out << str[i];
        return out;
    }

    std::istream &operator>>(std::istream &in, string &str)
    {
        char buffer[128] = {'\0'};
        size_t index = 0;
        char ch = in.get();
        while (ch != ' ' && ch != '\n')
        {
            if (index == 126)
            {
                buffer[127] = '\0';
                str += buffer;
                index = 0;
            }

            buffer[index++] = ch;
            ch = in.get();
        }

        if (index > 0)
        {
            buffer[index] = '\0';
            str += buffer;
        }

        return in;
    }
}