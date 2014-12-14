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

#ifndef WIN32
#include "core_shell.hpp"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <boost/tokenizer.hpp>

namespace xr { namespace core { namespace execute
	{


	bool proc( std::string _Command, bool _Wait )
		{
		system( _Command.c_str() );
		}


	void open( std::string _Command )
		{
		proc( _Command );
		}

	void copy( std::string _Src, std::string _Dst )
		{
		std::stringstream Cmd;
		Cmd << "copy \"" << _Src << "\" \"" << _Dst << "\"";
		proc( _Command );
		}

	} /* shell */ } /* core */ } /* xr */

#endif // !WIN32