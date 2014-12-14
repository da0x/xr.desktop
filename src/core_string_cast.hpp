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

#ifndef __XR_CORE_STRING_CAST_HPP
#define __XR_CORE_STRING_CAST_HPP


#include "core_utf8.hpp"
#include <string>

namespace xr { namespace core {

	template<typename _DstType, typename _SrcType>
		_DstType inline string_cast( const _SrcType& _Src )
		{ // ignore
		return _Src;
		}

	template<> inline std::wstring string_cast<std::wstring,std::string>( const std::string& _Src )
		{ // multibyte to wide 
		return core::utf8::decode( _Src );
		}

	template<> inline std::string string_cast<std::string,std::wstring>( const std::wstring& _Src )
		{ // wide to multibyte
		return core::utf8::encode( _Src );
		}

	} /* core */ } /* xr */

#endif // __XR_CORE_STRING_CAST_HPP
