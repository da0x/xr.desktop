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

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include <stdarg.h>
#include "DSys.H"

#include "SysInfo/SysInfo.H"
#include <iostream>
#include <sstream>


// shared variables
namespace DSys {
	Var_string		sv_sWindowsDir			("sys_windowsDir",		"null",	Var::NoSet);
	Var_string		sv_sSystemDir			("sys_systemDir",		"null",	Var::NoSet);
	Var_string		sv_sCurrentDir			("sys_currentDir",		"null",	Var::NoSet);
	Var_string		sv_sTempPath	 		("sys_tempPath",		"null",	Var::NoSet);
	Var_string		sv_sComputerName		("sys_computerName",	"null",	Var::NoSet);
	Var_string		sv_sUserName			("sys_userName",		"null",	Var::NoSet);

	Var_bool		sv_bDeveloper			("sys_developer",		"1",	Var::None);
	Var_bool		sv_bTimeDemo			("sys_timeDemo",		"0",	Var::None);
	Var_float		sv_fTimescale			("sys_timescale",		"1",	Var::None);

	Var_bool		sv_bFullscreen			("sys_fullscreen",		"1");
	Var_bool		sv_bDesktop				("sys_desktop",			"0");
	Var_int			sv_iScreenWidth			("sys_screenWidth",		"1920");
	Var_int			sv_iScreenHeight		("sys_screenHeight",	"1200");
	Var_int			sv_iColorBits			("sys_colorBits",		"32");
	Var_int			sv_iDepthBits			("sys_depthBits",		"24");
	Var_bool		sv_bFixMousePosition	("sys_fixMousePosition","0");

	Var_string		sv_sTitle				("sys_title",			"Cube", Var::NoSet);
};

/*-------------------------------------------*/
/*                Variables                  */
/*-------------------------------------------*/

bool			DSys::Window::active;
bool			DSys::Window::isDesktop;
bool			DSys::Window::isFullscreen;

HDC				DSys::Window::hDC			= NULL;
HGLRC			DSys::Window::hRC			= NULL;
HWND			DSys::Window::hWnd			= NULL;
HINSTANCE		DSys::Window::hInstance		= NULL;


int				DSys::Window::width			= 0;
int				DSys::Window::height		= 0;

/*-------------------------------------------*/
/*        Engine's Message Handler           */
/*-------------------------------------------*/





LRESULT CALLBACK
DSys::Window::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DMacro_TraceEnter(DWndProc);
	switch (uMsg){
		case WM_ACTIVATE:			if (!HIWORD(wParam)){
										active=TRUE;
										if( sv_bFixMousePosition )
											ShowCursor(FALSE);
									} else {
										active=FALSE;
										if( sv_bFixMousePosition )
											ShowCursor(TRUE);
									}
									break;	
		case WM_KILLFOCUS:			active = FALSE;
//									ShowCursor(TRUE);
									break;
		case WM_SETFOCUS:			active = TRUE;
//									ShowCursor(FALSE);
									break;

		case WM_LBUTTONDBLCLK:
									DSys::Input::DoubleClick(true);
									break;
		case WM_SYSCOMMAND:			switch (wParam){
										case SC_SCREENSAVE:
										case SC_MONITORPOWER:
											DMacro_TraceLeave()
											return 0;
									}
									break;
		case WM_CLOSE:				PostQuitMessage(0);
									break;
		case WM_KEYDOWN:			Console::OnKey((char)wParam);return 0;
		case WM_KEYUP:				return 0;
		case WM_CHAR:				Console::OnChar((char)wParam);return 0;
		case WM_SIZE:				if( sv_bDesktop )
										return 0;
									else
										Window::ChangeSize(LOWORD(lParam),HIWORD(lParam)); break;
	}
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}



