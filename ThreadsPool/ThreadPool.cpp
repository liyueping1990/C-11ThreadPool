#include "ThreadPool.h"

ThreadPool::ThreadPool(int numThreads) : mQueue(MAX_TASK)
{
	Start(numThreads);
}

ThreadPool::~ThreadPool()
{
}

void ThreadPool::Stop()
{
	std::call_once(mFlag, [this] {StopThreadGroup(); });
}

void ThreadPool::AddTask(Task && task)
{
	mQueue.Put(std::forward<Task>(task));
}

void ThreadPool::AddTask(const Task& task)
{
	mQueue.Put(task);
}

void ThreadPool::Start(int numThreads)
{
	mRunning = true;
	for (int i = 0; i < numThreads; ++i)
	{
		mThreadGroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
	}
}

void ThreadPool::RunInThread()
{
	while (mRunning)
	{
		Task oneTask;
		mQueue.Take(oneTask);

		if (!mRunning)
		{
			return;
		}
		oneTask();
	}
}

void ThreadPool::StopThreadGroup()
{
	mQueue.Stop();
	mRunning = false;

	for (auto thread : mThreadGroup)
	{
		if (thread)
		{
			thread->join();
		}
	}
	mThreadGroup.clear();
}
