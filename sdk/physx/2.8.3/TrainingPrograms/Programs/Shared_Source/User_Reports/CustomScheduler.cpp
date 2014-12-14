#define NOMINMAX

#ifdef WIN32
#include <windows.h>
#endif
#include "NxPhysics.h"
#include "CustomScheduler.h"

CustomScheduler::CustomScheduler()
{
	threadCount   = 0;
	threadHandles = 0;
	quitEvent     = 0;
	pendingEvent  = 0;

}
CustomScheduler::~CustomScheduler()
{
	KillThreads();
}

void CustomScheduler::CreateThreads(const int count)
{
	KillThreads();

	threadCount = count;
	threadHandles = new HANDLE[threadCount];

	quitEvent = CreateEvent(0, TRUE, FALSE, 0);
	assert(0 != quitEvent);
	pendingEvent = CreateEvent(0, TRUE, FALSE, 0);
	assert(0 != pendingEvent);

	workingCount = 0;
	InitializeCriticalSection(&taskQueueCS);

	for(int i = 0; i < threadCount; ++i)
	{
		threadHandles[i] = CreateThread(0, 0, threadFuncStatic, this, 0, 0);
		assert(0 != threadHandles[i]);
	}
}

void CustomScheduler::KillThreads()
{
	if(0 != quitEvent)
		SetEvent(quitEvent);

	if(0 != threadHandles)
	{
		DWORD waitResult = WaitForMultipleObjects(threadCount, threadHandles, TRUE, INFINITE);
		assert(WAIT_FAILED != waitResult);
		for(int i = 0; i < threadCount; ++i)
			CloseHandle(threadHandles[i]);
		delete[] threadHandles;
		threadHandles = 0;
	}
	DeleteCriticalSection(&taskQueueCS);

	if(0 != pendingEvent)
	{
		CloseHandle(pendingEvent);
		pendingEvent = 0;
	}

	if(0 != quitEvent)
	{
		CloseHandle(quitEvent);
		quitEvent = 0;
	}
}

void CustomScheduler::addTask(NxTask *task)
{
	EnterCriticalSection(&taskQueueCS);
	taskQueue.push_back(task);
	SetEvent(pendingEvent);
	LeaveCriticalSection(&taskQueueCS);
}
void CustomScheduler::addBackgroundTask(NxTask *task)
{
	//Background tasks should really go into another task queue so they dont stall processing of time critical tasks.
	addTask(task);
}

void CustomScheduler::waitTasksComplete()
{
	while(executeTask());

	//ensure all threads have finished executing.
	//we spin here, which may not be optimal.
	while(InterlockedCompareExchange(&workingCount,0,0)>0);
}

DWORD __stdcall CustomScheduler::threadFuncStatic(LPVOID userParam)
{
	return ((CustomScheduler *)userParam)->threadFunc();
}

DWORD CustomScheduler::threadFunc()
{
	HANDLE handles[]={quitEvent, pendingEvent};
	while(true)
	{
		if(WAIT_OBJECT_0 == WaitForMultipleObjects(2, handles, FALSE, INFINITE))
			break;//quit event
		else
			executeTask();
	}
	return 0;
}

bool CustomScheduler::executeTask()
{
	NxTask *task = 0;

	//task pending
	EnterCriticalSection(&taskQueueCS);
	if(taskQueue.empty())
		ResetEvent(pendingEvent);
	else
	{
		InterlockedIncrement(&workingCount);
		task = taskQueue.front();
		taskQueue.pop_front();
	}
	LeaveCriticalSection(&taskQueueCS);

	if(0 == task)
		return false;

	task->execute();
	InterlockedDecrement(&workingCount);
	return true;
}


