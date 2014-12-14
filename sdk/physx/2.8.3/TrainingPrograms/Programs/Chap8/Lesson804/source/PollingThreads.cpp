#define NOMINMAX

#ifdef WIN32
#	include <windows.h>
#endif
#include "NxPhysics.h"
#include "PollingThreads.h"

void PollingThreads::CreateThreads(const int count, NxScene *newScene)
{
	KillThreads();

	quitEvent = CreateEvent(0, TRUE, FALSE, 0);
	assert(0 != quitEvent);
	simRunningEvent = CreateEvent(0, TRUE, FALSE, 0);
	assert(0 != simRunningEvent);

	workingCounter = -1;
	InitializeCriticalSection(&shieldCS);
	workingMutexEvent = CreateEvent(0, FALSE, TRUE, 0);

	threadCount = count;
	threadArray = new HANDLE[threadCount];

	for(int i = 0; i < threadCount; ++i)
	{
		threadArray[i] = CreateThread(0, 0, threadFuncStatic, this, 0, 0);
		assert(0 != threadArray[i]);
	}

	scene = newScene;
	shutdownCount = -1;
}

void PollingThreads::ResetPollForWork()
{
	if(0 == threadArray)
		return;

	// wait for all threads to stop working.
	WaitForSingleObject(workingMutexEvent, INFINITE);
	scene->resetPollForWork();

	// release threads to start working again.
	SetEvent(simRunningEvent);
	SetEvent(workingMutexEvent);

}

void PollingThreads::KillThreads()
{
	if(0 != quitEvent)
		SetEvent(quitEvent);

	scene = 0;

	if(0 != threadArray)
	{
		DWORD waitResult = WaitForMultipleObjects(threadCount, threadArray, TRUE, INFINITE);
		assert(WAIT_FAILED != waitResult);
		for(int i = 0; i < threadCount; ++i)
			CloseHandle(threadArray[i]);
		delete[] threadArray;
		threadArray = 0;
	}

	if(0 != simRunningEvent)
	{
		CloseHandle(simRunningEvent);
		simRunningEvent = 0;
	}

	if(0 != workingMutexEvent)
	{
		CloseHandle(workingMutexEvent);
		workingMutexEvent = 0;
		workingCounter = -1;
	}
	DeleteCriticalSection(&shieldCS);

	if(0 != quitEvent)
	{
		CloseHandle(quitEvent);
		quitEvent = 0;
	}
}

DWORD __stdcall PollingThreads::threadFuncStatic(LPVOID userParam)
{
	return ((PollingThreads *)userParam)->threadFunc();
}

void PollingThreads::ShutDownWorkerThreads()
{
	++shutdownCount;
	assert(0 == shutdownCount); //Make sure to call NxScene::shutdownWorkerThreads once before releasing the scene.

	assert(0 != scene);
	scene->shutdownWorkerThreads();
}

DWORD PollingThreads::threadFunc()
{
	while(true)
	{
		HANDLE handles[] = {quitEvent, simRunningEvent};
		if(WAIT_OBJECT_0 == WaitForMultipleObjects(2, handles, FALSE,INFINITE))
			break;//quit event

		// The first thread to enter obtains the working mutex(actually an event).
		EnterCriticalSection(&shieldCS);
		if(0 == InterlockedIncrement(&workingCounter))
			WaitForSingleObject(workingMutexEvent, INFINITE);
		LeaveCriticalSection(&shieldCS);

		NxThreadPollResult pollResult;
		do
		{
			pollResult = scene->pollForWork(NX_WAIT_SIMULATION_END);
		}while((pollResult == NX_THREAD_MOREWORK) || (pollResult == NX_THREAD_NOWORK));

		ResetEvent(simRunningEvent);//block the first time pollForWork() says the sim ended.

		// The last thread to leave releases the working mutex
		EnterCriticalSection(&shieldCS);
		if(InterlockedDecrement(&workingCounter) < 0)
			SetEvent(workingMutexEvent);
		LeaveCriticalSection(&shieldCS);
	}
	return 0;
}
