#include <iostream>
#include <vector>
#include "vector.h"

// 测试Linux下vector扩容机制
int main()
{
    memorial::vector<int> ve;
    ve.pop_back();
    // std::cout << "New capacity: " << ve.capacity() << std::endl;
    // size_t capacity = ve.capacity();

    // for (int i = 0; i < 200; i++)
    // {
    //     ve.push_back(1);
    //     if (capacity != ve.capacity())
    //     {
    //         std::cout << "New capacity: " << ve.capacity() << std::endl;
    //         capacity = ve.capacity();
    //     }
    // }
}