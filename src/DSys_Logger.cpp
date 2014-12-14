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

#include "DSys.H"

using namespace DSys;

/*
===========================================

  Logger::Error/Logger::Warning/Logger::Print

===========================================
*/

Daher::Logger		Logger::dlog("user/console.log", false);
int					Logger::level = 0;
Var_bool			Logger::show_debug_info("show_debug_info", "1");
	
void Logger::Print(const char *fmt, ...){
		va_list		argptr;
		DSTR		text;

		va_start (argptr, fmt);
		vsprintf (text, fmt, argptr);
		va_end (argptr);
		EchoToConsole(text);
		dlog.Printf("[%s]\t- ", StringTime());
		for(int i=0; i< level; i++)
			dlog.Printf("%c", '\t');
		dlog.Printf("%s\n", text);

		if( show_debug_info )
			{
			std::cout << text << std::endl;
			std::cout.flush();
			}
	}
	
void Logger::Error(const char *fmt, ...){
		va_list		argptr;
		DSTR		text;

		va_start (argptr, fmt);
		vsprintf (text, fmt, argptr);
		va_end (argptr);

		EchoToConsoleError(text);
		dlog.Printf("[%s]\t! ", StringTime());
		for(int i=0; i< level; i++)
			dlog.Printf("%c", '\t');
		dlog.Printf("%s\n", text);

		if( show_debug_info )
			{
			std::cout << text << std::endl;
			std::cout.flush();
			}
	}

void Logger::Warning(const char *fmt, ...){
		va_list		argptr;
		DSTR		text;

		va_start (argptr, fmt);
		vsprintf (text, fmt, argptr);
		va_end (argptr);
		EchoToConsoleWarning(text);
		dlog.Printf("[%s]\t~ ", StringTime());
		for(int i=0; i< level; i++)
			dlog.Printf("%c", '\t');
		dlog.Printf("%s\n", text);

		if( show_debug_info )
			{
			std::cout << text << std::endl;
			std::cout.flush();
			}
	}

void Logger::Good(const char *fmt, ...){
		va_list		argptr;
		DSTR		text;

		va_start (argptr, fmt);
		vsprintf (text, fmt, argptr);
		va_end (argptr);

		EchoToConsoleGood(text);
		dlog.Printf("[%s]\t+ ", StringTime());
		for(int i=0; i< level; i++)
			dlog.Printf("%c", '\t');
		dlog.Printf("%s\n", text);

		if( show_debug_info )
			{
			std::cout << text << std::endl;
			std::cout.flush();
			}
	}

void Logger::EnterLevel()
	{
		level ++;
	}

void Logger::LeaveLevel()
	{
		if(level >= 0)
			level --;
		else
			level = 0;
	}

Logger::Level::Level()
	{
		Logger::EnterLevel();
	}
Logger::Level::~Level()
	{
		Logger::LeaveLevel();
	}