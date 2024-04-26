#include "ThreadCache.h"
#include "ConcourrentAlloc.h"

void alloc1()
{
    for (int i = 0; i < 10; ++i)
    {
        void *ptr = con_current_alloc(5);
        std::cout << i << "-" << ptr << std::endl;
    }
}

// void alloc2()
// {
//     for (int i = 0; i < 10; ++i)
//     {
//         void *ptr = con_current_alloc(10);
//         std::cout << "2 - " << ptr << std::endl;
//     }
// }

void test_thread()
{
    std::thread t1(alloc1);
    // std::thread t2(alloc2);

    t1.join();
    // t2.join();
}

int main()
{
    test_thread();

    return 0;
}