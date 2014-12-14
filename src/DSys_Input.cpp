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

#include "DSys.H"

/*
TODO List:
- ...
*/

#pragma comment(lib, "Dxguid.lib")
#pragma comment(lib, "Dinput8.lib")


///////////////
// Variables //
///////////////

bool					DSys::Input::ready				= false;
unsigned char			DSys::Input::keyBuffer[256]		= {0};
unsigned char			DSys::Input::keyCharBuffer[256]	= {0};	
unsigned char			DSys::Input::mouseCharBuffer[4]	= {0};
DIMOUSESTATE			DSys::Input::mouseState;
bool					DSys::Input::mouseDoubleClick	= false;
LPDIRECTINPUT8			DSys::Input::lpdi;
LPDIRECTINPUTDEVICE8	DSys::Input::lpdiKeyboard;
LPDIRECTINPUTDEVICE8	DSys::Input::lpdiMouse;
int						DSys::Input::x = 0;
int						DSys::Input::y = 0;
int						DSys::Input::z = 0;
int						DSys::Input::dx = 0;
int						DSys::Input::dy = 0;
int						DSys::Input::dz = 0;

///////////////
// Functions //
///////////////

void DSys::Input::Init(HWND hWnd, HINSTANCE hInstance){
	HRESULT hr;		// DirectX result variable

	DSys::Logger::Print("--------------------------");
	DSys::Logger::Print(" DirectInput Initiation");
	Logger_EnterLevel();
	DSys::Logger::Print(" DirectInput Version: %d",DIRECTINPUT_VERSION);

	// Create the main DirectInput object
	hr = (DirectInput8Create( hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpdi, NULL));
	if ( FAILED(hr) ) {
		DSys::Logger::Error( "Could not create main DirectInput object");
			return;
	}
	// Create the keyboard's device object
	hr = (lpdi->CreateDevice( GUID_SysKeyboard, &lpdiKeyboard, NULL));
	if ( FAILED(hr) ) {
		DSys::Logger::Error( "Could not create main keyboard's object");
		Shutdown();
		return;
	}

	// Set the keyboard's data format 
	hr = (lpdiKeyboard->SetDataFormat(&c_dfDIKeyboard));
	if ( FAILED(hr) ) {
		DSys::Logger::Error( "Could not set keyboard's data format");
		Shutdown();
		return;
	}
	/*
	// Set the keyboard's cooperation level with your computer's environment.
	hr = (lpdiKeyboard->SetCooperativeLevel( hWnd, DISCL_BACKGROUND|DISCL_NONEXCLUSIVE));
	if ( FAILED(hr) ) {
		DSys::Logger::Error( "Could not set keyboard's cooperation level");
		Shutdown();
		return;
	}
	*/
	// And finally, acquire the keyboard for use.
	lpdiKeyboard->Acquire();

	// Its time to get mouse-y
	hr = lpdi->CreateDevice( GUID_SysMouse, &lpdiMouse, NULL);
	if ( FAILED(hr) ) {
		DSys::Logger::Error( "Could not set create the mouse device");
		Shutdown();
		return;
	}

	hr = lpdiMouse->SetDataFormat(&c_dfDIMouse);
	if ( FAILED(hr) ) {
		DSys::Logger::Error("Could not set the mouse's data format");
		Shutdown();
		return;
	}
	/*
	hr = lpdiMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);
	if ( FAILED(hr) ) {
		DSys::Logger::Error("Could not set the mouse's behavior");
		Shutdown();
		return;
	}
	*/
	hr = lpdiMouse->Acquire();
	if ( FAILED(hr) ) {
		DSys::Logger::Error("Could not acquire the mouse");
		Shutdown();
	}

	hr = lpdiMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
	if ( FAILED(hr) ) {
		DSys::Logger::Error("The mouse has been lost in initialization");
		Shutdown();
	}

	ready = true;
	Update();
	DSys::Logger::Print("Initialization successeeded");
	DSys::Logger::Print("---------------------------");
}


