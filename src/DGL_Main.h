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

#ifndef ___DGLU_H
#define ___DGLU_H


// OS and OpenGL
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "gl/glaux.h"
#include "DShared.h"
#include "DSys.h"



/*
===========================
Utilities for DGL namespace
===========================
*/

namespace DGL {
	int GetFPS(float msec);
	void LogPrint(const char *, ...);		// logging
	void LogError(const char *, ...);




	namespace MatrixOp {
		
		/*
		==========================
		Ortho
		==========================
		*/
		class Ortho {
			private:
				static float	orthoMatrix[16];
			public:
				static void Begin();
				static void End();
		};
		/*
		==========================
		BalloonPoint
		==========================
		*/
		class BalloonPoint {
			public:
				static void Begin(Vector position);
				static void End();
		};

	}
}


#endif // ___DGLU_H