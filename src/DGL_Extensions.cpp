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
- Add more exentions
*/

#include "DShared.h"
#include <windows.h>
#include <gl/gl.h>
#include "DGL_Main.h"
#include "DGL_Extensions.h"


namespace DGL {


	/* --------------------- */
	/*     Enabled flags
	/* --------------------- */

	bool	Extensions::_GL_ARB_multitexture				= false;
	int		Extensions::_GL_ARB_multitexture_NumOfUnits		= 0;
	bool	Extensions::_GL_EXT_texture_env_combine			= false;
	bool	Extensions::_GL_EXT_texture_env_add				= false;
	bool	Extensions::_GL_EXT_vertex_array				= false;
	bool	Extensions::_GL_EXT_compiled_vertex_array		= false;
	bool	Extensions::_GL_EXT_texture_edge_clamp			= false;
	bool	Extensions::_GL_SGIS_texture_edge_clamp			= false;
	bool	Extensions::_GL_ARB_texture_border_clamp		= false;
	bool	Extensions::_GL_EXT_bgra						= false;
	bool	Extensions::_WGL_EXT_swap_control				= false;
	bool	Extensions::_GL_EXT_clip_volume_hint			= false;
	bool	Extensions::_GL_NV_texgen_reflection			= false;
	bool	Extensions::_GL_EXT_point_parameters			= false;

	/* --------------------- */
	/*     gl extentions     */
	/* --------------------- */
	bool Extensions::useExtension( const char *name, bool available){
			if ( available ){
				DGL::LogPrint("using %s", name);
				return true;
			} else {
				DGL::LogPrint("Not found %s", name);
				return false;
			}
	}

	void Extensions::Init(){
			DGL::LogPrint("..........................................");
			DGL::LogPrint("..... Initializing OpenGL Extentions .....");
			DGL::LogPrint("..........................................");

			// multitexture
			_GL_ARB_multitexture			= useExtension("GL_ARB_multitexture", EnableMultitexture());
			if(_GL_ARB_multitexture){
				_GL_EXT_texture_env_combine	= useExtension("GL_EXT_texture_env_combine", IsSupported("GL_EXT_texture_env_combine"));
				if(!_GL_EXT_texture_env_combine)
					_GL_EXT_texture_env_add	= useExtension("GL_EXT_texture_env_add", IsSupported("GL_EXT_texture_env_add"));
			}

			// vertex arrays
			_GL_EXT_vertex_array			= useExtension("GL_EXT_vertex_array", EnableVertexArray());
			if(_GL_EXT_vertex_array)
			_GL_EXT_compiled_vertex_array	= useExtension("GL_EXT_compiled_vertex_array", EnableLockArray());

			// texture edge clamp
			_GL_EXT_texture_edge_clamp		= useExtension("GL_EXT_texture_edge_clamp",	IsSupported("GL_EXT_texture_edge_clamp"));
			if(!_GL_EXT_texture_edge_clamp){
				_GL_SGIS_texture_edge_clamp	= useExtension("GL_SGIS_texture_edge_clamp",	IsSupported("GL_SGIS_texture_edge_clamp"));
				if(!_GL_SGIS_texture_edge_clamp)
					_GL_ARB_texture_border_clamp	= useExtension("GL_ARB_texture_border_clamp",	IsSupported("GL_ARB_texture_border_clamp"));
			}

			// bgra
			_GL_EXT_bgra					= useExtension("GL_EXT_bgra", IsSupported("GL_EXT_bgra"));
			// swap control
			_WGL_EXT_swap_control			= useExtension("WGL_EXT_swap_control", EnableSwapControl());
			// clip volume
			_GL_EXT_clip_volume_hint		= useExtension("GL_EXT_clip_volume_hint", IsSupported("GL_EXT_clip_volume_hint"));
			// NV texture reflection
			_GL_NV_texgen_reflection		= useExtension("GL_NV_texgen_reflection", IsSupported("GL_NV_texgen_reflection"));
			// point parameters
			_GL_EXT_point_parameters		= useExtension("GL_EXT_point_parameters", EnablePointParameters());

			DGL::LogPrint("..........................................\n");
	}


