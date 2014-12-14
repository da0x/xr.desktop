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

#ifndef ___DSYSTEM_H
#define ___DSYSTEM_H

/*
TODO List:
- ...
*/


namespace DSys
{
	static const char *Version = "Cube 0.1";

	void SystemInitialize( std::string commandArg);
	void SystemStart();
	bool SystemThink();
	void SystemShutdown();

	void Message( const char *fmt, ...);	// Displays a message to the user.
	void GetSysInfo();						// echos system info into console and sets them into the cvars sys_*

	// console
	void InitConsole(void);
	void DestroyConsole(void);
	void AddConsoleCommand(char *name, void ( *cmdptr)(char *fmt) );
	void AddConsoleCommand(char *name, void ( *cmdptr)(void) );
	void SendConsoleCommand(char *fmt); // Execute a command as if was writen in the console
	void EchoToConsole(char *fmt);
	void EchoToConsoleError(char *fmt);
	void EchoToConsoleWarning(char *fmt);
	void EchoToConsoleGood(char *fmt);

	void IntroScene();
	void UpdateViewport();

	char *StringDate();
	char *StringTime();
}


#endif // ___DSYSTEM_H