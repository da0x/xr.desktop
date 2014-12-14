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

#include "core_utf8.hpp"

#include <windows.h>

#include <memory>
#include <string>

namespace xr { namespace core { namespace utf8
	{

	std::string encode( std::wstring _Data )
		{
		int Size = WideCharToMultiByte( CP_UTF8, 0, _Data.c_str(), static_cast<int>(_Data.length()), 0, 0, 0, 0 );
		std::auto_ptr<char> Buffer( new char[Size+1] );

		Size = WideCharToMultiByte( CP_UTF8, 0, _Data.c_str(), static_cast<int>(_Data.length()), Buffer.get(), Size, 0, 0 );
		Buffer.get()[Size] = '\0';
		std::string Out = Buffer.get();
		return Out;
		}

	std::wstring decode( std::string _Data )
		{
		int Size = MultiByteToWideChar( CP_UTF8, 0, _Data.c_str(), static_cast<int>(_Data.length()), 0, 0 );
		std::auto_ptr<wchar_t> Buffer( new wchar_t[Size+1] );

		Size = MultiByteToWideChar( CP_UTF8, 0, _Data.c_str(), static_cast<int>(_Data.length()), Buffer.get(), Size );
		Buffer.get()[Size] = L'\0';
		std::wstring Out( Buffer.get() );
		return Out;
		}


	} /* utf8 */ } /* core */ } /* xr */

