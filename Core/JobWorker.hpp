#pragma once
#include <thread>


class JobSystem;

class Job
{
public:
	virtual ~Job() = default;
	virtual void Execute() = 0;
};

class JobWorker
{
public:
	JobWorker(JobSystem* owner, int threadID);
	~JobWorker();

	void ThreadMain(JobWorker* worker);
private:
	JobSystem* m_jobSystem = nullptr;
	int			 m_threadID = -1;
	std::thread* m_thread = nullptr;
};