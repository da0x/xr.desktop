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

#ifndef ___DSYS_INPUT_H
#define ___DSYS_TIMER_H

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


namespace DSys {

	class Input {
		public:
			enum {
				KEY_RELESED,
				KEY_HELD,
				DIM_MAX	= 4
			};
	
		private:
			static bool					ready;				// Ready?
			static unsigned char		keyBuffer[256];		// The keyboard's key buffer
			static unsigned char		keyCharBuffer[256];	// Key char processed?
			static DIMOUSESTATE			mouseState;			// The mouse's 'buffer'
			static unsigned char		mouseCharBuffer[4];	// Mouse char processed?
			static bool 				mouseDoubleClick;	// did we get a double click message?
			static LPDIRECTINPUT8		lpdi;				// Main DirectInput object
			static LPDIRECTINPUTDEVICE8	lpdiKeyboard;		// Keyboard's DirectInput device
			static LPDIRECTINPUTDEVICE8	lpdiMouse;			// Mouse's DirectInput device
	
			static void GetWheelMovement();
			static void GetMouseMovement();
			
			static int					x;					// Mouse positions
			static int					y;
			static int					z;					// For wheel movement
			static int					dx;					// Mouse positions
			static int					dy;
			static int					dz;					// For wheel movement
		public:
			
	
			static void Init(HWND,HINSTANCE);
			static void Shutdown();
			static void Update();
	
			static bool KeyDown(unsigned char key);
			static int KeyValue(unsigned char key);
			static bool KeyChar(unsigned char key);
			static bool MouseDown(unsigned char button);
			static bool MouseChar(unsigned char button);
			static void DoubleClick( bool Set );
			static bool DoubleClick();
			static int	MouseX();
			static int	MouseY();
			static int	MouseZ();
			static int	MouseXDelta();
			static int	MouseYDelta();
			static int	MouseZDelta();
	};

}

#endif // ___DSYS_TIMER_H