#pragma once
#include <functional>
#include <mutex>
#include <thread>
#include <queue>

class ThreadPool 
{
public:
	~ThreadPool();
	void Start();
	void QueueJob(const std::function<void()>& job);
	bool Busy();
	std::queue<std::function<void()>> GetJobsList() { return myJobs; }
	void ClearJobs();
private:
	void ThreadLoop();

	bool myShouldTerminate = false;
	std::mutex myQueueMutex;
	std::condition_variable myMutexCondition;
	std::vector<std::thread> myThreads;
	std::queue<std::function<void()>> myJobs;
};