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

#ifndef ___DSYS_H
#define ___DSYS_H

#include "DShared.h"

#include <windows.h>
#include "DSys_Main.h"
#include "DSys_Var.h"
#include "DSys_Logger.h"
#include "DSys_Window.h"
#include "DSys_Timer.h"
#include "DSys_Input.h"
#include "DSys_Console.h"
#include "DSys_File.h"
#include "DSys_ConfigDialog.h"
#include "DSys_Physics.h"

#include "DAudio_Main.h"
#include "DAudio_Sound.h"
#include "DAudio_Listener.h"

////////////////////////////////////////
// Shared Variables

namespace DSys {

	extern Var_string			sv_sWindowsDir;
	extern Var_string			sv_sSystemDir;
	extern Var_string			sv_sCurrentDir;
	extern Var_string			sv_sTempPath;
	extern Var_string			sv_sComputerName;
	extern Var_string			sv_sUserName;

	extern Var_bool				sv_bFixMousePosition;
	extern Var_bool				sv_bDeveloper;
	extern Var_bool				sv_bTimeDemo;
	extern Var_float			sv_fTimescale;

	extern Var_bool				sv_bDesktop;
	extern Var_bool				sv_bFullscreen;
	extern Var_int				sv_iScreenWidth;
	extern Var_int				sv_iScreenHeight;
	extern Var_int				sv_iColorBits;
	extern Var_int				sv_iDepthBits;
	


	extern Var_string			sv_sTitle;

};


extern DSys::Var_float	gl_fov;
extern DSys::Var_float	gl_near;
extern DSys::Var_float	gl_far;
extern DSys::Var_bool	gl_drawFaces;
extern DSys::Var_bool	gl_avi;

extern DSys::Var_bool	r_drawFPS;
extern DSys::Var_bool	r_reflections;
extern DSys::Var_bool	r_loopDemo;
extern DSys::Var_bool	r_drawTimer;
extern DSys::Var_bool	r_modelBounds;

extern DSys::Var_int	g_speed;


#endif // ___DSYS_H