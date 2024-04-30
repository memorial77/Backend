#include "Common.h"
#include "ThreadCache.h"
#include "ConCurrentAlloc.h"
#include <thread>
#include <chrono>

int main()
{
	// // 测试线程缓存
	// std::thread t1([]()
	// 	{
	// 		for (size_t i = 0; i < 100; ++i)
	// 		{
	// 			void* ptr = con_current_alloc(1);
	// 			std::cout << "t1: " << ptr << std::endl;
	// 			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	// 		}
	// 	}
	// );

	std::thread t2([]()
		{
			for (size_t i = 0; i < 100; ++i)
			{
				void* ptr = con_current_alloc(1);
				std::cout << "t2: " << ptr << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
	);

	while (true)
	{
		void* ptr = con_current_alloc(static_cast<size_t>(1));

		std::cout << "main: " << ptr << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	return 0;
}
