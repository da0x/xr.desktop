/*//C++

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

#ifndef ___FONT3D_H
#define ___FONT3D_H

/*
TODO List:
- ...
*/

namespace DGL {

	class Font3D {
		protected:
			bool	ready;
			GLuint	base;

			GLYPHMETRICSFLOAT gmf[256];

		public:
			GLvoid Build(const char *fontname, HDC hDC)								// Build Our Bitmap Font
			{
				LogPrint("3D Font Building \"%s\"",fontname);
				HFONT	font;										// Windows Font ID

				base = glGenLists(256);								// Storage For 256 Characters

				font = CreateFont(	
					-12,							// Height Of Font
					0,								// Width Of Font
					0,								// Angle Of Escapement
					0,								// Orientation Angle
					FW_BOLD,						// Font Weight
					FALSE,							// Italic
					FALSE,							// Underline
					FALSE,							// Strikeout
					ANSI_CHARSET,					// Character Set Identifier
					OUT_TT_PRECIS,					// Output Precision
					CLIP_DEFAULT_PRECIS,			// Clipping Precision
					NONANTIALIASED_QUALITY/*ANTIALIASED_QUALITY*/,			// Output Quality
					FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
					fontname);						// Font Name

				SelectObject(hDC, font);							// Selects The Font We Created

				wglUseFontOutlines(	hDC,							// Select The Current DC
					0,								// Starting Character
					255,							// Number Of Display Lists To Build
					base,							// Starting Display Lists
					0.0f,							// Deviation From The True Outlines
					0.2f,							// Font Thickness In The Z Direction
					WGL_FONT_POLYGONS,				// Use Polygons, Not Lines
					gmf);							// Address Of Buffer To Recieve Data
				ready = true;
			}


			~Font3D()								// Delete The Font
			{
				glDeleteLists(base, 256);								// Delete All 256 Characters
			}

			Font3D(){
				ready = false;
			}


			GLvoid Print(const char *fmt, ...)					// Custom GL "Print" Routine
			{
				if(!ready) return;
				float		length=0;								// Used To Find The length Of The Text
				char		text[256];								// Holds Our String
				va_list		ap;										// Pointer To List Of Arguments

				if (fmt == NULL)									// If There's No Text
					return;											// Do Nothing

				va_start(ap, fmt);									// Parses The String For Variables
				vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
				va_end(ap);											// Results Are Stored In Text

				int len = (strlen(text));

				for (int loop=0;loop<len;loop++){
					length+=gmf[text[loop]].gmfCellIncX;
				}

				glPushMatrix();
				glTranslatef(-length/2,0.0f,0.0f);					// Center Our Text On The Screen

				glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
				glListBase(base);									// Sets The Base Character to 0
				glCallLists(len, GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
				glPopAttrib();										// Pops The Display List Bits

				glPopMatrix();
			}
	};

}//namespace DGL


#endif // ___FONT3D_H