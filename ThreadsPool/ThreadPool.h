#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#include <memory>
#include <functional>
#include "SyncQueue.h"

#define MAX_TASK 15

class ThreadPool
{
public:
	using Task = std::function<void()>;
	ThreadPool(int numThreads = std::thread::hardware_concurrency());
	virtual ~ThreadPool();

	void Stop();
	void AddTask(Task&& task);
	void AddTask(const Task& task);

private:
	void Start(int numThreads);
	void RunInThread();
	void StopThreadGroup();

private:
	std::list<std::shared_ptr<std::thread>> mThreadGroup;
	SyncQueue<Task> mQueue;
	std::atomic_bool mRunning;
	std::once_flag mFlag;
};

#endif //_THREAD_POOL_H_