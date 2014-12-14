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

#ifndef ___DSYS_LOGGER_H
#define ___DSYS_LOGGER_H

namespace DSys {
	//------------- Logging class -------------//

	
	class Logger {
		private:
			static Daher::Logger	dlog;
			static int				level;
			static Var_bool         show_debug_info;
		public:
			static void Print(const char *fmt, ...);
			static void Error(const char *fmt, ...);
			static void Warning(const char *fmt, ...);
			static void Good(const char *fmt, ...);
			static bool UseExtension( const char *name, bool available, Var& cvar);

			static void DSys::Logger::EnterLevel();
			static void DSys::Logger::LeaveLevel();
			
			class Level {
			public:
				Level();
				~Level();
			};

#			define Logger_EnterLevel() DSys::Logger::Level logger_level_instance
	};
}

#endif // ___DSYS_LOGGER_H