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
- ...
*/


#include "DSys.h"


/*-------------------------------------------*/
/*			       Libraries                 */
/*-------------------------------------------*/


#pragma comment( lib, "User32.lib")
#pragma comment( lib, "OpenGL32.lib" )
#pragma comment( lib, "GLu32.lib" )
#pragma comment( lib, "DevIL.lib" )
#pragma comment( lib, "ILU.lib" )
#pragma comment( lib, "ILUT.lib" )
#pragma comment( lib, "WinMM.lib" )
#pragma comment( lib, "AdvAPI32.lib")
#pragma comment( lib, "GDI32.lib")
#pragma comment( lib, "DXguid.lib")
//#pragma comment( lib, "OLE32.lib")

// try catch macros
#ifdef _DEBUG
# define Macro_Try()
# define Macro_CatchLoop()
# define Macro_CatchAll()
#else
# define Macro_Try()	try {
# define Macro_CatchLoop()	\
			} catch (Daher::Exception ex) {\
				DSys::Message("An Unhandled Exception in main loop.\n"\
					"- Exception Message:\n %s", ex.What());\
				/*Daher::CallStackTrace::Dump();*/\
			} catch (...) {\
				DSys::Message("An Unhandled Exception in main loop.");\
				/*Daher::CallStackTrace::Dump();*/\
			}
# define Macro_CatchAll() \
			} catch (Daher::Exception ex) {\
				DSys::Message("An Exception Occured in main thread.\n"\
					"- Exception Message:\n %s", ex.What());\
				/*Daher::CallStackTrace::Dump();*/\
			} catch (...) {\
				DSys::Message("An Unhandled Exception Occured main thread.");\
				/*Daher::CallStackTrace::Dump();*/\
			}
#endif

#include "DSys_Console.h"

// farward declerations for the scene
bool Init();
void AddConsoleCommands();
void Update(float msec);
void Render3D();
void Render2D(float msec);
void Shutdown();

/*---------------------
    Main Window Entry
  ---------------------*/

int WINAPI WinMain(HINSTANCE	hInstance,
				   HINSTANCE	hPrevInstance,
				   LPSTR		commandArg,
				   int			nCmdShow)
{
std::string Args = commandArg;
/*/
int main(int argi, char **commandArg)
{
std::string Args = *commandArg;
/*/
	DSys::Logger::Print("WinMain Starts");
	Macro_Try();
	

		DSys::SystemInitialize( Args );
		int CmdShow = DSys::ConfigDialog::Show();
		if( !CmdShow )
		{// aborted
			return -1;
		}

		DSys::SystemStart();

		if(::Init())
		{
			::AddConsoleCommands();

			Macro_Try();

				/*-----------
				  Main Loop
				-----------*/
				DSys::Timer::Init();
				float	timeprev = DSys::Timer::Time();
				float	time, dt;
				while( DSys::SystemThink() )
				{					
					time = DSys::Timer::Time();
					dt = time - timeprev;
					timeprev = time;
					/*
					DWORD SleepTime = (1000 / 15.0f) - dt;
					if( SleepTime > 0 && SleepTime < 1000 )
						Sleep( SleepTime );
					*/
					::Update(dt);
					::Render3D();		// Draw The 3D Scene
					::Render2D(dt);		// Draw The 2D Scene
					
					DSys::Console::GFX_Render(dt);
				}

			Macro_CatchLoop();

			::Shutdown();
		}// ::Init
		
		DSys::SystemShutdown();

	Macro_CatchAll();
	return 0;
}