void DSys::Input::Shutdown(){
	DSys::Logger::Print("DirectInput shutdown...");

	ready = false;

	// First, since we create the mouse last, we need to 'destroy' it first.
	if (lpdiMouse!=NULL)	{
		// Unacquire the mouse
		lpdiMouse->Unacquire();

		// Throw the mouse away
		lpdiMouse->Release();
		lpdiMouse= NULL;
	}

	if (lpdiKeyboard!=NULL) {
		// Unacquire the keyboard
		lpdiKeyboard->Unacquire();
		
		// Let the keyboard go faster than your ex-girlfriend's phone number
		lpdiKeyboard->Release();
		lpdiKeyboard= NULL;
	}
	
	// Now the main DirectInput object
	if (lpdi!=NULL) {
		// Kill, burn, and destroy the main object
		lpdi->Release();
		lpdi=NULL;
	}

}


void DSys::Input::Update() {
	if(!ready) return;
	HRESULT hr;
	
	// First, check to see if the keyboard is still working/functioning
	hr = (lpdiKeyboard->GetDeviceState(sizeof(unsigned char[256]),(LPVOID)&keyBuffer));
	if ( FAILED(hr) ) { 
		if ( hr==DIERR_INPUTLOST ) {
			// Try to re-acquire the keyboard
			hr = (lpdiKeyboard->Acquire());
			if ( FAILED(hr) ) {
				DSys::Logger::Error("DirectInput8 lost the keyboard device");
				Shutdown();
			}
		}
	}

	// Now check the mouse
	hr = (lpdiMouse->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&mouseState));
	if ( FAILED(hr) ) { 
		if ( hr==DIERR_INPUTLOST ) {
			// Try to re-acquire the keyboard
			hr = (lpdiMouse->Acquire());
			if ( FAILED(hr) ) {
				DSys::Logger::Error("DirectInput8 has lost the mouse device");
				Shutdown();
			}
		}
	}

	// Check for chars	
	for(int i=0; i<256; i++){
		if(!keyBuffer[i])
			keyCharBuffer[i] = NULL;
	}

	for(int i=0; i< 4; i++){
		if(!mouseState.rgbButtons[i])
			mouseCharBuffer[i] = NULL;
	}
	
	GetWheelMovement();
	GetMouseMovement();
}


void DSys::Input::DoubleClick( bool Set )
	{
	mouseDoubleClick = Set;
	}

bool DSys::Input::DoubleClick()
	{
	bool dc = mouseDoubleClick;
	mouseDoubleClick = false;
	return dc;
	}

void DSys::Input::GetWheelMovement() {
	dz = mouseState.lZ;	
	z += mouseState.lZ;
}

void DSys::Input::GetMouseMovement() {
	dx = mouseState.lX;		
	dy = mouseState.lY;

	
	POINT Mouse;
	GetCursorPos( &Mouse );
	x = Mouse.x;
	y = Mouse.y;
}

bool DSys::Input::KeyDown(unsigned char key){
	if(keyBuffer[key] & 0x80)
		return true;
	return false;
}
int DSys::Input::KeyValue(unsigned char key){
	return (keyBuffer[key]);
}
bool DSys::Input::KeyChar(unsigned char key){
	if(keyBuffer[key]){
		if(!keyCharBuffer[key]){
			keyCharBuffer[key] = true;
			return true;
		}
	}
	return false;
}
bool DSys::Input::MouseDown(unsigned char button){
	if(mouseState.rgbButtons[button] & 0x80)
		return true;
	return false;
}
bool DSys::Input::MouseChar(unsigned char button){
	if(mouseState.rgbButtons[button]){
		if(!mouseCharBuffer[button]){
			return (mouseCharBuffer[button] = true);
		}
	}
	return false;
}
int	DSys::Input::MouseXDelta(){
	return dx;
}
int	DSys::Input::MouseYDelta(){
	return dy;
}
int DSys::Input::MouseZDelta(){
	int delta = dz;
	dz = 0;
	return delta;
}
int	DSys::Input::MouseX(){
	return x;
}
int	DSys::Input::MouseY(){
	return y;
}
int DSys::Input::MouseZ(){
	return z;
}