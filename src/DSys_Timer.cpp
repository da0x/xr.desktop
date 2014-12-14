//C++

/*
----------------------------------------------------
The Desktop Project
------------------

Copyright 2004 Daher Alfawares

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

----------------------------------------------------
*/


/*
=======================================================
	Titel:		TIMER CLASS
=======================================================
*/

#include "DSys.H"

bool		DSys::Timer::valid	= false;			// true if the timer is valid
__int64		DSys::Timer::frequency;					// Timer Frequency
float		DSys::Timer::resolution;					// Timer Resolution
DWORD		DSys::Timer::mmTimerStart;				// Multimedia Timer Start Value
DWORD		DSys::Timer::mmTimerElapsed;				// Multimedia Timer Elapsed Time
DWORD		DSys::Timer::mmTimerPause;				// Stores Pause Time
bool		DSys::Timer::performanceTimer;			// Using The Performance Timer?
__int64		DSys::Timer::performanceTimerStart;		// Performance Timer Start Value
__int64		DSys::Timer::performanceTimerElapsed;	// Performance Timer Elapsed Time
__int64		DSys::Timer::performanceTimerPause;		// Stores Pause Time


void
DSys::Timer::performanceTimerInit(){
	QueryPerformanceCounter((LARGE_INTEGER *) &performanceTimerStart);
	performanceTimer		= true;
	// calculate the timer resolution using the timer frequency
	resolution		= (float) ( (double)1.0/(double)frequency );
	// set the elapsed time to the current time
	performanceTimerElapsed	= performanceTimerStart;
	performanceTimerPause = 0;
}

void
DSys::Timer::mmTimerInit(){
	performanceTimer	= false;				// Set Performance Timer To FALSE
	mmTimerStart		= timeGetTime();		// Use timeGetTime() To Get Current Time
	resolution			= 1.0f/1000.0f;			// Set Our Timer Resolution To .001f
	frequency			= 1000;					// Set Our Timer Frequency To 1000
	mmTimerElapsed		= mmTimerStart;			// Set The Elapsed Time To The Current Time
	mmTimerPause		= 0;
}
void
DSys::Timer::Init(){
	DSys::Logger::Print("----- Timer Initializing -----");
	DSys::Logger::Print("Checking for Performance Counter ....");

	// check to see if a performance counter is available
	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &frequency)){
		// no performace counter available
		DSys::Logger::Warning("No Performace Counter Available. Using Multimedia Timer Instead.");
		mmTimerInit();
	} else {
		// performance counter is available, use it instead of the multimedia timer
		DSys::Logger::Print("Performance Counter Is Available with frequency of %d Hz.",frequency);
		performanceTimerInit();
	}
	valid = true;
	DSys::Logger::Print("------------------------------");
}

void
DSys::Timer::Shutdown(){
	valid = false;
}

float 
DSys::Timer::performanceTimerGetTime(){
	__int64 time;
	QueryPerformanceCounter((LARGE_INTEGER *) &time);
	// return the current time minus the start time multiplied by the resolution and 1000 (to get ms)
	performanceTimerElapsed= ( (float) ( time - performanceTimerStart) * resolution)*1000.0f;
	return performanceTimerElapsed;
}
float DSys::Timer::mmTimerGetTime(){
	// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
	mmTimerElapsed= ( (float) ( timeGetTime() - mmTimerStart) * resolution)*1000.0f;
	return mmTimerElapsed;		
}

float DSys::Timer::Time() {
	if(!valid) return -1;

	if (performanceTimer)
		return performanceTimerGetTime();
	else
		return mmTimerGetTime();
}

void DSys::Timer::Pause(){
	if (performanceTimer){
		QueryPerformanceCounter((LARGE_INTEGER *) &performanceTimerPause);
	} else {
		mmTimerPause = timeGetTime();
	}
}

void DSys::Timer::Unpause(){
	if (performanceTimer){
		__int64 time;
		QueryPerformanceCounter((LARGE_INTEGER *) &time);
		performanceTimerStart += time - performanceTimerPause;
		performanceTimerPause = 0;
	} else {
		DWORD time;
		time = timeGetTime();
		mmTimerStart += time - mmTimerPause;
		mmTimerPause = 0;
	}
}