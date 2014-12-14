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

#ifndef ___DAVI_H
#define ___DAVI_H

/*
TODO List:
- ...
*/

#pragma once
#ifndef ___D_SHARED_H
#include "D_Shared.H"
#endif

/* ----------------- *
 *     AVI CLASS     *
 * ----------------- */
#include <vfw.h>							// Header File For Video For Windows
#pragma comment( lib, "vfw32.lib")

namespace DGL {
	class Avi{
	protected:
		AVISTREAMINFO		psi;				// Pointer To A Structure Containing Stream Info
		PAVISTREAM			pavi;				// Handle To An Open Stream
		PGETFRAME			pgf;				// Pointer To A GetFrame Object
		BITMAPINFOHEADER	bmih;				// Header Information For DrawDibDraw Decoding
		long				lastframe;			// Last Frame Of The Stream
		int					width;				// Video Width
		int					height;				// Video Height
		char				*pdata;				// Pointer To Texture Data
		int					mpf;				// Will Hold Rough Milliseconds Per Frame

		int					next;				// Next Frame Time
		int					aviFrame;			// Frame Number
		int					aviFramePrv;		// Priveous Frame
		bool				ready;				// Ready?
		long				totalTime;			// Total Avi Time
		long				currentTime;		// Current Time

		HDRAWDIB			hdd;				// Handle For Our Dib
		HBITMAP				hBitmap;			// Handle To A Device Dependant Bitmap
		HDC					hdc;
		unsigned char*		data;				// Pointer To Our Resized Image
		GLuint				tex;				// Holds the texture
		int					dataFormat;


		void flipIt(void* buffer){				// Flips The Red And Blue Bytes (256x256)
			void* b = buffer;					// Pointer To The Buffer
			__asm {								// Assembler Code To Follow
					mov ecx, 256*256			// Set Up A Counter (Dimensions Of Memory Block)
					mov ebx, b					// Points ebx To Our Data (b)
				label:							// Label Used For Looping
					mov al,[ebx+0]				// Loads Value At ebx Into al
					mov ah,[ebx+2]				// Loads Value At ebx+2 Into ah
					mov [ebx+2],al				// Stores Value In al At ebx+2
					mov [ebx+0],ah				// Stores Value In ah At ebx
					
					add ebx,3					// Moves Through The Data By 3 Bytes
					dec ecx						// Decreases Our Loop Counter
					jnz label					// If Not Zero Jump Back To Label
			}
		}
	public:
		Avi(){
			next = 0;
			ready = false;
		}

		void OpenAVI(LPCSTR szFile){				// Opens An AVI File (szFile)
			hdc = CreateCompatibleDC(0);
			hdd = DrawDibOpen();					// Grab A Device Context For Our Dib

			AVIFileInit();							// Opens The AVIFile Library

			// Opens The AVI Stream
			if (AVIStreamOpenFromFile(&pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL) !=0){
				// An Error Occurred Opening The Stream
				LogError( "Failed To Open The AVI Stream '%s'",szFile);
				return;
			}

			AVIStreamInfo(pavi, &psi, sizeof(psi));				// Reads Information About The Stream Into psi
			width=psi.rcFrame.right-psi.rcFrame.left;			// Width Is Right Side Of Frame Minus Left
			height=psi.rcFrame.bottom-psi.rcFrame.top;			// Height Is Bottom Of Frame Minus Top

			lastframe=AVIStreamLength(pavi);					// The Last Frame Of The Stream

			totalTime = AVIStreamSampleToTime(pavi,lastframe);
			mpf=totalTime/lastframe;							// Calculate Rough Milliseconds Per Frame
			

			bmih.biSize		= sizeof (BITMAPINFOHEADER);		// Size Of The BitmapInfoHeader
			bmih.biPlanes		= 1;					// Bitplanes
			bmih.biBitCount		= 24;					// Bits Format We Want (24 Bit, 3 Bytes)
			bmih.biWidth		= 256;					// Width We Want (256 Pixels)
			bmih.biHeight		= 256;					// Height We Want (256 Pixels)
			bmih.biCompression	= BI_RGB;				// Requested Mode = RGB

			hBitmap = CreateDIBSection (hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&data), NULL, NULL);
			SelectObject (hdc, hBitmap);					// Select hBitmap Into Our Device Context (hdc)

			pgf=AVIStreamGetFrameOpen(pavi, NULL);				// Create The PGETFRAME Using Our Request Mode
			if (pgf==NULL)
			{
				// An Error Occurred Opening The Frame
				LogError("Failed To Open The AVI Frame '%s'",szFile);
				return;
			}

			glGenTextures(1,&tex);
			glBindTexture( GL_TEXTURE_2D, tex);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// Set Texture Max Filter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// Set Texture Min Filter

			// Create The Texture
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			ready = true;
			aviFramePrv = 0;
			LogPrint("Loading AVI file to texture '%s'\n - Dim=(%dx%d)\n - Frames=(%d)\n - Time= %s",
				szFile, width, height, lastframe, D_Msec2String(totalTime));
			dataFormat = Extensions::IsEnabled_GL_EXT_bgra() ? GL_BGRA_EXT : GL_RGBA;
		}
		
		GLuint GrabAVIFrame(DWORD milliseconds){
			if(!ready)
				return false;
			next+= milliseconds;
			aviFrame=(next/mpf)%lastframe;

			if(aviFrame == aviFramePrv){
				glBindTexture(GL_TEXTURE_2D, tex);
				return tex;
			} else {
				aviFramePrv = aviFrame;
			}

			currentTime = AVIStreamSampleToTime(pavi,aviFrame);

			LPBITMAPINFOHEADER lpbi;					// Holds The Bitmap Header Information
			lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, aviFrame);	// Grab Data From The AVI Stream
			pdata=(char *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame
			// (Skip The Header Info To Get To The Data)
			// Convert Data To Requested Bitmap Format
			DrawDibDraw (hdd, hdc, 0, 0, 256, 256, lpbi, pdata, 0, 0, width, height, 0);

			if( Extensions::IsEnabled_GL_EXT_bgra() ){
				flipIt(data);
			}

			// Update The Texture
			glBindTexture( GL_TEXTURE_2D, tex);
			glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, 256, 256, dataFormat, GL_UNSIGNED_BYTE, data);

			return tex;
		}

		long GetCurrentAVITime(){
			return currentTime;
		}

		void CloseAVI(void)							// Properly Closes The Avi File
		{
			DeleteObject(hBitmap);					// Delete The Device Dependant Bitmap Object
			DrawDibClose(hdd);						// Closes The DrawDib Device Context
			AVIStreamGetFrameClose(pgf);			// Deallocates The GetFrame Resources
			AVIStreamRelease(pavi);					// Release The Stream
			AVIFileExit();							// Release The File
			glDeleteTextures(1, &tex);
			ready = false;
		}

		~Avi(){
			if (ready)
				CloseAVI();
		}
	};
}//namespace DGL

#endif // ___DAVI_H