	bool Extensions::IsSupported(char *extension){
		char *start;
		char *where, *terminator;

		/* Extension names should not have spaces. */
		where = (char *) strchr(extension, ' ');
		if (where || *extension == '\0'){
			DGL::LogError("Invalid Extension to look for '%s'.",extension);
			return false;
		}
		char* extensions= new char [strlen((char *)glGetString(GL_EXTENSIONS))+1];	// Allocate Memory For Our Extension String
		strcpy (extensions,(char *)glGetString(GL_EXTENSIONS));		// Grab The Extension List, Store In Text
		/* It takes a bit of care to be fool-proof about parsing the
		OpenGL extensions string. Don't be fooled by sub-strings,
		etc. */
		start = extensions;
		for (;;) {
			where = (char *) strstr((char *) start, extension);
			if (!where)
				break;
			terminator = where + strlen(extension);
			if (where == start || *(where - 1) == ' ')
				if (*terminator == ' ' || *terminator == '\0'){
					return true;
				}
				start = terminator;
		}
		//	DGL::LogError("Extension is not supported: '%s'.",extension);
		return false;
	}
	
	/* ---------------------- */
	/*   EnableMultitexture   */
	/* ---------------------- */

	bool Extensions::EnableMultitexture(){
		if ( !IsSupported( "GL_ARB_multitexture" ) ) return false;

		glActiveTextureARB			= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB	= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glClientActiveTextureARB");
		glMultiTexCoord1dARB		= (PFNGLMULTITEXCOORD1DARBPROC)		wglGetProcAddress("glMultiTexCoord1dARB");
		glMultiTexCoord1dvARB		= (PFNGLMULTITEXCOORD1DVARBPROC)	wglGetProcAddress("glMultiTexCoord1dvARB");
		glMultiTexCoord1fARB		= (PFNGLMULTITEXCOORD1FARBPROC)		wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord1fvARB		= (PFNGLMULTITEXCOORD1FVARBPROC)	wglGetProcAddress("glMultiTexCoord1fvARB");
		glMultiTexCoord1iARB		= (PFNGLMULTITEXCOORD1IARBPROC)		wglGetProcAddress("glMultiTexCoord1iARB");
		glMultiTexCoord1ivARB		= (PFNGLMULTITEXCOORD1IVARBPROC)	wglGetProcAddress("glMultiTexCoord1ivARB");
		glMultiTexCoord1sARB		= (PFNGLMULTITEXCOORD1SARBPROC)		wglGetProcAddress("glMultiTexCoord1sARB");
		glMultiTexCoord1svARB		= (PFNGLMULTITEXCOORD1SVARBPROC)	wglGetProcAddress("glMultiTexCoord1svARB");
		glMultiTexCoord2dARB		= (PFNGLMULTITEXCOORD2DARBPROC)		wglGetProcAddress("glMultiTexCoord2dARB");
		glMultiTexCoord2dvARB		= (PFNGLMULTITEXCOORD2DVARBPROC)	wglGetProcAddress("glMultiTexCoord2dvARB");
		glMultiTexCoord2fARB		= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord2fvARB		= (PFNGLMULTITEXCOORD2FVARBPROC)	wglGetProcAddress("glMultiTexCoord2fvARB");
		glMultiTexCoord2iARB		= (PFNGLMULTITEXCOORD2IARBPROC)		wglGetProcAddress("glMultiTexCoord2iARB");
		glMultiTexCoord2ivARB		= (PFNGLMULTITEXCOORD2IVARBPROC)	wglGetProcAddress("glMultiTexCoord2ivARB");
		glMultiTexCoord2sARB		= (PFNGLMULTITEXCOORD2SARBPROC)		wglGetProcAddress("glMultiTexCoord2sARB");
		glMultiTexCoord2svARB		= (PFNGLMULTITEXCOORD2SVARBPROC)	wglGetProcAddress("glMultiTexCoord2svARB");
		glMultiTexCoord3dARB		= (PFNGLMULTITEXCOORD3DARBPROC)		wglGetProcAddress("glMultiTexCoord3dARB");
		glMultiTexCoord3dvARB		= (PFNGLMULTITEXCOORD3DVARBPROC)	wglGetProcAddress("glMultiTexCoord3dvARB");
		glMultiTexCoord3fARB		= (PFNGLMULTITEXCOORD3FARBPROC)		wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord3fvARB		= (PFNGLMULTITEXCOORD3FVARBPROC)	wglGetProcAddress("glMultiTexCoord3fvARB");
		glMultiTexCoord3iARB		= (PFNGLMULTITEXCOORD3IARBPROC)		wglGetProcAddress("glMultiTexCoord3iARB");
		glMultiTexCoord3ivARB		= (PFNGLMULTITEXCOORD3IVARBPROC)	wglGetProcAddress("glMultiTexCoord3ivARB");
		glMultiTexCoord3sARB		= (PFNGLMULTITEXCOORD3SARBPROC)		wglGetProcAddress("glMultiTexCoord3sARB");
		glMultiTexCoord3svARB		= (PFNGLMULTITEXCOORD3SVARBPROC)	wglGetProcAddress("glMultiTexCoord3svARB");
		glMultiTexCoord4dARB		= (PFNGLMULTITEXCOORD4DARBPROC)		wglGetProcAddress("glMultiTexCoord4dARB");
		glMultiTexCoord4dvARB		= (PFNGLMULTITEXCOORD4DVARBPROC)	wglGetProcAddress("glMultiTexCoord4dvARB");
		glMultiTexCoord4fARB		= (PFNGLMULTITEXCOORD4FARBPROC)		wglGetProcAddress("glMultiTexCoord4fARB");
		glMultiTexCoord4fvARB		= (PFNGLMULTITEXCOORD4FVARBPROC)	wglGetProcAddress("glMultiTexCoord4fvARB");
		glMultiTexCoord4iARB		= (PFNGLMULTITEXCOORD4IARBPROC)		wglGetProcAddress("glMultiTexCoord4iARB");
		glMultiTexCoord4ivARB		= (PFNGLMULTITEXCOORD4IVARBPROC)	wglGetProcAddress("glMultiTexCoord4ivARB");
		glMultiTexCoord4sARB		= (PFNGLMULTITEXCOORD4SARBPROC)		wglGetProcAddress("glMultiTexCoord4sARB");
		glMultiTexCoord4svARB		= (PFNGLMULTITEXCOORD4SVARBPROC)	wglGetProcAddress("glMultiTexCoord4svARB");

		if(! glActiveTextureARB		||!glClientActiveTextureARB	||!glMultiTexCoord1dARB		||!glMultiTexCoord1dvARB	||!glMultiTexCoord1fARB		||!
			glMultiTexCoord1fvARB	||!glMultiTexCoord1iARB		||!glMultiTexCoord1ivARB	||!glMultiTexCoord1sARB		||!glMultiTexCoord1svARB	||!
			glMultiTexCoord2dARB	||!glMultiTexCoord2dvARB	||!glMultiTexCoord2fARB		||!glMultiTexCoord2fvARB	||!glMultiTexCoord2iARB		||!
			glMultiTexCoord2ivARB	||!glMultiTexCoord2sARB		||!glMultiTexCoord2svARB	||!glMultiTexCoord3dARB		||!glMultiTexCoord3dvARB	||!
			glMultiTexCoord3fARB	||!glMultiTexCoord3fvARB	||!glMultiTexCoord3iARB		||!glMultiTexCoord3ivARB	||!glMultiTexCoord3sARB		||!
			glMultiTexCoord3svARB	||!glMultiTexCoord4dARB		||!glMultiTexCoord4dvARB	||!glMultiTexCoord4fARB		||!glMultiTexCoord4fvARB	||!
			glMultiTexCoord4iARB	||!glMultiTexCoord4ivARB	||!glMultiTexCoord4sARB		||!glMultiTexCoord4svARB )
		{
			DGL::LogError("One or more functions of the extension GL_ARB_multitexture cannot be located!...");
			return false;
		}

		glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB,
			&_GL_ARB_multitexture_NumOfUnits );
		return true;
	}
	
	/* --------------------- */
	/*   EnableVertexArray   */
	/* --------------------- */

	bool Extensions::EnableVertexArray(){
		if ( !Extensions::IsSupported( "GL_EXT_vertex_array" ) ) return false;
		
		glArrayElementEXT		= (PFNGLARRAYELEMENTEXTPROC)		wglGetProcAddress("glArrayElementEXT");
		glDrawArraysEXT			= (PFNGLDRAWARRAYSEXTPROC)			wglGetProcAddress("glDrawArraysEXT");
		glVertexPointerEXT		= (PFNGLVERTEXPOINTEREXTPROC)		wglGetProcAddress("glVertexPointerEXT");
		glNormalPointerEXT		= (PFNGLNORMALPOINTEREXTPROC)		wglGetProcAddress("glNormalPointerEXT");
		glColorPointerEXT		= (PFNGLCOLORPOINTEREXTPROC)		wglGetProcAddress("glColorPointerEXT");
		glIndexPointerEXT		= (PFNGLINDEXPOINTEREXTPROC)		wglGetProcAddress("glIndexPointerEXT");
		glTexCoordPointerEXT	= (PFNGLTEXCOORDPOINTEREXTPROC)		wglGetProcAddress("glTexCoordPointerEXT");
		glEdgeFlagPointerEXT	= (PFNGLEDGEFLAGPOINTEREXTPROC)		wglGetProcAddress("glEdgeFlagPointerEXT");
		glGetPointervEXT		= (PFNGLGETPOINTERVEXTPROC)			wglGetProcAddress("glGetPointervEXT");
		//	glArrayElementArrayEXT	= (PFNGLARRAYELEMENTARRAYEXTPROC)	wglGetProcAddress("glArrayElementArrayEXT");
        
		if(!glArrayElementEXT	||!glDrawArraysEXT			||!glVertexPointerEXT	||!glNormalPointerEXT	||!
			glColorPointerEXT	||!glIndexPointerEXT		||!glTexCoordPointerEXT	||!glEdgeFlagPointerEXT	||!
			glGetPointervEXT	/*||!glArrayElementArrayEXT*/)
		{
			DGL::LogError("One or more functions of the extension GL_EXT_vertex_array cannot be located!...");
			return false;
		}
        
		return true;
	}

	/* ------------------- */
	/*   EnableLockArray   */
	/* ------------------- */

	bool Extensions::EnableLockArray(){
		if ( !Extensions::IsSupported( "GL_EXT_compiled_vertex_array" ) ) return false;

		glLockArraysEXT		= (PFNGLLOCKARRAYSEXTPROC)		wglGetProcAddress("glLockArraysEXT");
		glUnlockArraysEXT	= (PFNGLUNLOCKARRAYSEXTPROC)	wglGetProcAddress("glUnlockArraysEXT");

		if(glLockArraysEXT == NULL  ||  glUnlockArraysEXT == NULL)	{
			DGL::LogError("One or more functions of the extension GL_EXT_compiled_vertex_array cannot be located!...");
			return false;
		}

		return true;
	}

	/* --------------------- */
	/*   EnableVertexArray   */
	/* --------------------- */

	bool Extensions::EnableSwapControl(){
		if ( !Extensions::IsSupported( "WGL_EXT_swap_control" ) ) return false;
	
		wglSwapIntervalEXT		= 	( BOOL (__stdcall *)(int) ) wglGetProcAddress("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT	= 	( int (__stdcall *)(void) ) wglGetProcAddress("wglGetSwapIntervalEXT");

		if(wglSwapIntervalEXT == NULL  ||  wglGetSwapIntervalEXT == NULL){
			DGL::LogError("One or more functions of the extension WGL_EXT_swap_control cannot be located!...");
			return false;
		}
		return true;
	}


	/* --------------------------- */
	/*   Enable Point Parameters   */
	/* --------------------------- */

	bool Extensions::EnablePointParameters(){
		if ( !Extensions::IsSupported( "GL_EXT_point_parameters" ) ) return false;

		glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC) wglGetProcAddress("glPointParameterfEXT");
		glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC) wglGetProcAddress("glPointParameterfvEXT");

		if(glPointParameterfEXT == NULL  ||  glPointParameterfvEXT == NULL)	{
			DGL::LogError("One or more functions of the extension GL_EXT_point_parameters cannot be located!...");
			return false;
		}
		return true;
	}

}// namespace DGL;


