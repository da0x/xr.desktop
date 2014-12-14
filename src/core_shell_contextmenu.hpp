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

#ifndef __XR_CORE_CONTEXTMENU_HPP
#define __XR_CORE_CONTEXTMENU_HPP

#include <vector>
#include <string>

namespace xr { namespace core { namespace shell
	{

	void contextmenu( std::string _Item, void* _ParentWindowHandle, std::pair<int,int> _Point );
	void contextmenu( std::vector<std::string> _ItemList, void* _ParentWindowHandle, std::pair<int,int> _Point );

	} /* shell */ } /* core */ } /* xr */


#endif // __XR_CORE_CONTEXTMENU_HPP

