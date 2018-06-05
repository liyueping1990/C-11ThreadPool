#ifndef _SYNC_QUEUE_H_
#define _SYNC_QUEUE_H_
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <atomic>

template<typename T>
class SyncQueue
{
public:
	SyncQueue(int maxSize);
	virtual ~SyncQueue();

	void Put(const T& t);
	void Put(T &&t);
	void Take(T &t);
	void Stop();
	bool IsEmpty();
	bool IsFull();
	size_t Size();
	size_t Count();
private:
	bool NotFull() const;
	bool NotEmpty() const;

	template <typename F>
	void Add(F &&f);

private:
	std::list<T> mQueue;
	std::mutex   mMutex;
	std::condition_variable mNotEmpty;
	std::condition_variable mNotFull;
	int mMaxSize;
	bool mNeedStop;
};

template<typename T>
SyncQueue<T>::SyncQueue(int maxSize) : mMaxSize(maxSize),mNeedStop(false)
{
}

template<typename T>
SyncQueue<T>::~SyncQueue()
{
}

template<typename T>
void SyncQueue<T>::Put(const T & t)
{
	Add(t);
}

template<typename T>
void SyncQueue<T>::Put(T && t)
{
	Add(t);
}

template<typename T>
void SyncQueue<T>::Take(T & t)
{
	std::unique_lock<std::mutex> locker(mMutex);
	mNotEmpty.wait(locker, [this] {return mNeedStop || NotEmpty(); });

	if (mNeedStop)
	{
		return;
	}

	t = mQueue.front();
	mQueue.pop_front();
	mNotFull.notify_one();
}

template<typename T>
void SyncQueue<T>::Stop()
{
	{
		std::lock_guard<std::mutex> locker(mMutex);
		mNeedStop = true;
	}
	mNotEmpty.notify_all();
	mNotFull.notify_all();
}

template<typename T>
bool SyncQueue<T>::IsEmpty()
{
	std::lock_guard<std::mutex> locker(mMutex);
	return mQueue.empty();
}

template<typename T>
bool SyncQueue<T>::IsFull()
{
	bool isFull = false;
	if (mMaxSize == mQueue.size())
	{
		isFull = true;
	}
	return isFull;
}

template<typename T>
size_t SyncQueue<T>::Size()
{
	std::lock_guard<std::mutex> locker(mMutex);
	return mQueue.size();
}

template<typename T>
size_t SyncQueue<T>::Count()
{
	return mQueue.size();
}

template<typename T>
bool SyncQueue<T>::NotFull() const
{
	bool isFull = mQueue.size() >= mMaxSize;
	if (isFull)
	{
		std::cout << "the queue is full, please wait a moment..." << std::endl;
	}
	return !isFull;
}

template<typename T>
bool SyncQueue<T>::NotEmpty() const
{
	bool isEmpty = mQueue.empty();
	if (isEmpty)
	{
		std::cout << "the queue is empty, please wait a moment..." << std::endl;
	}
	return !isEmpty;
}

template<typename T>
template<typename F>
inline void SyncQueue<T>::Add(F && f)
{
	std::unique_lock<std::mutex> locker(mMutex);
	mNotFull.wait(locker, [this] {return mNeedStop || NotFull(); });

	if (mNeedStop)
	{
		return;
	}
	mQueue.push_back(std::forward<F>(f));
	mNotEmpty.notify_one();
}
#endif //_SYNC_QUEUE_H_