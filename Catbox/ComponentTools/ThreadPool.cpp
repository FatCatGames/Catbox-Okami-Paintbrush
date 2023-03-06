#include "stdafx.h"
#include "ThreadPool.h"

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(myQueueMutex);
        myShouldTerminate = true;
    }
    myMutexCondition.notify_all();
    for (std::thread& active_thread : myThreads)
    {
        active_thread.join();
    }
    myThreads.clear();
}

void ThreadPool::Start()
{
	const uint32_t numberOfThreads = std::thread::hardware_concurrency();
	myThreads.resize(numberOfThreads);
	for (size_t i = 0; i < numberOfThreads; i++)
	{
		myThreads.at(i) = std::thread(&ThreadPool::ThreadLoop, this);
	}
}

void ThreadPool::QueueJob(const std::function<void()>& job)
{
    {
        std::unique_lock<std::mutex> lock(myQueueMutex);
        myJobs.push(job);
    }
    myMutexCondition.notify_one();
}


bool ThreadPool::Busy()
{
    bool poolbusy;
    {
        std::unique_lock<std::mutex> lock(myQueueMutex);
        poolbusy = myJobs.empty();
    }
    return poolbusy;
}

void ThreadPool::ClearJobs()
{
    std::queue<std::function<void()>> Empty;
    std::swap(myJobs, Empty);
}

void ThreadPool::ThreadLoop()
{
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(myQueueMutex);
            myMutexCondition.wait(lock, [this] 
            {
                return !myJobs.empty() || myShouldTerminate;
            });
            if (myShouldTerminate) 
            {
                return;
            }
            job = myJobs.front();
            myJobs.pop();
        }
        job();
    }
}
