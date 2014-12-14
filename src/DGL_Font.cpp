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

/*
TODO List:
- ...
*/

#define NOMINMAX
#include "DGL_Main.h"
#include "DGL_Font.h"
#include "DGL_Texture.h"
#include "DSys.h"

#define CHAR_HEIGHT		10.0f
#define CHAR_WIDTH		10.0f
#define CHAR_XSPACING	7.75f

namespace DGL {

	//
	// Interface
	//

	void Font::Create(const char *name)
	{
		this->font = Font::CreateFont(name);
	}

	void Font::Print( int x, int y, const char *fmt, ...) const
	{
		va_list arg;
		char	text[256];
		va_start( arg, fmt);
		vsprintf( text, fmt, arg);
		Font::Print2D(	x,
						y,
						text,
						this->size,
						this->style,
						this->color,
						this->font);
		va_end(arg);
	}

	void Font::Print3D( Vector pos, const char *fmt, ...) const
	{
		va_list arg;
		char	text[256];
		va_start( arg, fmt);
		vsprintf( text, fmt, arg);

		Font::Print3D(pos.x_(),pos.y_(),pos.z_(),text,this->size,this->style,this->color,this->font);
		va_end(arg);
	}


	//
	// Background
	//

	bool			Font::valid	= false;
	GLuint			Font::charList;
	Texture			Font::fontTexture[MAX_FONTS];
	char			Font::fontTextureName[MAX_FONTS][64];
	int				Font::currentFree;
	float			Font::orthoMatrix[16];
	bool			Font::orthoMatrixValid;
	float			Font::shadowColor[4] = {0.00f,0.00f,0.00f,1.00f};


	/*
	---------------------
	Generate Font Lists
	---------------------
	*/
	
	void Font::GenerateFontLists(GLuint& ListID, float char_width,float char_height, float xspacing){
		DGL::LogPrint("Creating GL List For 256 Character...", char_width, char_height);
		float	s1;
		float	t1;
		float	s2;
		float	t2;
		s1=t1=s2=t2= 0.0f;

		float	height = char_height/ 2.0f;
		float	width =	char_width/ 2.0f;
		ListID= glGenLists(256);	// generate storage for the lists
		t1= 1.0f;	t2= t1+ ((16.0f) / (256.0f));
		for(int j= 16; j> 0; j--){
			t1-= ((16.0f) / (256.0f));	t2= t1+ ((16.0f) / (256.0f));
			glColor4f(1.0f,0.0f,0.0f,1.0f);
			for(int i=0; i<16; i++){
				glTranslatef(2.0f,0.0f,0.0f);
				glNewList( ListID++, GL_COMPILE);
				glBegin(GL_QUADS);
					glTexCoord2f(s1,t1);	glVertex3f(-height,-width,0.0f);
					glTexCoord2f(s2,t1);	glVertex3f( height,-width,0.0f);
					glTexCoord2f(s2,t2);	glVertex3f( height, width,0.0f);
					glTexCoord2f(s1,t2);	glVertex3f(-height, width,0.0f);
				glEnd();
				glTranslated(xspacing,0,0);
				glEndList();
				s1+= ((16.0f) / (256.0f));s2= s1+ ((16.0f) / (256.0f));
			}
		}
		ListID-= (GLuint)(256);		// reset char to the begining of the lists
	}

	/*
	--------------------
	Init Fonts
	--------------------
	*/
	void Font::Init(void){
		DGL::LogPrint("Initializing Font");

		// now create gl lists for small and large font
		GenerateFontLists(charList, CHAR_WIDTH, CHAR_HEIGHT, CHAR_XSPACING);
		currentFree = 0;
		valid= true;
	}
	
	/*
	--------------------
	Kill Fonts
	--------------------
	*/
	void Font::Kill(void){
		DGL::LogPrint("Deleting Font GL Lists ...");
		glDeleteLists( charList, 256);
		while(currentFree--){
			fontTexture[currentFree].Delete();
		}
		currentFree = 0;
		valid= false;
	}
	
