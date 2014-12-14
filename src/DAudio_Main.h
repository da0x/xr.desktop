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

#ifndef ___DAUDIO_MAIN_H
#define ___DAUDIO_MAIN_H

//#include <fmod.h>
#include "DSys_Var.h"

namespace DAudio {
	extern DSys::Var_int	sv_iSound_MaxChannels;
	extern DSys::Var_int	sv_iSound_Frequency;
	extern DSys::Var_float	sv_fSound_Volume;

	void Init();
	void Update();
	void Shutdown();
}


#endif // ___DAUDIO_MAIN_H