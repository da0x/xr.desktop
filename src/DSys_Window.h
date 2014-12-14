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


#ifndef ___DSYS_WINDOW_H
#define ___DSYS_WINDOW_H

namespace DSys {

	class Window {
		public:
			struct Info {
				const char *title;
				int			width;
				int			height;
				bool		desktop;
				bool		fullscreen;

				int			colorBits;
				int			alphaBits;
				int			depthBits;
				int			stencilBits;
				int			accumBits;
				int			auxBuffers;
			};

			static bool Create( Info* );
			static void Destroy( void);
			static bool ChangeRes(int width, int height, int bits);
			static bool ChangeSize(int width, int height);
			static void SetTitle( const char *);	// Changes The Text On The Title Bar
			static void SwapGLBuffers();			// Swaps the gl buffers

			static HDC	GetDC(){return hDC;}
			static HWND GetHWND(){return hWnd;}
			static bool Active(){return active;}
			static int	Width(){return width;}
			static int	Height(){return height;}
			
		private:
			static LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
			static bool				active;			// Active?
			static bool				isDesktop;		// Are we embedding in the desktop?
			static bool				isFullscreen;	// Are we in fullscreen mode?

			static HDC				hDC;			// Device Context
			static HGLRC			hRC;			// GL Rendering Context
			static HWND				hWnd;			// Window Handle
			static HINSTANCE		hInstance;		// Instance Handle

			static int				width;
			static int				height;
		
	};


}

#endif // ___DSYS_WINDOW_H