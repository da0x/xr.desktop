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

#ifndef ___DGL_EXTENTIONS_H
#define ___DGL_EXTENTIONS_H

/*
TODO List:
- ...
*/

#pragma once
#include "gl/glext.h"

namespace DGL {

	/*
=================================================================

  GL_EXENTIONS

=================================================================
*/

	class  Extensions {
				static bool		_GL_ARB_multitexture;
				static int		_GL_ARB_multitexture_NumOfUnits;
				static bool		_GL_EXT_texture_env_combine;
				static bool		_GL_EXT_texture_env_add;
				static bool		_GL_EXT_vertex_array;
				static bool		_GL_EXT_compiled_vertex_array;
				static bool		_GL_EXT_texture_edge_clamp;
				static bool		_GL_SGIS_texture_edge_clamp;
				static bool		_GL_ARB_texture_border_clamp;
				static bool		_GL_EXT_bgra;
				static bool		_WGL_EXT_swap_control;
				static bool		_GL_EXT_clip_volume_hint;
				static bool		_GL_NV_texgen_reflection;
				static bool		_GL_EXT_point_parameters;

			static bool useExtension( const char *name, bool available);
			static bool IsSupported(char *extension);
			static bool EnableMultitexture( void );
			static bool EnableVertexArray( void );
			static bool EnableLockArray( void );
			static bool EnableSwapControl( void );
			static bool EnablePointParameters( void );
		public:
			
			static void Init();

			static bool		IsEnabled_GL_ARB_multitexture() { return _GL_ARB_multitexture; }
			static int		Get_GL_ARB_multitexture_NumOfUnits() { return _GL_ARB_multitexture_NumOfUnits; }
			static bool		IsEnabled_GL_EXT_texture_env_combine() { return _GL_EXT_texture_env_combine; }
			static bool		IsEnabled_GL_EXT_texture_env_add() { return _GL_EXT_texture_env_add; }
			static bool		IsEnabled_GL_EXT_vertex_array() { return _GL_EXT_vertex_array; }
			static bool		IsEnabled_GL_EXT_compiled_vertex_array() { return _GL_EXT_compiled_vertex_array; }
			static bool		IsEnabled_GL_EXT_texture_edge_clamp() { return _GL_EXT_texture_edge_clamp; }
			static bool		IsEnabled_GL_SGIS_texture_edge_clamp() { return _GL_SGIS_texture_edge_clamp; }
			static bool		IsEnabled_GL_ARB_texture_border_clamp() { return _GL_ARB_texture_border_clamp; }
			static bool		IsEnabled_GL_EXT_bgra() { return _GL_EXT_bgra; }
			static bool		IsEnabled_WGL_EXT_swap_control() { return _WGL_EXT_swap_control; }
			static bool		IsEnabled_GL_EXT_clip_volume_hint() { return _GL_EXT_clip_volume_hint; }
			static bool		IsEnabled_GL_NV_texgen_reflection() { return _GL_NV_texgen_reflection; }
			static bool		IsEnabled_GL_EXT_point_parameters() { return _GL_EXT_point_parameters; }
	};
}

/*
#if !GL_ARB_multitexture ||\
	!GL_EXT_vertex_array ||\
	!GL_EXT_compiled_vertex_array ||\
	!GL_EXT_texture_edge_clamp ||\
	!WGL_EXT_swap_control
#error The glExt Library is too old to compile the code
#endif
*/



