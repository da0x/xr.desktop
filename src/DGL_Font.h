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

#ifndef ___DGL_FONT_H
#define ___DGL_FONT_H


#include "DGL_Color.h"

namespace DGL {

	class Texture;
		
	/*
	=================================================================

	FONTS
	
	=================================================================
	*/

	class Font {
		public:
			//
			// Interface
			//
			void Create(const char *name);			
			void Print(int x, int y, const char *fmt, ...) const;
			void Print3D( Vector pos, const char *fmt, ...) const;
			void SetColor(Color &color) { this->color = color; }
			void SetSize(float size) { this->size = size; }
			enum EStyle {
				STYLE_LEFT		= 0x00000001,
				STYLE_RIGHT		= 0x00000002,
				STYLE_CENTER	= 0x00000004,
				STYLE_SHADOWS	= 0x00000008
			};
			void SetStyle(int style) { this->style = style; }
			
			inline Font& operator = (Font& f)
			{
				this->size = f.size;
				this->style = f.style;
				this->color = f.color;
				this->font = f.font;
				return *this;
			}

			Font() :
				size(1),
				style(STYLE_LEFT|STYLE_SHADOWS),
				color(1.0f,1.0f,1.0f),
				font(0) { }

		private:
			float			size;
			int				style;
			Color			color;
			int				font;

		public:
			static void Init(void);
			static void Kill(void);
			static int	CreateFont(const char *name);
			static void Print2D( int x, int y, const char *fmt, float size, int attribs, const float* color, int font, ...);
			static void Print3D( float x, float y, float z, const char *fmt, float size, int attribs, const float* color, int font,	...);
			static float CharWidth();
			static float CharHeight();
		private:
			static bool			valid;
			static GLuint		charList;
			static const int	MAX_FONTS	= 0x0100;
			static Texture		fontTexture[MAX_FONTS];
			static char			fontTextureName[MAX_FONTS][64];
			static int			currentFree;
			static float		orthoMatrix[16];
			static bool			orthoMatrixValid;
			static float		shadowColor[4];

			static void GenerateFontLists(GLuint& ListID, float char_width,float char_height, float xspacing);
			static void PrintToOrtho(int x,int y, const char *fmt, int fmtMax, int charindex, const float *color, float scale);
			static void PrintToScene(float x,float y,float z, const char *fmt, int fmtMax, int charindex, const float *color, float scale);
	};
}


#endif // ___DGL_FONT_H

// GLFT_Font (http://polimath.com/blog/code/glft_font/)
//  by James Turk (james.p.turk@gmail.com)
//  Based on work by Marijn Haverbeke (http://marijn.haverbeke.nl)
//
// Version 0.2.1 - Released 2 March 2008 - Updated contact information.
// Version 0.2.0 - Released 18 July 2005 - Added beginDraw/endDraw, 
//                                       Changed vsprintf to vsnprintf
// Version 0.1.0 - Released  1 July 2005 - Initial Release
//
// Copyright (c) 2005-2008, James Turk
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, 
//      this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the GLFT_Font nor the names of its contributors
//      may be used to endorse or promote products derived from this software 
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.


#ifndef GLFT_FONT_HPP
#define GLFT_FONT_HPP

#include <GL/gl.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdarg>
#include <sstream>

class FTLibraryContainer
{
public:
    FTLibraryContainer();
    ~FTLibraryContainer();

    FT_Library& getLibrary();

private:
    static FT_Library library_;
};

class StreamFlusher { };
std::ostream& operator<<(std::ostream& os, const StreamFlusher& rhs);

class GLFT_Font
{
public:
    GLFT_Font();
    GLFT_Font(const std::string& filename, unsigned int size);

    ~GLFT_Font();

    void open(const std::string& filename, unsigned int size);
    void release();
    
    bool isValid() const;

    void drawText(float x, float y, const char *str, ...) const;
    void drawText(float x, float y, const std::string& str) const;
	void drawText(float x, float y, const std::string& str, float scale ) const;
    
    std::ostream& beginDraw(float x, float y);
    StreamFlusher endDraw();
    
    unsigned int calcStringWidth(const std::string& str) const;
    unsigned int getHeight() const;

private:
    // leave copy constructor and operator= undefined to make noncopyable
    GLFT_Font(const GLFT_Font&);
    const GLFT_Font& operator=(const GLFT_Font&);
    
private:
    // font data
    unsigned int texID_;
    unsigned int listBase_;
    std::vector<unsigned char> widths_;
    unsigned char height_;
    // stream drawing stuff
    std::ostringstream ss_;
    float drawX_;
    float drawY_;

    static const unsigned int SPACE = 32;
    static const unsigned int NUM_CHARS = 96;

    static FTLibraryContainer library_;
};

#endif //GLFT_FONT_HPP