///////////////////////////
// Functions decleration //
///////////////////////////


// ARB_multitexture
 PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB = NULL;
 PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB = NULL;
 PFNGLMULTITEXCOORD1DARBPROC			glMultiTexCoord1dARB = NULL;
 PFNGLMULTITEXCOORD1DVARBPROC		glMultiTexCoord1dvARB = NULL;
 PFNGLMULTITEXCOORD1FARBPROC			glMultiTexCoord1fARB = NULL;
 PFNGLMULTITEXCOORD1FVARBPROC		glMultiTexCoord1fvARB = NULL;
 PFNGLMULTITEXCOORD1IARBPROC			glMultiTexCoord1iARB = NULL;
 PFNGLMULTITEXCOORD1IVARBPROC		glMultiTexCoord1ivARB = NULL;
 PFNGLMULTITEXCOORD1SARBPROC			glMultiTexCoord1sARB = NULL;
 PFNGLMULTITEXCOORD1SVARBPROC		glMultiTexCoord1svARB = NULL;
 PFNGLMULTITEXCOORD2DARBPROC			glMultiTexCoord2dARB = NULL;
 PFNGLMULTITEXCOORD2DVARBPROC		glMultiTexCoord2dvARB = NULL;
 PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB = NULL;
 PFNGLMULTITEXCOORD2FVARBPROC		glMultiTexCoord2fvARB = NULL;
 PFNGLMULTITEXCOORD2IARBPROC			glMultiTexCoord2iARB = NULL;
 PFNGLMULTITEXCOORD2IVARBPROC		glMultiTexCoord2ivARB = NULL;
 PFNGLMULTITEXCOORD2SARBPROC			glMultiTexCoord2sARB = NULL;
 PFNGLMULTITEXCOORD2SVARBPROC		glMultiTexCoord2svARB = NULL;
 PFNGLMULTITEXCOORD3DARBPROC			glMultiTexCoord3dARB = NULL;
 PFNGLMULTITEXCOORD3DVARBPROC		glMultiTexCoord3dvARB = NULL;
 PFNGLMULTITEXCOORD3FARBPROC			glMultiTexCoord3fARB = NULL;
 PFNGLMULTITEXCOORD3FVARBPROC		glMultiTexCoord3fvARB = NULL;
 PFNGLMULTITEXCOORD3IARBPROC			glMultiTexCoord3iARB = NULL;
 PFNGLMULTITEXCOORD3IVARBPROC		glMultiTexCoord3ivARB = NULL;
 PFNGLMULTITEXCOORD3SARBPROC			glMultiTexCoord3sARB = NULL;
 PFNGLMULTITEXCOORD3SVARBPROC		glMultiTexCoord3svARB = NULL;
 PFNGLMULTITEXCOORD4DARBPROC			glMultiTexCoord4dARB = NULL;
 PFNGLMULTITEXCOORD4DVARBPROC		glMultiTexCoord4dvARB = NULL;
 PFNGLMULTITEXCOORD4FARBPROC			glMultiTexCoord4fARB = NULL;
 PFNGLMULTITEXCOORD4FVARBPROC		glMultiTexCoord4fvARB = NULL;
 PFNGLMULTITEXCOORD4IARBPROC			glMultiTexCoord4iARB = NULL;
 PFNGLMULTITEXCOORD4IVARBPROC		glMultiTexCoord4ivARB = NULL;
 PFNGLMULTITEXCOORD4SARBPROC			glMultiTexCoord4sARB = NULL;
 PFNGLMULTITEXCOORD4SVARBPROC		glMultiTexCoord4svARB = NULL;

