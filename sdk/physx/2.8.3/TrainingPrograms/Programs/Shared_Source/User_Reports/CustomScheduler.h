#ifndef __CUSTOM_SCHEDULER__
#define __CUSTOM_SCHEDULER__

#include <deque>

class CustomScheduler : public NxUserScheduler
{
public:
	CustomScheduler();
	~CustomScheduler();

	void CreateThreads(const int count);
	void KillThreads();

	virtual void addTask(NxTask *task);
	virtual void addBackgroundTask(NxTask *task);
	virtual void waitTasksComplete();

private:
	bool executeTask();
	DWORD threadFunc();
	static DWORD __stdcall threadFuncStatic(LPVOID userParam);

private:
	CRITICAL_SECTION taskQueueCS;
	std::deque<NxTask *> taskQueue;

	int threadCount;
	HANDLE *threadHandles;
	HANDLE quitEvent;
	HANDLE pendingEvent;

	volatile long workingCount;
};
#endif