void DSys::GetSysInfo(){
	DMacro_TraceEnter(DSys::GetSysInfo);
	DSystemInfo::SysInfo sysInfo;
	int intval[2];
	char tempstr[MAX_PATH];
	unsigned long strsize = MAX_PATH;


	DSys::Logger::Print (" ");
	DSys::Logger::Print ("----- GETTING DSys INFORMATION -----");;
	Logger_EnterLevel();

	DSys::Logger::Print ("--------------");
	DSys::Logger::Print ("OS INFORMATION");
	DSys::Logger::Print ("--------------");
	DSys::Logger::Print (va("Operating System:      \"%s\"", sysInfo.getOSDescription().c_str()));

	GetWindowsDirectory(tempstr, MAX_PATH);
	sv_sWindowsDir.setval(tempstr);
	DSys::Logger::Print ("Windows Directory:     \"%s\"", tempstr);

	GetSystemDirectory(tempstr,MAX_PATH);
	sv_sSystemDir.setval(tempstr);
	DSys::Logger::Print ("System Directory:      \"%s\"", tempstr);

	GetTempPath(MAX_PATH, tempstr);
	sv_sTempPath.setval(tempstr);
	DSys::Logger::Print ("Temp Path:        \"%s\"", tempstr);

	GetCurrentDirectory(MAX_PATH, tempstr);
	sv_sCurrentDir.setval(tempstr);
	DSys::Logger::Print ("Current Directory:     \"%s\"", tempstr);

	GetComputerName(tempstr, &strsize);
	sv_sComputerName.setval(tempstr);
	DSys::Logger::Print ("Computer Name:         \"%s\"", tempstr);

		GetUserName(tempstr, &strsize);
		sv_sUserName.setval(tempstr);
	DSys::Logger::Print ("User Name:             \"%s\"", tempstr);

	DSys::Logger::Print ("---------------------");
	DSys::Logger::Print ("PROCESSOR INFORMATION");
	DSys::Logger::Print ("---------------------");

	DSys::Logger::Print (va("CPU String:         \"%s\"", sysInfo.getCpuIdentification().c_str()));
	DSys::Logger::Print (va("CPU Speed:          \"%s\"", sysInfo.getCpuSpeed().c_str()));
	DSys::Logger::Print (va("Number of CPUs:     \"%s\"", sysInfo.getNumProcessors().c_str()));
	DSys::Logger::Print (va("Family:             \"%i\"", sysInfo.getCpuFamily()));
	DSys::Logger::Print (va("Model:              \"%i\"", sysInfo.getCpuModel()));
	DSys::Logger::Print (va("Stepping:           \"%i\"", sysInfo.getCpuStepping()));
#if 0
	DSys::Logger::Print ("------------------");
	DSys::Logger::Print ("PROCESSOR FEATURES");
	DSys::Logger::Print ("------------------");
	DSys::Logger::Print (va("FPU:     %i", sysInfo.getFeature(FPU_FLAG)));
	DSys::Logger::Print (va("CMOV:    %i", sysInfo.getFeature(CMOV_FLAG)));
	DSys::Logger::Print (va("VME:     %i", sysInfo.getFeature(VME_FLAG))); 
	DSys::Logger::Print (va("PAT:     %i", sysInfo.getFeature(PAT_FLAG)));
	DSys::Logger::Print (va("DE:      %i", sysInfo.getFeature(DE_FLAG)));
	DSys::Logger::Print (va("PSE36:   %i", sysInfo.getFeature(PSE36_FLAG)));
	DSys::Logger::Print (va("PSE:     %i", sysInfo.getFeature(PSE_FLAG)));
	DSys::Logger::Print (va("PSNUM:   %i", sysInfo.getFeature(PSNUM_FLAG)));
	DSys::Logger::Print (va("TSC:     %i", sysInfo.getFeature(TSC_FLAG)));  
	DSys::Logger::Print (va("CLFLUSH: %i", sysInfo.getFeature(CLFLUSH_FLAG)));
	DSys::Logger::Print (va("MSR:     %i", sysInfo.getFeature(MSR_FLAG)));
	DSys::Logger::Print (va("DTS:     %i", sysInfo.getFeature(DTS_FLAG)));
	DSys::Logger::Print (va("PAE:     %i", sysInfo.getFeature(PAE_FLAG))); 
	DSys::Logger::Print (va("ACPI:    %i", sysInfo.getFeature(ACPI_FLAG)));
	DSys::Logger::Print (va("MCE:     %i", sysInfo.getFeature(MCE_FLAG)));
	DSys::Logger::Print (va("MMX:     %i", sysInfo.getFeature(MMX_FLAG)));
	DSys::Logger::Print (va("CX8:     %i", sysInfo.getFeature(CX8_FLAG)));
	DSys::Logger::Print (va("FXSR:    %i", sysInfo.getFeature(FXSR_FLAG)));
	DSys::Logger::Print (va("APIC:    %i", sysInfo.getFeature(APIC_FLAG)));
	DSys::Logger::Print (va("SSE:     %i", sysInfo.getFeature(SSE_FLAG)));
	DSys::Logger::Print (va("SEP:     %i", sysInfo.getFeature(SEP_FLAG))); 
	DSys::Logger::Print (va("SSE2:    %i", sysInfo.getFeature(SSE2_FLAG)));
	DSys::Logger::Print (va("MTRR:    %i", sysInfo.getFeature(MTRR_FLAG)));
	DSys::Logger::Print (va("SS:      %i", sysInfo.getFeature(SS_FLAG)));
	DSys::Logger::Print (va("PGE:     %i", sysInfo.getFeature(PGE_FLAG))); 
	DSys::Logger::Print (va("TM:      %i", sysInfo.getFeature(TM_FLAG)));
	DSys::Logger::Print (va("MCA:     %i", sysInfo.getFeature(MCA_FLAG)));
	DSys::Logger::Print (va("3dNow!:  %i", sysInfo.getExtFeature(HAS3DNOW_FLAG)));
	DSys::Logger::Print (va("3dNow!Ex:%i", sysInfo.getExtFeature(EXT3DNOW_FLAG)));
	DSys::Logger::Print (va("SSE MMX: %i", sysInfo.getExtFeature(SSEMMX_FLAG)));
#endif
	DSys::Logger::Print ("------------------");
	DSys::Logger::Print ("MEMORY INFORMATION");
	DSys::Logger::Print ("------------------");
	DSys::Logger::Print (va("Total:              \"%s\"", sysInfo.getTotalRam().c_str()));
	DSys::Logger::Print (va("Available:          \"%s\"", sysInfo.getAvailRam().c_str()));
	DSys::Logger::Print (va("Total Page:         \"%s\"", sysInfo.getTotalPageFile().c_str()));
	DSys::Logger::Print (va("Avail Page:         \"%s\"", sysInfo.getAvailPageFile().c_str()));
	DSys::Logger::Print (va("Total Virtual:      \"%s\"", sysInfo.getTotalVirtual().c_str()));
	DSys::Logger::Print (va("Avail Virtual:      \"%s\"", sysInfo.getAvailVirtual().c_str()));

	DSys::Logger::Print ("------------------");
	DSys::Logger::Print ("SOCKET INFORMATION");
	DSys::Logger::Print ("------------------");
	DSys::Logger::Print (va("Version:            \"%s\"", sysInfo.getSocketVersion().c_str()));
	DSys::Logger::Print (va("Highest Ver:        \"%s\"", sysInfo.getHighestSocketVersion().c_str()));
	DSys::Logger::Print (va("Description:        \"%s\"", sysInfo.getSocketDescription().c_str()));
	DSys::Logger::Print (va("System Status:      \"%s\"", sysInfo.getSocketSystemStatus().c_str()));
	DSys::Logger::Print (va("Max:                \"%s\"", sysInfo.getSocketMax().c_str()));
	DSys::Logger::Print (va("IP Address:         \"%s\"", sysInfo.getIPAddress().c_str()));
	DSys::Logger::Print (va("Domain Name:        \"%s\"", sysInfo.getDomainName().c_str()));
	DSys::Logger::Print (va("UDP Max:            \"%s\"", sysInfo.getSocketUdpMax().c_str())); 
	DSys::Logger::Print (" ");

	DSys::Logger::Print ("----------------------");
	DSys::Logger::Print ("HARD DRIVE INFORMATION");
	DSys::Logger::Print ("----------------------");
	std::vector<DSystemInfo::DriveStats> const* pvDriveStats = sysInfo.getDriveStats();
	std::vector<DSystemInfo::DriveStats>::const_iterator i;
	for (i = pvDriveStats->begin(); i != pvDriveStats->end(); i++)
	{
		DSys::Logger::Print (va("Name: \"%s\"    Type: \"%s\"    Total: \"%s\"    Free: \"%s\"",
			i->getName().c_str(),
			i->getType().c_str(),
			i->getTotalSpace().c_str(),
			i->getFreeSpace().c_str()));
	}

	DSys::Logger::Print ("------------------");
    DSys::Logger::Print ("OPENGL INFORMATION");
    DSys::Logger::Print ("------------------");
	DSys::Logger::Print(va("GL Renderer:       \"%s\"", (char *)glGetString(GL_RENDERER)));
	DSys::Logger::Print(va("GL Vendor:         \"%s\"", (char *)glGetString(GL_VENDOR)));
	DSys::Logger::Print(va("GL Version:        \"%s\"", (char *)glGetString(GL_VERSION)));
	DSys::Logger::Print(va("GLU Version:       \"%s\"", (char *)gluGetString (GLU_VERSION)));

	char *text = new char[ strlen( (char *)glGetString(GL_EXTENSIONS) ) +1 ];
	char *text2 =  new char[ strlen( (char *)gluGetString(GLU_EXTENSIONS) ) +1 ];

	strcpy( text, (char *)glGetString(GL_EXTENSIONS));
	strcpy( text2, (char *)gluGetString(GLU_EXTENSIONS));
	
	DSys::Logger::Print ("-----------------");
    DSys::Logger::Print ("OPENGL EXTENTIONS");
    DSys::Logger::Print ("-----------------");

	char *token;
	token=strtok(text," ");
	DSys::Logger::Print(" - GL EXTENTIONS -");
	while(token!= NULL)
	{
		DSys::Logger::Print(token);
		token=strtok(NULL," ");
	}
	DSys::Logger::Print(" - GLU EXTENTIONS -");

	token=strtok(text2," ");
	while(token!= NULL)
	{
		DSys::Logger::Print(token);
		token=strtok(NULL," ");
	}
	delete [] text;
	delete [] text2;
	
	DSys::Logger::Print ("------------------");
	DSys::Logger::Print ("Other OpengGL Info");
	DSys::Logger::Print ("------------------");
	
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, intval);
	DSys::Logger::Print("Max texture size: %i",*intval);
