#pragma once
#include <queue>
#include <mutex>
#include "Engine/Core/JobWorker.hpp"


struct JobSystemConfig
{
	int	m_numWorkerThreads = 0;
};

class JobSystem
{
public:
	JobSystem(JobSystemConfig config);
	~JobSystem();
public:
	void Startup();
	void BeginFrame();
	void EndFrame();
	void ShutDown();

	Job* RetrieveJobToExecute();
	Job* RetrieveCompletedJobs();
	void MoveToCompletedJobs(Job* job);
	void MoveToExecutingJobs(Job* job);
	void RemoveFromExecutingJobs(Job* job);
	void QueueJob(Job* jobToQueue);

	JobSystemConfig GetConfig();
	bool IsQuitting() const;

public:
	std::atomic<bool> m_isQuitting = false;
	JobSystemConfig	     m_config;
	std::vector<JobWorker*>m_workerThreads;
	std::deque<Job*>  m_jobsToDoList;
	std::mutex	     m_jobsToDoListMutex;
	std::deque<Job*>  m_completedJobs;
	std::mutex	     m_completedJobsMutex;
	std::vector<Job*> m_retrievedJobs;
	std::mutex	     m_retrievedJobsMutex;
};