	/*
	--------------------
	Create Font
	--------------------
	*/
	int Font::CreateFont(const char *name){
		if(currentFree >= MAX_FONTS){
			DGL::LogError("MAX_FONTS reached.");
			return 0;
		}

		// check if the texture is already loaded
		for(int i=0; i< currentFree; i++){
			if(!_strcmpi(name, fontTextureName[i])){
				DGL::LogPrint("Font \"%s\" already loaded, using current.",name);
				return i;
			}
		}

		char TexName[MAX_PATH];
		sprintf(TexName, "textures/fonts/%s.tga", name);
		DGL::LogPrint("Creating Font %i \"%s\"", currentFree, name);
		fontTexture[currentFree].Build(TexName);
		strcpy(fontTextureName[currentFree], name);
		return currentFree++;
	}
    
	/*
	=======================================

	2D Printing

	=======================================
	*/
	void Font::PrintToOrtho(
			int				x,
			int				y,
			const char		*fmt,
			int				fmtMax,
			int				charindex,
			const float*	color,
			float			scale
	){
		if(!valid)	return;
		GLuint ch=0;

		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);	glPushMatrix(); glLoadIdentity();
		if(!orthoMatrixValid){
			glOrtho(0,DSys::sv_iScreenWidth,0,DSys::sv_iScreenHeight,-100,100);
			glGetFloatv ( GL_PROJECTION_MATRIX, orthoMatrix);
			orthoMatrixValid = true;
		} else {
			glLoadMatrixf(orthoMatrix);
		}
		glMatrixMode(GL_MODELVIEW);		glPushMatrix(); glLoadIdentity();
		glLoadIdentity();
		glBindTexture( GL_TEXTURE_2D, fontTexture[charindex]);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
		glColor4fv(color);
		glTranslated(x,y,0);
		glListBase(charList);
		glScalef(scale,scale,scale);
		glCallLists(fmtMax, GL_UNSIGNED_BYTE, fmt);
		glColor4f(1,1,1,1);
		glMatrixMode(GL_PROJECTION);	glPopMatrix();
		glMatrixMode(GL_MODELVIEW);		glPopMatrix();
		glPopAttrib();
	}
        
	void Font::Print2D(
			int					x,
			int					y,
			const char			*fmt,
			float				size,
			int					attribs,
			const float*		color,
			int					font,
			...
	){
		static int		xspacing = CHAR_XSPACING;
		static char		buf[1024*24];		// use large buffer
		static int		len;
		static va_list	ap;

		// if no fonts are available or if out of screen skip printing
		if( !currentFree || x < 0 || x > DSys::sv_iScreenWidth || y < 0 || y > DSys::sv_iScreenHeight) return;

		va_start( ap, font);
		vsprintf( buf, fmt, ap);
		va_end( ap);

		len = strlen( buf);

		if ( attribs & STYLE_CENTER){
			x-= ( len * ( xspacing/2 ) + xspacing/2 ) * size;
		} else {
			if( attribs & STYLE_RIGHT){
				x-= (len)* xspacing* size;
			}
		}
		Clamp<int>(0, currentFree-1, font);
		shadowColor[3] = color[3];
		if( attribs & STYLE_SHADOWS ){
			PrintToOrtho(x+1,y-1,buf,len,font,shadowColor,size);
		}
		PrintToOrtho(x,y,buf,len,font,color,size);
	}




	/*
	=======================================
	
	  3D Printing
	
	=======================================
	*/


	void Font::PrintToScene (
			float			x,
			float			y,
			float			z,
			const char		*fmt,
			int				fmtMax,
			int				charindex,
			const float*	color,
	 		float			scale
	){
		if(!valid)	return;
		GLuint ch=0;	

		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		
		glPushMatrix();
	
		glBindTexture( GL_TEXTURE_2D, fontTexture[charindex]);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4fv(color);
	
		glTranslatef(x,y,z);
		glListBase(charList);
		glScalef(scale,scale,scale);
		glCallLists(fmtMax, GL_UNSIGNED_BYTE, fmt);
		glColor4f(1,1,1,1);
		glPopMatrix();
		glPopAttrib();
	}

	void Font::Print3D(
			float				x,
			float				y,
			float				z,
			const char			*fmt,
			float				size,
			int					attribs,
			const float*		color,
			int					font,
		...
	){
		static int		xspacing = CHAR_XSPACING;
		static char		buf[1024*24];		// use large buffer
		static int		len;
		static va_list	ap;
	
		va_start(ap,font);
		vsprintf( buf, fmt, ap);
		va_end(ap);

		len = strlen(buf);
	
			if ( attribs & STYLE_CENTER){
			x-= len* (xspacing/2)* size;
		} else {
			if( attribs & STYLE_RIGHT){
				x-= len* xspacing* size;
			}
		}
		Clamp<int>(0, currentFree-1, font);
		PrintToScene(x,y,z,buf,len,font,color,size);
	}

	float Font::CharWidth(){return CHAR_XSPACING;}
	float Font::CharHeight(){return CHAR_HEIGHT;}

}//namespace DGL