//	gl_maxTextureSize.setval(va("%i",*intval));

	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, intval);
	DSys::Logger::Print("ARB Max texture units: %i",*intval);
//	gl_maxTextureUnitsARB.setval(va("%i",*intval));

	glGetIntegerv(GL_MAX_LIGHTS, intval);
	DSys::Logger::Print("Max lights: %i",*intval);

	glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, intval);
	DSys::Logger::Print("Max modelview stack depth: %i",*intval);

	glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, intval);
	DSys::Logger::Print("Max projection stack depth: %i",*intval);

	glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, intval);
	DSys::Logger::Print("Max texture stack depth: %i",*intval);

	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, intval);
	DSys::Logger::Print("Max viewport dims: %i x %i",intval[0], intval[1]);

	glGetIntegerv(GL_MAX_LIST_NESTING, intval);
	DSys::Logger::Print("Max list nesting: %i",*intval);

	DSys::Logger::Print("------------------------------------");
	DSys::Logger::Print(" ");
	DMacro_TraceLeave();
}




void DSys::Window::Destroy(void){
	DMacro_TraceEnter(DSys::Window::Destroy);
	DSys::Logger::Print("---------------------------");
	DSys::Logger::Print("----- Window Shutdown -----");
	
	if (isFullscreen){
		ChangeDisplaySettings(NULL,0);
	}

//	ShowCursor(TRUE);
	ReleaseCapture();

	if (hRC){		// Do We Have A Rendering Context?
		if (!wglMakeCurrent(NULL,NULL))						// Are We Able To Release The DC And RC Contexts?
		{
			DSys::Message("SHUTDOWN ERROR\nRelease Of DC And RC Failed.");
		} else {
			DSys::Logger::Print("Making Current DC and RC =(null) Succeeded ...");
		}
		if (!wglDeleteContext(hRC))							// Are We Able To Delete The RC?
		{
			DSys::Message("SHUTDOWN ERROR\nRelease Rendering Context Failed.");
		} else {
			DSys::Logger::Print("Deleting RC Succeeded ...");
		}
		hRC= NULL;											// Set RC To NULL
	}
	if (hDC && !ReleaseDC(hWnd,hDC))						// Are We Able To Release The DC
	{
		DSys::Message("SHUTDOWN ERROR\nRelease Device Context Failed.");
		hDC= NULL;											// Set DC To NULL
	} else {
		DSys::Logger::Print("Relese DC from window handle Succeeded ...");
	}
	if (hWnd && !DestroyWindow(hWnd))						// Are We Able To Destroy The Window?
	{
		DSys::Message("SHUTDOWN ERROR\nCould Not Release hWnd.");
		hWnd= NULL;											// Set hWnd To NULL
	} else {
		DSys::Logger::Print("Window Handle Destroyed Successfully ...");
	}
	if (!UnregisterClass("XREngine",hInstance))				// Are We Able To Unregister Class
	{
		DSys::Message("SHUTDOWN ERROR\nCould Not Unregister Class.");
		hInstance= NULL;										// Set hInstance To NULL
	} else {
		DSys::Logger::Print("Window Class Unregistered Successfully ...");
	}
	DMacro_TraceLeave();
}

