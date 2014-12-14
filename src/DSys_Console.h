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

#ifndef ___CONSOLE_H
#define ___CONSOLE_H

#pragma once
#ifndef ___D_SHARED_H
#include "DShared.H"
#endif


/*
==================
main console class
==================
*/

namespace DSys {

	class Console {
		private:

		/*
		==============
		console buffer
		==============
		*/
			
			enum { 
				CON_MAX_LINES			= 2048,
				CON_MAX_CHARS			= 80,
				CON_MAX_COMMANDS		= 1024
			};


			class ConsoleBuffer	{
				friend class Console;
				private:
					DSTR				buffer[CON_MAX_LINES];
					int					flags[CON_MAX_LINES];
					int					currentline;
				public:
					void clear();
					int findchar( DSTR fmt, char c);
					void add(const char *fmt);
					void addGood(const char *fmt);
					void addWarning(const char *fmt);
					void addError(const char *fmt);
					void extract(DSTR fmt, int line);
					void extract(DSTR fmt, int &flag, int line);
					void getcurrentline(int &line);
					void dump(DSTR filename);
					ConsoleBuffer();
			};
			
			/*
			============
			command line
			============
			*/
			class ConsoleCommandLine{
				friend class Console;
				private:
					DSTR			cmdline;
					int				currentchar;
				public:
					void clear();
					void close();
					void add(char c);
					void addstr( DCSTR fmt);
					void remove();
					void get(DSTR fmt);
					ConsoleCommandLine();
			};


			/*
			=======
			Command
			=======
			*/
			class DCommand  {
				friend class Console;
				private:
					bool	ready;
					int		flags;	// 0 for with param 1 without param
					DSTR	cmdname;
					void	( *cmdfunc)(char *arg);
					void	( *cmdfunc2)( void );
				public:
					DCommand();
					bool Ready();
					void Add(DSTR name, void ( *cmdptr)(char *arg));
					void Add(DSTR name, void ( *cmdptr)( void ));
					int Compare(const char *cmd);
					char *GetName() { return cmdname; }
					void Exec(char *arg);
			};
			
			/*
			===============
			Command History
			===============
			*/
			class DCommandHistory {
				friend class Console;	
				private:
					enum {CON_COMMAND_HISTORY_MAX = 64};
					DSTR	cmd[CON_COMMAND_HISTORY_MAX];
					int		cfree;
					int		scroll;
				public:
					DCommandHistory();
					void add(const char *cmd);
					void get(char *cmd);
					void scrollup();
					void scrolldown();
			};

			static void executefile(DSTR s);
			static void viewtextfile(DSTR s);
			static int findMatches( const char *search , int len = EOF , int match[] = NULL );
		public:
			
			// adds commands to the console
			static void AddCommand(char *name, void ( *cmdptr)( char *) );
			static void AddCommand(char *name, void ( *cmdptr)( void ) );
			static void SendConsoleCommand(char *cmdl);

			static void OnKey(char k);
			static void OnChar(char c);

			// inline functions
			static bool Active()						{return active;}
			// logs text
			static void Log(const char *fmt)			{buffer.add(fmt);}
			// posts a good msg
			static void LogGood(const char *fmt)		{buffer.addGood(va("Good: %s", fmt));}
			// posts warning msg
			static void LogWarning(const char *fmt)		{buffer.addWarning(va("Warning: %s", fmt));}
			// posts error msg
			static void LogError(const char *fmt)		{buffer.addError(va("Error: %s", fmt));}

			
			// console controls
			static void PageUp();
			static void PageDown();
			static void Home();
			static void End();
			static void HistoryUp();
			static void HistoryDown();
			static void CompleteCommand();
	        
			/*
			===========
			Console GFX
			===========
			*/
			struct ConsoleGFX;

			static void GFX_Init();
			static void GFX_Render(float dt);
			static void GFX_Destroy();

			/* ------------------------------------- */ 
			/*  Built-in commands should be friends  */
			/* ------------------------------------- */
				friend void start();
				friend void exit();
				friend void toggle();
				friend void quit();
				friend void cmdlist();
				friend void varlist();
				friend void clear();
				friend void exec(char *param);
				friend void echo(char *param);
				friend void dump(char *param);
				friend void viewtext(char *param);
				friend void set(char *param);
				friend void get(char *param);
				friend void writeconfig(char *param);

		private:
			static DCommand				cmdlist[CON_MAX_COMMANDS];
			static ConsoleGFX			gfx;
			static ConsoleBuffer		buffer;
			static ConsoleCommandLine	cmdline;
			static DCommandHistory		cmdhistory;
			static int					lines_up;

			static bool					active;
			static float				starttime;
			static float				exittime;
			static bool					starting;
			static bool					exiting;

			static int					y;
			static int					emptycmdslot;
	};

}//End namespace DSys
	
#endif // ___CONSOLE_H