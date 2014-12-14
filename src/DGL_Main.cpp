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

#include "DGL_Main.h"
#include "DGL_Ball.h"
#include "DSys.h"



/*
----------------
 dglGetFPS
----------------
*/

namespace DGL {

	 int GetFPS(float msec){
		static	const int	fpsFrames	= 4;	// flickering fix
		static	float		time;
		static	float		oldtime;
		static	float		fpsTime[fpsFrames];
				float		total;
		static	int			fps;
		static	int			index;
				int			i;

		time+= msec;
		fpsTime[ index ]= time - oldtime;
		oldtime= time;

		if( index >= fpsFrames ){
			index = total= 0;
			for(i=0; i< fpsFrames; i++)
				total+= fpsTime[i];
			if(!total)
				total= 1;
			fps= fpsFrames / total*1000;
		} else
			index ++;
		return fps;
	}

	
	namespace MatrixOp {
		/* ===================== */
		/*    Ortho    */
		/* ===================== */

		float Ortho::orthoMatrix[16]={
			0.003125f,	0.000000f,	0.000000f,	0.000000f,
			0.000000f,	0.004167f,	0.000000f,	0.000000f,
			0.000000f,	0.000000f,	-1.000000f,	0.000000f,
			-1.000000f,	-1.000000f,	0.000000f,	1.000000f
		};

		void Ortho::Begin(){
#ifdef _OLD_ORTHO_
			glMatrixMode( GL_PROJECTION );
			glPushMatrix();
			glLoadMatrixf(orthoMatrix);

			glMatrixMode( GL_MODELVIEW );
			glPushMatrix();
			glLoadIdentity();
            
			glPushAttrib(GL_ENABLE_BIT);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#else
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0, DSys::sv_iScreenWidth, DSys::sv_iScreenHeight, 0, -1.0, 1.0);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_DEPTH_TEST);
			glDisable( GL_LIGHTING);
			glDisable( GL_CULL_FACE);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
#endif
		}

		void Ortho::End(){
			glMatrixMode( GL_PROJECTION );	glPopMatrix();
			glMatrixMode( GL_MODELVIEW );	glPopMatrix();
			glPopAttrib();
		}

		/* ===================== */
		/*    BalloonPoint       */
		/* ===================== */
		void BalloonPoint::Begin(Vector v){
			float matrix[16];
			glPushMatrix();
			glTranslatef(v.x_(),v.y_(),v.z_());
			glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
			#define _M(row,col)  matrix[col*4+row]
				_M(0, 0) = 1.0f;
				_M(0, 1) = 0.0f;
				_M(0, 2) = 0.0f;
				_M(1, 0) = 0.0f;
				_M(1, 1) = 1.0f;
				_M(1, 2) = 0.0f;
				_M(2, 0) = 0.0f;
				_M(2, 1) = 0.0f;
				_M(2, 2) = 1.0f;
				_M(3, 0) = v.x_();
				_M(3, 1) = v.y_();
				_M(3, 2) = v.z_();
			#undef _M			
			glLoadMatrixf(matrix);
		}
		void BalloonPoint::End(){
			glPopMatrix();
		}
	}



	void LogPrint(const char *fmt,...){
		va_list ap;
		char text[1024];
		va_start(ap,fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		DSys::Logger::Print(text);
	}

	void LogError(const char *fmt,...){
		va_list ap;
		char text[1024];
		va_start(ap,fmt);
		vsprintf(text, fmt, ap);
		va_end(ap);

		DSys::Logger::Error(text);
	}


#pragma message("Wipe this shit out of here")

	//
	// DFX.H
	//

	// Quadratics
	GLUquadricObj *quadricObj;		// Quadric Object
	void InitQuadrics(){
		quadricObj = gluNewQuadric();
		gluQuadricNormals(quadricObj, GLU_SMOOTH);
		gluQuadricTexture(quadricObj, GL_TRUE);
	}


	//
	// Ball.h
	//
	DGL::Ball	*Ball::list = NULL;


}// namespace DGL;