/* EXT_vertex_array */
 PFNGLARRAYELEMENTEXTPROC			glArrayElementEXT = NULL;
 PFNGLDRAWARRAYSEXTPROC				glDrawArraysEXT = NULL;
 PFNGLVERTEXPOINTEREXTPROC			glVertexPointerEXT = NULL;
 PFNGLNORMALPOINTEREXTPROC			glNormalPointerEXT = NULL;
 PFNGLCOLORPOINTEREXTPROC			glColorPointerEXT = NULL;
 PFNGLINDEXPOINTEREXTPROC			glIndexPointerEXT = NULL;
 PFNGLTEXCOORDPOINTEREXTPROC			glTexCoordPointerEXT = NULL;
 PFNGLEDGEFLAGPOINTEREXTPROC			glEdgeFlagPointerEXT = NULL;
 PFNGLGETPOINTERVEXTPROC				glGetPointervEXT = NULL;
 PFNGLARRAYELEMENTARRAYEXTPROC		glArrayElementArrayEXT = NULL;

// EXT_compiled_vertex_array
 PFNGLLOCKARRAYSEXTPROC				glLockArraysEXT		= NULL;
 PFNGLUNLOCKARRAYSEXTPROC			glUnlockArraysEXT	= NULL;

// WGL_EXT_swap_control
 BOOL (APIENTRY * wglSwapIntervalEXT) (int);
 int (APIENTRY * wglGetSwapIntervalEXT) (void);

/* GL_EXT_point_parameters */
 PFNGLPOINTPARAMETERFEXTPROC			glPointParameterfEXT = NULL;
 PFNGLPOINTPARAMETERFVEXTPROC		glPointParameterfvEXT = NULL;
