#include <iostream>
#include "ThreadPool.h"

void TestThreadPool()
{
	ThreadPool threadPool(2);

	std::thread thd1([&threadPool] 
	{
		for (size_t i = 0; i < 10; i++)
		{
			auto thdId = std::this_thread::get_id();
			threadPool.AddTask([thdId]
			{
				std::cout << "同步层线程：" << thdId << std::endl;
			});
		}
	});

	std::thread thd2([&threadPool]
	{
		for (size_t i = 0; i < 10; i++)
		{
			auto thdId = std::this_thread::get_id();
			threadPool.AddTask([thdId]
			{
				std::cout << "同步层线程：" << thdId << std::endl;
			});
		}
	});
	
	std::this_thread::sleep_for(std::chrono::seconds(5));
	getchar();
	threadPool.Stop();
	thd1.join();
	thd2.join();
}


int main()
{
	TestThreadPool();
	return 0;
}

