// #include "ObjectPool.h"

// class MyClass {
// public:
//     MyClass() {
//         std::cout << "MyClass constructed" << std::endl;
//     }
//     ~MyClass() {
//         std::cout << "MyClass destructed" << std::endl;
//     }
//     void DoSomething() {
//         std::cout << "MyClass doing something" << std::endl;
//     }
// };

// int main() {
//     // 创建对象池
//     ObjectPool<MyClass> pool;

//     // 从对象池中分配对象
//     MyClass *obj1 = pool.New();
//     MyClass *obj2 = pool.New();

//     // 使用对象
//     obj1->DoSomething();
//     obj2->DoSomething();

//     // 释放对象
//     pool.Delete(obj1);
//     pool.Delete(obj2);

//     return 0;
// }

#include <iostream>
#include <chrono>
#include "ObjectPool.h"

class MyClass
{
public:
    MyClass() {}
    ~MyClass() {}
    void DoSomething() {}
};

int main()
{
    const int num_iterations = 1000000;

    // 测试使用普通的 new/delete
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < num_iterations; ++i)
    {
        MyClass *obj = new MyClass();
        delete obj;
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time taken for new/delete: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " milliseconds" << std::endl;

    // 测试使用对象池
    start = std::chrono::steady_clock::now();
    ObjectPool<MyClass> pool;
    for (int i = 0; i < num_iterations; ++i)
    {
        MyClass *obj = pool.New();
        pool.Delete(obj);
    }
    end = std::chrono::steady_clock::now();
    std::cout << "Time taken for object pool: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " milliseconds" << std::endl;

    return 0;
}
