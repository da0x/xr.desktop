#ifndef __POLLING_THREADS__
#define __POLLING_THREADS__

class PollingThreads
{
public:
	PollingThreads()
	{
		threadCount = 0;
		threadArray = 0;
		quitEvent = 0;
		simRunningEvent = 0;
		workingCounter = -1;
		workingMutexEvent = 0;
		scene = 0;
		shutdownCount = 0;
	}

	~PollingThreads()
	{
		KillThreads();
		assert(0 == shutdownCount); //Make sure to call NxScene::shutdownWorkerThreads once before releasing the scene.
	}

	void CreateThreads(const int count, NxScene *newScene);
	void KillThreads();
	void ResetPollForWork();
	void ShutDownWorkerThreads();

private:
	static DWORD __stdcall threadFuncStatic(LPVOID userParam);
	DWORD threadFunc();

private:
	int threadCount;
	HANDLE *threadArray;

	HANDLE quitEvent;
	HANDLE simRunningEvent;

	LONG workingCounter;
	CRITICAL_SECTION shieldCS;
	HANDLE workingMutexEvent;

	NxScene *scene;
	int shutdownCount;
};

#endif