// ARB_multitexture
extern  PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
extern  PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;
extern  PFNGLMULTITEXCOORD1DARBPROC		glMultiTexCoord1dARB;
extern  PFNGLMULTITEXCOORD1DVARBPROC		glMultiTexCoord1dvARB;
extern  PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1fARB;
extern  PFNGLMULTITEXCOORD1FVARBPROC		glMultiTexCoord1fvARB;
extern  PFNGLMULTITEXCOORD1IARBPROC		glMultiTexCoord1iARB;
extern  PFNGLMULTITEXCOORD1IVARBPROC		glMultiTexCoord1ivARB;
extern  PFNGLMULTITEXCOORD1SARBPROC		glMultiTexCoord1sARB;
extern  PFNGLMULTITEXCOORD1SVARBPROC		glMultiTexCoord1svARB;
extern  PFNGLMULTITEXCOORD2DARBPROC		glMultiTexCoord2dARB;
extern  PFNGLMULTITEXCOORD2DVARBPROC		glMultiTexCoord2dvARB;
extern  PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB;
extern  PFNGLMULTITEXCOORD2FVARBPROC		glMultiTexCoord2fvARB;
extern  PFNGLMULTITEXCOORD2IARBPROC		glMultiTexCoord2iARB;
extern  PFNGLMULTITEXCOORD2IVARBPROC		glMultiTexCoord2ivARB;
extern  PFNGLMULTITEXCOORD2SARBPROC		glMultiTexCoord2sARB;
extern  PFNGLMULTITEXCOORD2SVARBPROC		glMultiTexCoord2svARB;
extern  PFNGLMULTITEXCOORD3DARBPROC		glMultiTexCoord3dARB;
extern  PFNGLMULTITEXCOORD3DVARBPROC		glMultiTexCoord3dvARB;
extern  PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB;
extern  PFNGLMULTITEXCOORD3FVARBPROC		glMultiTexCoord3fvARB;
extern  PFNGLMULTITEXCOORD3IARBPROC		glMultiTexCoord3iARB;
extern  PFNGLMULTITEXCOORD3IVARBPROC		glMultiTexCoord3ivARB;
extern  PFNGLMULTITEXCOORD3SARBPROC		glMultiTexCoord3sARB;
extern  PFNGLMULTITEXCOORD3SVARBPROC		glMultiTexCoord3svARB;
extern  PFNGLMULTITEXCOORD4DARBPROC		glMultiTexCoord4dARB;
extern  PFNGLMULTITEXCOORD4DVARBPROC		glMultiTexCoord4dvARB;
extern  PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB;
extern  PFNGLMULTITEXCOORD4FVARBPROC		glMultiTexCoord4fvARB;
extern  PFNGLMULTITEXCOORD4IARBPROC		glMultiTexCoord4iARB;
extern  PFNGLMULTITEXCOORD4IVARBPROC		glMultiTexCoord4ivARB;
extern  PFNGLMULTITEXCOORD4SARBPROC		glMultiTexCoord4sARB;
extern  PFNGLMULTITEXCOORD4SVARBPROC		glMultiTexCoord4svARB;

/* EXT_vertex_array */
extern  PFNGLARRAYELEMENTEXTPROC			glArrayElementEXT;
extern  PFNGLDRAWARRAYSEXTPROC			glDrawArraysEXT;
extern  PFNGLVERTEXPOINTEREXTPROC		glVertexPointerEXT;
extern  PFNGLNORMALPOINTEREXTPROC		glNormalPointerEXT;
extern  PFNGLCOLORPOINTEREXTPROC			glColorPointerEXT;
extern  PFNGLINDEXPOINTEREXTPROC			glIndexPointerEXT;
extern  PFNGLTEXCOORDPOINTEREXTPROC		glTexCoordPointerEXT;
extern  PFNGLEDGEFLAGPOINTEREXTPROC		glEdgeFlagPointerEXT;
extern  PFNGLGETPOINTERVEXTPROC			glGetPointervEXT;
extern  PFNGLARRAYELEMENTARRAYEXTPROC	glArrayElementArrayEXT;

// EXT_compiled_vertex_array
extern  PFNGLLOCKARRAYSEXTPROC			glLockArraysEXT;
extern  PFNGLUNLOCKARRAYSEXTPROC			glUnlockArraysEXT;

// WGL_EXT_swap_control
extern  BOOL (APIENTRY * wglSwapIntervalEXT) (int);
extern  int (APIENTRY * wglGetSwapIntervalEXT) (void);

