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

/*
TODO List:
- ...
*/

#include "DSys.h"
#include "DGL_Main.h"
#include "DGL_Texture.h"
#include "DGL_Extensions.h"

#include <GL/GL.h>
#include <IL/il.h>
#include <IL/ilut.h>
#include <memory>
#include <map>

//#include <QtOpenGL/QGLWidget>
//#include <QtGui/QFileIconProvider>
//#include <QtGui/QPixmap>
//#include <QtGui/QImage>

#include <shlobj.h>  
#include <shlguid.h>  
#include <shellapi.h>  
#include <commctrl.h>  
#include <commoncontrols.h> 

//#pragma comment (lib,"QtGui4.lib")
//#pragma comment (lib,"QtOpenGL4.lib")

namespace DGL {


	/////////////////////////////////////
	// auxFilter : aux instance
	Texture::Filter		Texture::auxDefaultFilter(Filter::FilterName::TRILINEAR);

	/////////////////////////////////////
	// Texture::Filter
	Texture::Filter::Filter(int filter, bool noRepeat)
	{
		switch(filter)
			{
			case FilterName::NEAREST:
				this->textureGenMipmap = false;
				this->SetParameter( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				this->SetParameter( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				break;
			case FilterName::LINEAR:
				this->textureGenMipmap = false;
				this->SetParameter( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				this->SetParameter( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				break;
			case FilterName::BILINEAR:
				this->textureGenMipmap = true;
				this->SetParameter( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				this->SetParameter( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				break;
			case FilterName::TRILINEAR:
				this->textureGenMipmap = true;
				this->SetParameter( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				this->SetParameter( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				break;
		}

		if(!noRepeat)
		{
			this->textureWarpS			= GL_REPEAT;
			this->textureWarpT			= GL_REPEAT;
		}
		else
		{
			if(Extensions::IsEnabled_GL_EXT_texture_edge_clamp())
			{
				this->textureWarpS			= GL_CLAMP_TO_EDGE;
				this->textureWarpT			= GL_CLAMP_TO_EDGE;
			}
			else if (Extensions::IsEnabled_GL_SGIS_texture_edge_clamp())
			{
				this->textureWarpS			= GL_CLAMP_TO_EDGE_SGIS;
				this->textureWarpT			= GL_CLAMP_TO_EDGE_SGIS;
			}
			else
			{
				this->textureWarpS			= GL_CLAMP;
				this->textureWarpT			= GL_CLAMP;
			}
		}
	}

	/////////////////////////////////////
	// Texture::Filter : Copy constructor
	Texture::Filter::Filter(Texture::Filter& other)
	{
		this->textureGenMipmap = other.textureGenMipmap;
		this->textureMagFilter = other.textureMagFilter;
		this->textureMinFilter = other.textureMinFilter;
		this->textureWarpS = other.textureWarpS;
		this->textureWarpT = other.textureWarpT;
	}
	
	/////////////////////////////////////
	// Texture::Filter::SetParameter
	void Texture::Filter::SetParameter( GLenum target, GLenum pname, GLint param)
	{
		if(target != GL_TEXTURE_2D) return;
		switch (pname)
		{
			case GL_TEXTURE_MAG_FILTER:
				this->textureMagFilter = param;
				break;
			case GL_TEXTURE_MIN_FILTER:
				this->textureMinFilter = param;
				break;
			case GL_TEXTURE_WRAP_S:
				this->textureWarpS = param;
				break;
			case GL_TEXTURE_WRAP_T:
				this->textureWarpT = param;
				break;
		}
	}

	/////////////////////////////////////
	// Texture::Filter::GetParameter
	GLint Texture::Filter::GetParameter( GLenum pname)
	{
		switch (pname)
		{
			case GL_TEXTURE_MAG_FILTER:
				return this->textureMagFilter;
			case GL_TEXTURE_MIN_FILTER:
				return this->textureMinFilter;
			case GL_TEXTURE_WRAP_S:
				return this->textureWarpS;
			case GL_TEXTURE_WRAP_T:
				return this->textureWarpT;
		}
		return 0;
	}
	
	/////////////////////////////////////
	// Texture::Filter::Use
	void Texture::Filter::Use() const
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->textureMinFilter);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->textureMagFilter);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->textureWarpS);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->textureWarpT);
	}

	/////////////////////////////////////
	// Texture::Filter::IsMipmap
	bool Texture::Filter::IsMipmap() const
	{
		return this->textureGenMipmap;
	}

	
	/*
	==========================
	Build a texture
	==========================
	*/

	namespace global
		{
		typedef std::map<std::string, GLuint> database_type;

		database_type textureDatabase;
		}

	void Texture::Init()
		{
		static bool Inited;
		if( !Inited )
			{
			DSys::Logger::Print("Initializing DevIL v%i", ilGetInteger(IL_VERSION_NUM) );

			ilInit();
			iluInit();
			ilutInit();
			ilutRenderer(ILUT_OPENGL);
			Inited = true;
			}
		}

	void Texture::Build( const char *filename, Filter glFilter )
		{
		Init();
		ILuint ilTexture;
		global::database_type::iterator Iter = global::textureDatabase.find( filename );
		if( Iter != global::textureDatabase.end() )
			{ // a texture for filename already exists.
			this->owner = false;
			this->glTexture = Iter->second;
			return;
			}

			// otherwise we are the owners.
		this->path = filename;
		this->owner = true;

		ilGenImages(1, &ilTexture);
		ilBindImage(ilTexture);
		bool success = ilLoadImage(filename);
		if (!success)
			{
				// load the file's icon instead.
			DSys::Logger::Error("%s: '%s'", iluErrorString( ilGetError() ), filename );
			return;
			}

		GLint maxSize;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

		GLint			_width	= ilGetInteger(IL_IMAGE_WIDTH);
		GLint			_height	= ilGetInteger(IL_IMAGE_HEIGHT);
		if(	(_width > maxSize) || (_height > maxSize) || !IsPowerOfTwo(_height) || !IsPowerOfTwo(_width) )
			{
			Clamp <GLint> ( 1, maxSize, _width);
			Clamp <GLint> ( 1, maxSize, _height);
			_width = ClosestPowerOfTwo(_width);
			_height= ClosestPowerOfTwo(_height);

			success = iluScale( _width, _height, ilGetInteger(IL_IMAGE_DEPTH) );
			if (!success)
				{
				DSys::Logger::Error("%s: '%s'", iluErrorString( ilGetError() ), filename );
				return;
				}
			}

		glTexture = ilutGLBindTexImage();

		global::textureDatabase[filename] = glTexture;
		}

	void Texture::BuildIcon( const char * filename, Filter glFilter )
		{
		Init();

		global::database_type::iterator Iter = global::textureDatabase.find( filename );
		if( Iter != global::textureDatabase.end() )
			{ // a texture for filename already exists.
			this->owner = false;
			this->glTexture = Iter->second;
			return;
			}

			// otherwise we are the owners.
		this->path = filename;
		this->owner = true;
		
			// try to load as image.
		ILuint ilTexture;
		ilGenImages(1, &ilTexture);
		ilBindImage(ilTexture);
		bool success = ilLoadImage(filename);
		if (!success)
			{
			// load the file's icon.

			HICON hIcon = 0;
				// Get the icon index using SHGetFileInfo  
			SHFILEINFO sfi = {0};  
			SHGetFileInfo(filename, -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);  

			// Retrieve the system image list.  
			// To get the 48x48 icons, use SHIL_EXTRALARGE  
			// To get the 256x256 icons (Vista only), use SHIL_JUMBO  
			HIMAGELIST* imageList;  
			HRESULT hResult = SHGetImageList(SHIL_JUMBO, IID_IImageList, (void**)&imageList);  

			if (hResult == S_OK)
				{
					// Get the icon we need from the list. Note that the HIMAGELIST we retrieved  
					// earlier needs to be casted to the IImageList interface before use.
				hResult = ((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);  

				if (hResult == S_OK)
					{
					}
				}

			ICONINFO Info;
			GetIconInfo( hIcon, &Info );

			ilGenImages(1, &ilTexture);
			ilBindImage( ilTexture );
			bool success = ilutSetHBitmap( Info.hbmColor );
			if (!success)
				{
				DSys::Logger::Error("%s: '%s'", iluErrorString( ilGetError() ), filename );
				return;
				}
			}

		GLint maxSize = 256;
//		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

		GLint			_width	= ilGetInteger(IL_IMAGE_WIDTH);
		GLint			_height	= ilGetInteger(IL_IMAGE_HEIGHT);
		if(	(_width > maxSize) || (_height > maxSize) || !IsPowerOfTwo(_height) || !IsPowerOfTwo(_width) )
			{
			Clamp <GLint> ( 1, maxSize, _width);
			Clamp <GLint> ( 1, maxSize, _height);
			_width = ClosestPowerOfTwo(_width);
			_height= ClosestPowerOfTwo(_height);

			success = iluScale( _width, _height, ilGetInteger(IL_IMAGE_DEPTH) );
			if (!success)
				{
				DSys::Logger::Error("%s: '%s'", iluErrorString( ilGetError() ), filename );
				return;
				}
			}

		glTexture = ilutGLBindTexImage();
		this->owner = true;
		global::textureDatabase[filename] = glTexture;
		}

	void Texture::Delete()
		{
		if( this->glTexture && this->owner )
			{
//			global::database_type::iterator Iter = global::textureDatabase.find( this->path );
//			if( Iter != global::textureDatabase.end() )
//				global::textureDatabase.erase( Iter );

			glDeleteTextures(1, &this->glTexture);
			}
		}

	/*
	================== 
	ScreenShot
	================== 
	*/  
	
	void Texture::ScreenShot(){
		ilutGLScreenie();
	}



}// namespace DGL;