// GLFT_Font (http://polimath.com/blog/code/glft_font/)
//  by James Turk (james.p.turk@gmail.com)
//  Based on work by Marijn Haverbeke (http://marijn.haverbeke.nl)
//
// Version 0.2.1 - Released 2 March 2008 - Updated contact information.
// Version 0.2.0 - Released 18 July 2005 - Added beginDraw/endDraw,
//                                       Changed vsprintf to vsnprintf
// Version 0.1.0 - Released 1 July 2005 - Initial Release
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


#include "DGL_Font.h"
#include <cstring>
#include <algorithm>


// static members
FT_Library FTLibraryContainer::library_;
FTLibraryContainer GLFT_Font::library_;

// FTLibraryContainer implementation //

FTLibraryContainer::FTLibraryContainer()
{
    if (FT_Init_FreeType(&library_) != 0)
    {
        throw std::runtime_error("Could not initialize FreeType2 library.");
    }
}

FTLibraryContainer::~FTLibraryContainer()
{
    FT_Done_FreeType(library_);
}

FT_Library& FTLibraryContainer::getLibrary()
{
    return library_;
}

std::ostream& operator<<(std::ostream& os, const StreamFlusher& rhs)
{
    return os.flush();
}

// GLFT_Font implementation //

GLFT_Font::GLFT_Font() :
    texID_(0), listBase_(0),    // initalize GL variables to zero
    widths_(NUM_CHARS),         // make room for 96 widths
    height_(0), drawX_(0), drawY_(0)
{
}

GLFT_Font::GLFT_Font(const std::string& filename, unsigned int size) :
    texID_(0), listBase_(0),    // initalize GL variables to zero
    widths_(NUM_CHARS),         // make room for 96 widths
    height_(0), drawX_(0), drawY_(0)
{
    open(filename, size);
}

GLFT_Font::~GLFT_Font()
{
    release();
}