/* GL_EXT_point_parameters */
extern  PFNGLPOINTPARAMETERFEXTPROC		glPointParameterfEXT;
extern  PFNGLPOINTPARAMETERFVEXTPROC		glPointParameterfvEXT;


// ARB_multitexture
 void dglActiveTextureARB(GLenum target);
 void dglClientActiveTextureARB(GLenum target);
 void dglMultiTexCoord1dARB(GLenum target, GLdouble s);
 void dglMultiTexCoord1dvARB(GLenum target, const GLdouble *v);
 void dglMultiTexCoord1fARB(GLenum target, GLfloat s);
 void dglMultiTexCoord1fvARB(GLenum target, const GLfloat *v);
 void dglMultiTexCoord1iARB(GLenum target, GLint s);
 void dglMultiTexCoord1ivARB(GLenum target, const GLint *v);
 void dglMultiTexCoord1sARB(GLenum target, GLshort s);
 void dglMultiTexCoord1svARB(GLenum target, const GLshort *v);
 void dglMultiTexCoord2dARB(GLenum target, GLdouble s, GLdouble t);
 void dglMultiTexCoord2dvARB(GLenum target, const GLdouble *v);
 void dglMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t);
 void dglMultiTexCoord2fvARB(GLenum target, const GLfloat *v);
 void dglMultiTexCoord2iARB(GLenum target, GLint s, GLint t);
 void dglMultiTexCoord2ivARB(GLenum target, const GLint *v);
 void dglMultiTexCoord2sARB(GLenum target, GLshort s, GLshort t);
 void dglMultiTexCoord2svARB(GLenum target, const GLshort *v);
 void dglMultiTexCoord3dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r);
 void dglMultiTexCoord3dvARB(GLenum target, const GLdouble *v);
 void dglMultiTexCoord3fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r);
 void dglMultiTexCoord3fvARB(GLenum target, const GLfloat *v);
 void dglMultiTexCoord3iARB(GLenum target, GLint s, GLint t, GLint r);
 void dglMultiTexCoord3ivARB(GLenum target, const GLint *v);
 void dglMultiTexCoord3sARB(GLenum target, GLshort s, GLshort t, GLshort r);
 void dglMultiTexCoord3svARB(GLenum target, const GLshort *v);
 void dglMultiTexCoord4dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
 void dglMultiTexCoord4dvARB(GLenum target, const GLdouble *v);
 void dglMultiTexCoord4fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
 void dglMultiTexCoord4fvARB(GLenum target, const GLfloat *v);
 void dglMultiTexCoord4iARB(GLenum target, GLint s, GLint t, GLint r, GLint q);
 void dglMultiTexCoord4ivARB(GLenum target, const GLint *v);
 void dglMultiTexCoord4sARB(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
 void dglMultiTexCoord4svARB(GLenum target, const GLshort *v);

// EXT_vertex_array
 void dglArrayElementEXT(GLint i);
 void dglDrawArraysEXT(GLenum mode, GLint first, GLsizei count);
 void dglVertexPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
 void dglNormalPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
 void dglColorPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
 void dglIndexPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
 void dglTexCoordPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
 void dglEdgeFlagPointerEXT(GLsizei stride, GLsizei count, const GLboolean *pointer);
 void dglGetPointervEXT(GLenum pname, GLvoid* *params);
 void dglArrayElementArrayEXT(GLenum mode, GLsizei count, const GLvoid* pi);

// EXT_compiled_vertex_array
 void dglLockArraysEXT(GLint first, GLsizei count);
 void dglUnlockArraysEXT(void);

// WGL_EXT_swap_control
 BOOL dwglSwapIntervalEXT(int interval);
 int dwglGetSwapIntervalEXT(void);

// EXT_point_parameters
 void dglPointParameterfEXT (GLenum pname, GLfloat param);
 void dglPointParameterfvEXT (GLenum pname, const GLfloat *params);


#endif // ___DGL_EXTENTIONS_H