extern "C" PFNWGLGETEXTENSIONSSTRINGARBPROC    	wglGetExtensionsStringARB = NULL;
extern "C" PFNWGLCREATECONTEXTATTRIBSARBPROC	wglCreateContextAttribsARB = NULL;


bool DSys::Window::Create( DSys::Window::Info* info ){
	DMacro_TraceEnter(DSys::Window::Create);
	DSys::Logger::Print("------------------------------");
	DSys::Logger::Print("----- Creating GL Window -----");
	Logger_EnterLevel();
	
	isFullscreen = sv_bFullscreen;
	isDesktop = info->desktop;

	if( isDesktop )
		{
		isFullscreen = info->fullscreen = false;
		width = info->width = GetSystemMetrics(SM_CXSCREEN);
		height = info->height = GetSystemMetrics(SM_CYSCREEN);

		std::stringstream Cmd;
		Cmd << static_cast<int>(info->fullscreen);
		sv_bFullscreen.setval( Cmd.str().c_str() );
		Cmd.str("");
		Cmd << info->width;
		sv_iScreenWidth.setval( Cmd.str().c_str() );
		Cmd.str("");
		Cmd << info->height;
		sv_iScreenHeight.setval( Cmd.str().c_str() );

		sv_bFullscreen.update();
		sv_iScreenWidth.update();
		sv_iScreenHeight.update();
		}
	
	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	HWND 		parent = 0;

	DSys::Logger::Print("Window Dim = %dx%d", info->width, info->height);
	DSys::Logger::Print("Fullscreen = %s", (info->fullscreen ? "True":"False"));

	hInstance		= GetModuleHandle(NULL);
	wc.style		= /*CS_HREDRAW | CS_VREDRAW | */CS_DBLCLKS | CS_OWNDC;	// Redraw On Move, And Own DC For Window
	wc.lpfnWndProc	= (WNDPROC) WndProc;					// DWndProc Handles Messages
	wc.cbClsExtra	= 0;									// No Extra Window Data
	wc.cbWndExtra	= 0;									// No Extra Window Data
	wc.hInstance	= hInstance;							// Set The Instance
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground= NULL;									// No Background Required For GL
	wc.lpszMenuName	= NULL;									// We Don't Want A Menu
	wc.lpszClassName= "XREngine";						    // Set The Class Name
	if (!RegisterClass(&wc))								// Attempt To Register The Window Class
	{
		DSys::Message("Failed To Register The Window Class ...");
		DMacro_TraceLeave();
		return false;										// Exit And Return FALSE
	}


	RECT WindowRect;

	WindowRect.left=(long)0;
	WindowRect.right=(long)info->width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)info->height;

	if(isDesktop){

		dwStyle= WS_POPUP;
		dwExStyle=0;

		parent = GetDesktopWindow(); // now get the desktop handle
	} else if (isFullscreen){
		DEVMODE devMode = {0};
		devMode.dmSize			= sizeof(devMode);
		devMode.dmPelsWidth		= info->width;
		devMode.dmPelsHeight	= info->height;
		devMode.dmBitsPerPel	= info->colorBits;
		devMode.dmDisplayFrequency = 75;
		devMode.dmFields		= DM_PELSWIDTH |
									DM_PELSHEIGHT |
									DM_BITSPERPEL |
									DM_DISPLAYFREQUENCY;
		

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&devMode,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "DaherEngine" ,MB_YESNO|MB_ICONEXCLAMATION)==IDYES){
				isFullscreen = false;
			} else {
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				DMacro_TraceLeave();
				return false;								// Exit And Return FALSE
			}
		}
	
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
	} else {
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;						// Windows Style
	}
	
		// Adjust Window To True Requested Size
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);
	if (!(hWnd=CreateWindowEx(	dwExStyle,					// Extended Style For The Window
									"XREngine",				// Class Name
									info->title,				// Window Title
//									WS_CLIPSIBLINGS |			// Required Window Style
//									WS_CLIPCHILDREN |			// Required Window Style
									dwStyle,					// Selected Window Style
									0, 0,						// Window Position
									WindowRect.right-WindowRect.left,	// Calculate Adjusted Window Width
									WindowRect.bottom-WindowRect.top,	// Calculate Adjusted Window Height
									parent,						// Parent Window
									NULL,						// No Menu
									hInstance,					// Instance
									NULL)))						// Don't Pass Anything To WM_CREATE
		{
			DSys::Message("Window Creation Error.");
			DMacro_TraceLeave();
			return false;										// Return FALSE
		}

	
	if (!(hDC= ::GetDC(hWnd)))
	{
		DSys::Message("Can't Create A Device Context.");
		DMacro_TraceLeave();
		return false;
	}

	static	PIXELFORMATDESCRIPTOR pfd=						// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),						// Size Of This Pixel Format Descriptor
			1,													// Version Number
			PFD_DRAW_TO_WINDOW |								// Format Must Support Window
			PFD_SUPPORT_OPENGL |								// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,									// Must Support Double Buffering
			PFD_TYPE_RGBA,										// Request An RGBA Format
			info->colorBits,									// Color Depth
			0, 0, 0, 0, 0, 0,									// Color Bits Ignored
			0,//info->alphaBits,								// No Alpha Buffer
			0,													// Shift Bit Ignored
			0,//info->accumBits,								// No Accumulation Buffer
			0, 0, 0, 0,											// Accumulation Bits Ignored
			info->depthBits,									// Z-Buffer (Depth Buffer)
			0,//info->stencilBits,								// Stencil Buffer
			0,//info->auxBuffers,								// Auxiliary Buffer
			PFD_MAIN_PLANE,										// Main Drawing Layer
			0,													// Reserved
			0, 0, 0												// Layer Masks Ignored
	};
	
	GLuint PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(hDC,&pfd)))	
	{
		DSys::Message("Can't Find A Suitable PixelFormat.");
		DMacro_TraceLeave();
		return false;
	}

	DSys::Logger::Print("Choosed The Following Pixel Format:");
	DSys::Logger::Print("Color Bits    = %d", info->colorBits);
	DSys::Logger::Print("Alpha Bits    = %d", info->alphaBits);
	DSys::Logger::Print("Depth Bits    = %d", info->depthBits);
	DSys::Logger::Print("Stencil Bits  = %d", info->stencilBits);
	DSys::Logger::Print("Accum Bits    = %d", info->accumBits);

	// Checking hardware support before setting the pixel format
	DescribePixelFormat(hDC,PixelFormat,sizeof(tagPIXELFORMATDESCRIPTOR),&pfd);
	if (pfd.dwFlags & PFD_GENERIC_FORMAT)
	{
		Message("Sorry: OpenGL hardware acceleration not found.\nTo quit hold the ESC key while loading.");
		DSys::Logger::Warning("Hardware accel = False");
	}
	else
	{
		DSys::Logger::Good("Hardware accel = True");
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))
	{
		DSys::Message("Can't Set The PixelFormat.");
//		DMacro_TraceLeave();
//		return false;
	}

	if (!(hRC=wglCreateContext(hDC)))
	{
		DSys::Message("Can't Create A GL Rendering Context.");
		DMacro_TraceLeave();
		return false;
	}

	if(!wglMakeCurrent(hDC,hRC))
	{
		DSys::Message("Can't Activate The GL Rendering Context.");
		DMacro_TraceLeave();
		return false;
	}
	/*************************/
	/*
	 wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC) wglGetProcAddress("wglGetExtensionsStringARB");
	 if (wglGetExtensionsStringARB == NULL)
		 return 0;

	const char *extensions = wglGetExtensionsStringARB(hDC);
	if (strstr(extensions, "WGL_ARB_create_context" ) == NULL)
		{
		DSys::Logger::Warning("WGL_ARB_create_context is not supported.");
			// does not support GL3
		}
	else
		{
		wglCreateContextAttribsARB =(PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");
		if (wglCreateContextAttribsARB == NULL)
			{
			DSys::Logger::Error("wglCreateContextAttribsARB is NOT supported.");
			}
		else
			{

			HGLRC hRC_old = hRC;

				
			//Now setup attributes to specify the version of OpenGL to be created.
			int attribs[5] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 0,
				NULL
				};
			hRC = wglCreateContextAttribsARB(hDC, NULL, attribs);
			//Check that a valid context was created. If context creation failed, check the error.
			if(hRC == NULL)
				{
				// Context creation failed, find out why
				DWORD error = GetLastError() ;
				if (error == ERROR_INVALID_VERSION_ARB)
					DSys::Logger::Error("OpenGL 3.0 not supported.");
				else
					DSys::Logger::Error("Unknown error creating an OpenGL 3.0 Context." );

					// wind back to older version.
				hRC = hRC_old;
				}
			else
				{

					//If a valid context handle was returned, make it current. Also check the version of the new context.
				wglMakeCurrent( hDC, hRC );

					//The old context is no longer needed and can be deleted.
				wglMakeCurrent(NULL, NULL);
				wglDeleteContext (hRC_old);
				}
			}
		}

/****************************/

