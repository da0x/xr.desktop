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

#ifndef ___DSYS_TIMER_H
#define ___DSYS_TIMER_H

namespace DSys {

	class Timer{
		private:
			static bool			valid;						// true if the timer is valid
			static __int64		frequency;					// Timer Frequency
			static float		resolution;					// Timer Resolution
			static DWORD		mmTimerStart;				// Multimedia Timer Start Value
			static DWORD		mmTimerElapsed;				// Multimedia Timer Elapsed Time
			static DWORD		mmTimerPause;				// Stores the pause time
			static bool			performanceTimer;			// Using The Performance Timer?
			static __int64		performanceTimerStart;		// Performance Timer Start Value
			static __int64		performanceTimerElapsed;	// Performance Timer Elapsed Time
			static __int64		performanceTimerPause;		// Stores the pause time

			static void			performanceTimerInit();
			static void			mmTimerInit();

			static float		performanceTimerGetTime();
			static float		mmTimerGetTime();
		public:
			static void			Init();						// Initializes the timer
			static float		Time();						// Gets Time In Milliseconds
			static void			Shutdown();				// Terminates the timer
			static void			Pause();					// Sets a pause point
			static void			Unpause();					// Resumes from the paused point
	};
}

#endif // ___DSYS_TIMER_H