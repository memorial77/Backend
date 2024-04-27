#include "Common.h"
#include "ThreadCache.h"
#include "ConCurrentAlloc.h"
#include <thread>
#include <chrono>

int main()
{
	//// ≤‚ ‘œﬂ≥Ãª∫¥Ê
	//std::thread t1([]()
	//	{
	//		for (size_t i = 0; i < 100; ++i)
	//		{
	//			void* ptr = con_current_alloc(1);
	//			std::cout << "t1: " << ptr << std::endl;
	//			std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	//		}
	//	}
	//);

	//std::thread t2([]()
	//	{
	//		for (size_t i = 0; i < 100; ++i)
	//		{
	//			void* ptr = con_current_alloc(1);
	//			std::cout << "t2: " << ptr << std::endl;
	//			std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	//		}
	//	}
	//);

	while (true)
	{
		void* ptr = con_current_alloc(256 *1024);
		std::cout << "main: " << ptr << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}