//	ShowCursor(false);
	ShowWindow(hWnd,SW_SHOW);								// Show The Window
	if(!isDesktop){
		SetForegroundWindow(hWnd);							// Slightly Higher Priority
		SetActiveWindow(hWnd);
	}
	
	if(isDesktop){
		/**/
		 HWND hprogman = FindWindow("Progman","Program Manager"); // now set it as a child to the window manager of XP
  		 if (hprogman == NULL){
 	       hprogman = FindWindow("#32769",""); // this is for Vista compatibility
	    }
  		SetParent(hWnd, hprogman); // set it
		SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE) | WS_CHILD));
		SendMessage( hprogman, WM_UPDATEUISTATE,0,0);
		/**/
	//	SetParent(hWnd,parent);
		SetWindowPos(hWnd,HWND_BOTTOM,0,0,info->width,info->height,SWP_NOMOVE|SWP_NOSENDCHANGING);
		LockSetForegroundWindow(LSFW_LOCK);/**/
	}

	SetFocus(hWnd);											// Sets Keyboard Focus To The Window
	SetCapture(hWnd);
	DSys::GetSysInfo();

	DMacro_TraceLeave();
	return true;
}



bool DSys::Window::ChangeRes(int width, int height, int bits){
	if(!isFullscreen) return false;
	DMacro_TraceEnter(DSys::Window::ChangeRes);
	DSys::Logger::Print("Changing screen resolution to %ix%ix%i",width,height,bits);

	ChangeDisplaySettings(NULL, 0);

	DEVMODE devMode = {0};
	devMode.dmSize			= sizeof(devMode);
	devMode.dmPelsWidth		= width;
	devMode.dmPelsHeight	= height;
	devMode.dmBitsPerPel	= bits;
	devMode.dmFields		= DM_BITSPERPEL|
								DM_PELSWIDTH|
								DM_PELSHEIGHT;
	devMode.dmDisplayFrequency = 85;

	// Try To Set Selected Mode And Get Results.
	long result = ChangeDisplaySettings(&devMode,CDS_FULLSCREEN);
	switch(result){
		case DISP_CHANGE_SUCCESSFUL:
			DMacro_TraceLeave();
			return MoveWindow(hWnd, 0,0, width, height, FALSE);
		case DISP_CHANGE_BADMODE:
			DSys::Logger::Error("The screen mode is not supported!");
		default:
			DSys::Logger::Print("Falling back to defaults, DSys::sv_iScreenWidthxDSys::sv_iScreenHeight");
			DMacro_TraceLeave();
			return ChangeRes(DSys::sv_iScreenWidth,DSys::sv_iScreenHeight,32);	
	}
}

bool DSys::Window::ChangeSize(int width, int height){
	return bool(MoveWindow(hWnd, 0,0, width, height, FALSE));
}

void DSys::Window::SetTitle(const char *title){
	SetWindowText(hWnd, title);
}

void DSys::Window::SwapGLBuffers(){
	SwapBuffers(hDC);
}