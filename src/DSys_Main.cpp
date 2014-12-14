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
 TODO List:
- Execute command line
*/

#include <stdarg.h>
#include "DSys.H"
#include "DSys_ConfigDialog.h"
#include <gl/gl.h>
#include <gl/glu.h>

namespace DSys
{
	void SystemInitialize( std::string commandArg)
	{

		// Todo: execute command line
		DSys::InitConsole();
	}

	void SystemStart()
	{
		// default.cfg MUST exist
//		if(! D_FileExist( "user/default.cfg" ) )
//			throw Daher::Exception("Could not execute 'default.cfg'...");

		BOOL Result;
			// priority class
/*		Result = SetPriorityClass( GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS );
		if( !Result )
			DSys::Logger::Error( "Unable to set priority class! %d", GetLastError() );

			// set thread priority to below normal so that it does not interruprt other processes.
		Result = SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL );
		if( !Result )
			DSys::Logger::Error( "Unable to set thread priority! %d", GetLastError() );
*/

		DSys::SendConsoleCommand("exec user/default.cfg");
		// load console autoexec config if it exists.
		DSys::SendConsoleCommand("exec user/autoexec.cfg");
		// user.cfg stores current user's configurations
		DSys::SendConsoleCommand("exec user/user.cfg");
		// update the varlist
		DSys::Var::ListUpdateAll();

		// create the window
		DSys::Window::Info		windowInfo = {0};
		windowInfo.title		= DSys::sv_sTitle;
		windowInfo.width		= DSys::sv_iScreenWidth;
		windowInfo.height		= DSys::sv_iScreenHeight;
		windowInfo.colorBits	= DSys::sv_iColorBits;
		windowInfo.depthBits	= DSys::sv_iDepthBits;
		windowInfo.desktop		= DSys::sv_bDesktop;
		windowInfo.fullscreen	= DSys::sv_bFullscreen;

		DSys::Window::Create( &windowInfo );

		// setup direct input
		DSys::Input::Init( Window::GetHWND(), ::GetModuleHandle(0));
		// init fmod audio system
		DAudio::Init();
		// init physics
		DSys::Physics::Init();
		// init the timer
		DSys::Timer::Init();
		// view the intro screen
		DSys::IntroScene();
		// init the console graphics
		Console::GFX_Init();
		UpdateViewport();
	}

	bool SystemThink()
	{
		// if the window is not active, then block..
		if (! DSys::Window::Active())
			{
			static float LastMessageTime;

			if( DSys::Timer::Time() > LastMessageTime + 10000 )
				{
				WaitMessage();
				LastMessageTime = DSys::Timer::Time();
				}
			}


		// finish the message queue
		MSG msg;
		while( PeekMessage(&msg,DSys::Window::GetHWND(),0,0,PM_REMOVE))
		{
			if (msg.message != WM_QUIT)
			{
				TranslateMessage(&msg);	// Translate The Message
				DispatchMessage(&msg);	// Dispatch The Message
			}
			else
				return false;
		}		

		// swap the opengl buffers
		Window::SwapGLBuffers();

		// do we have a fixed mouse?
//		if( sv_bFixMousePosition )
//			SetCursorPos(320,240);

		// update the audio system
//		DAudio::Update();

		// update the opengl viewport // every fucking frame?
//		UpdateViewport();

		return true;
	}

	void SystemShutdown()
	{
		Input::Shutdown();
		DAudio::Shutdown();
		Timer::Shutdown();
		Console::GFX_Destroy();
		DSys::Physics::Shutdown();
		DSys::DestroyConsole();
	}

	void UpdateViewport(){
		if (sv_iScreenHeight ==0) //(division fix)
		{// out of range
			DSys::EchoToConsoleWarning("screen height is out of range...");
			sv_iScreenHeight.setval("1");
			sv_iScreenHeight.update();
		}

		glViewport(0, 0, sv_iScreenWidth, sv_iScreenHeight);		

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		// Calculate The Aspect Ratio Of The Window
		gluPerspective( gl_fov, DSys::sv_iScreenWidth/DSys::sv_iScreenHeight, gl_near, gl_far);
		glMatrixMode(GL_MODELVIEW);		// Select The Modelview Matrix
		glLoadIdentity();				// Reset The Modelview Matrix
	}

	/* ------------------------- */
	/*        system tools       */
	/* ------------------------- */


	void Message( const char *fmt, ...)
	{
		va_list ap;
		char text[1024];

		va_start(ap, fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		MessageBox(NULL, text, "DaherEngine", MB_OK|MB_ICONINFORMATION);
	}

	/*
	---------------
	System Time
	---------------
	*/

	char *StringDate()
	{
		static char buffer[1024];
		SYSTEMTIME systime;
		GetSystemTime(&systime);
		sprintf(buffer, "%d:%d:%d",
			systime.wDay,
			systime.wMonth,
			systime.wYear);
		return buffer;
	}

	char *StringTime()
	{
		static char buffer[1024];
		SYSTEMTIME systime;
		GetSystemTime(&systime);
		sprintf(buffer, "%d:%d:%d",
			systime.wHour,
			systime.wMinute,
			systime.wSecond);
		return buffer;
	}
}// namespace DSys