void GLFT_Font::open(const std::string& filename, unsigned int size)
{
    const size_t MARGIN = 3;

    // release the font if it already exists
    if(isValid())
    {
        release();
    }

    // Step 1: Open the font using FreeType //
    FT_Face face;

    if(FT_New_Face(library_.getLibrary(), filename.c_str(), 0, &face) != 0)
    {
        throw std::runtime_error("Could not load font file.");
    }

    // Abort if this is not a scalable font.
    if(!(face->face_flags & FT_FACE_FLAG_SCALABLE) ||
        !(face->face_flags & FT_FACE_FLAG_HORIZONTAL))
    {
        throw std::runtime_error("Invalid font: Error setting font size.");
    }

    // Set the font size
    FT_Set_Pixel_Sizes(face, size, 0);

    // Step 2: Find maxAscent/Descent to calculate imageHeight //
    size_t imageHeight = 0;
    size_t imageWidth = 256;
    int maxDescent = 0;
    int maxAscent = 0;
    size_t lineSpace = imageWidth - MARGIN;
    size_t lines = 1;
    size_t charIndex;

    for(unsigned int ch = 0; ch < NUM_CHARS; ++ch)
    {
        // Look up the character in the font file.
        charIndex = FT_Get_Char_Index(face, ch+SPACE);

        // Render the current glyph.
        FT_Load_Glyph(face, charIndex, FT_LOAD_RENDER);

        widths_[ch] = (face->glyph->metrics.horiAdvance >> 6) + MARGIN;
        // If the line is full go to the next line
        if(widths_[ch] > lineSpace)
        {
            lineSpace = imageWidth - MARGIN;
            ++lines;
        }
        lineSpace -= widths_[ch];

        maxAscent = std::max(face->glyph->bitmap_top, maxAscent);
        maxDescent = std::max(face->glyph->bitmap.rows -
                                face->glyph->bitmap_top, maxDescent);
    }

    height_ = maxAscent + maxDescent;   // calculate height_ for text

    // Compute how high the texture has to be.
    size_t neededHeight = (maxAscent + maxDescent + MARGIN) * lines + MARGIN;
    // Get the first power of two in which it will fit
    imageHeight = 16;
    while(imageHeight < neededHeight)
    {
        imageHeight <<= 1;
    }

    // Step 3: Generation of the actual texture //

    // create and zero the memory
    unsigned char* image = new unsigned char[imageHeight * imageWidth];
    memset(image, 0, imageHeight * imageWidth);

    // These are the position at which to draw the next glyph
    size_t x = MARGIN;
    size_t y = MARGIN + maxAscent;
    float texX1, texX2, texY1, texY2;   // used for display list

    listBase_ = glGenLists(NUM_CHARS);  // generate the lists for filling

    // Drawing loop
    for(unsigned int ch = 0; ch < NUM_CHARS; ++ch)
    {
        size_t charIndex = FT_Get_Char_Index(face, ch+SPACE);

        // Render the glyph
        FT_Load_Glyph(face, charIndex, FT_LOAD_DEFAULT);
        FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

        // See whether the character fits on the current line
        if(widths_[ch] > imageWidth - x)
        {
            x = MARGIN;
            y += (maxAscent + maxDescent + MARGIN);
        }

        // calculate texture coordinates of the character
        texX1 = static_cast<float>(x) / imageWidth;
        texX2 = static_cast<float>(x+widths_[ch]) / imageWidth;
        texY1 = static_cast<float>(y - maxAscent) / imageHeight;
        texY2 = texY1 + static_cast<float>(height_) / imageHeight;

        // generate the character's display list
        glNewList(listBase_ + ch, GL_COMPILE);
        glBegin(GL_QUADS);
        glTexCoord2f(texX1,texY1);  glVertex2i(0,0);
        glTexCoord2f(texX2,texY1);  glVertex2i(widths_[ch],0);
        glTexCoord2f(texX2,texY2);  glVertex2i(widths_[ch],height_);
        glTexCoord2f(texX1,texY2);  glVertex2i(0,height_);
        glEnd();
        glTranslatef(widths_[ch],0,0);  // translate forward
        glEndList();

        // copy image generated by FreeType to the texture
        for(int row = 0; row < face->glyph->bitmap.rows; ++row)
        {
            for(int pixel = 0; pixel < face->glyph->bitmap.width; ++pixel)
            {
                // set pixel at position to intensity (0-255) at the position
                image[(x + face->glyph->bitmap_left + pixel) +
                    (y - face->glyph->bitmap_top + row) * imageWidth] =
                        face->glyph->bitmap.buffer[pixel +
                            row * face->glyph->bitmap.pitch];
            }
        }

        x += widths_[ch];
    }

    // generate the OpenGL texture from the byte array
    glGenTextures(1, &texID_);
    glBindTexture(GL_TEXTURE_2D, texID_);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, imageWidth, imageHeight, 0,
                    GL_ALPHA, GL_UNSIGNED_BYTE, image);

    delete[] image;     // now done with the image memory
    FT_Done_Face(face); // free the face data
}

void GLFT_Font::release()
{
    if(glIsList(listBase_))
    {
        glDeleteLists(listBase_, NUM_CHARS);
    }
    if(glIsTexture(texID_))
    {
        glDeleteTextures(1, &texID_);
    }

    // clear out data
    texID_ = 0;
    listBase_ = 0;
    widths_.clear();
    widths_.resize(NUM_CHARS);
    height_ = 0;
}

