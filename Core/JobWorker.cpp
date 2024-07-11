#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/JobWorker.hpp"

JobWorker::JobWorker(JobSystem* owner, int threadID)
	:m_jobSystem(owner)
	, m_threadID(threadID)
{
	m_thread = new std::thread(&JobWorker::ThreadMain, this, this);
}

JobWorker::~JobWorker()
{
	m_thread->join();
	delete m_thread;
	m_thread = nullptr;
}

void JobWorker::ThreadMain(JobWorker* worker)
{
	while (!worker->m_jobSystem->IsQuitting())
	{
		Job* jobToExecute = m_jobSystem->RetrieveJobToExecute();
		if (jobToExecute != nullptr)
		{
			m_jobSystem->MoveToExecutingJobs(jobToExecute);
			jobToExecute->Execute();
			m_jobSystem->RemoveFromExecutingJobs(jobToExecute);
			m_jobSystem->MoveToCompletedJobs(jobToExecute);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
}