bool GLFT_Font::isValid() const
{
    return glIsTexture(texID_) == GL_TRUE;
}

void GLFT_Font::drawText(float x, float y, const char *str, ...) const
{
    if(!isValid())
    {
        throw std::logic_error("Invalid GLFT_Font::drawText call.");
    }

    std::va_list args;
    char buf[1024];

    va_start(args,str);
    vsnprintf(buf, 1024, str, args);   // avoid buffer overflow
    va_end(args);

    glBindTexture(GL_TEXTURE_2D, texID_);
    glPushMatrix();
    glTranslated(x,y,0);
    for(unsigned int i=0; i < strlen(buf); ++i)
    {
        unsigned char ch( buf[i] - SPACE );     // ch-SPACE = DisplayList offset
        // replace characters outside the valid range with undrawable
        if(ch > NUM_CHARS)
        {
            ch = NUM_CHARS-1;   // last character is 'undrawable'
        }
        glCallList(listBase_+ch);    // calculate list to call
    }

    // Alternative, ignores undrawables (no noticable speed difference)
    //glListBase(listBase_-32);
    //glCallLists(static_cast<int>(std::strlen(buf)), GL_UNSIGNED_BYTE, buf);

    glPopMatrix();
}

void GLFT_Font::drawText(float x, float y, const std::string& str) const
{
    if(!isValid())
    {
        throw std::logic_error("Invalid GLFT_Font::drawText call.");
    }

    glBindTexture(GL_TEXTURE_2D, texID_);
    glPushMatrix();
    glTranslated(x,y,0);
    for(std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        unsigned char ch( *i - SPACE ); // ch-SPACE = DisplayList offset
        // replace characters outside the valid range with undrawable
        if(ch > NUM_CHARS)
        {
            ch = NUM_CHARS-1;   // last character is 'undrawable'
        }
        glCallList(listBase_+ch);    // calculate list to call
    }

    // Alternative, ignores undrawables (no noticable speed difference)
    //glListBase(listBase_-32);
    //glCallLists(static_cast<int>(std::strlen(buf)), GL_UNSIGNED_BYTE, buf);

    glPopMatrix();
}

void GLFT_Font::drawText(float x, float y, const std::string& str, float scale ) const
{
    if(!isValid())
    {
        throw std::logic_error("Invalid GLFT_Font::drawText call.");
    }

    glBindTexture(GL_TEXTURE_2D, texID_);
    glPushMatrix();
    glTranslated(x,y,0);
	glScalef( scale,scale,scale );
    for(std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        unsigned char ch( *i - SPACE ); // ch-SPACE = DisplayList offset
        // replace characters outside the valid range with undrawable
        if(ch > NUM_CHARS)
        {
            ch = NUM_CHARS-1;   // last character is 'undrawable'
        }
        glCallList(listBase_+ch);    // calculate list to call
    }

    // Alternative, ignores undrawables (no noticable speed difference)
    //glListBase(listBase_-32);
    //glCallLists(static_cast<int>(std::strlen(buf)), GL_UNSIGNED_BYTE, buf);

    glPopMatrix();
}
std::ostream& GLFT_Font::beginDraw(float x, float y)
{
    // clear the string and store the draw-position
    ss_.str("");
    drawX_ = x;
    drawY_ = y;
    return ss_;
}

StreamFlusher GLFT_Font::endDraw()
{
    drawText(drawX_, drawY_, ss_.str());    // draw the string
    ss_.str("");                            // clear the buffer
    return StreamFlusher();
}

unsigned int GLFT_Font::calcStringWidth(const std::string& str) const
{
    if(!isValid())
    {
        throw std::logic_error("Invalid GLFT_Font::calcStringWidth call.");
    }
    unsigned int width=0;

    // iterate through widths of each char and accumulate width of string
    for(std::string::const_iterator i = str.begin(); i < str.end(); ++i)
    {
        width += widths_[static_cast<unsigned int>(*i) - SPACE];
    }

    return width;
}

unsigned int GLFT_Font::getHeight() const
{
    if(!isValid())
    {
        throw std::logic_error("Invalid GLFT_Font::getHeight call.");
    }
    return height_;
}
