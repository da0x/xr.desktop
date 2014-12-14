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
#include "DGL_Texture.h"
#include "DGL_LoadingScreen.h"
#include "DGL_Font.h"
#include "DGL_Color.h"
#include "DGL_Vertex.h"
#include "DGL_Fx.h"
#include "DSys_Window.h" // DSys::Window::SwapBuffers()
#include "DSys.h"



#define PROGRESS_STARTX			(DSys::sv_iScreenWidth/2.0f)-100
#define PROGRESS_ENDX			(DSys::sv_iScreenWidth/2.0f)+100
#define PROGRESS_STARTY			(DSys::sv_iScreenHeight/1.5f)
#define PROGRESS_ENDY			(DSys::sv_iScreenHeight/1.5f)+10
#define PROGRESS_COLOR0			Color::ColorBlack()
#define PROGRESS_COLOR1			Color::ColorGreen()
#define PROGRESS_BACK_COLOR0	Color::ColorBlack()
#define PROGRESS_BACK_COLOR1	Color::ColorGray()
#define PROGRESS_COLOR_DIM		Color::ColorDim()
#define PROGRESS_BORDER_COLOR	Color::ColorGold()

namespace DGL {
	
	void LoadingScreen::Init()
	{
		this->percent = 0;
		this->tex.Build("textures/Loading.tga");
		this->font.Create("font");
		this->font.SetSize(0.75f);
		this->font.SetColor(DGL::Color::ColorGray());
		this->timer.Init();
	}

	void LoadingScreen::SetStage(int _percent, char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		vsprintf(this->text, fmt, ap);
		va_end(ap);
#ifndef _DEBUG
		while( this->percent++ < _percent )
		{
			this->PopFrame();
//			::Sleep(30);
		}
#else // _DEBUG
		this->percent = _percent;
		this->PopFrame();
#endif
	}

    void LoadingScreen::PopFrame()
	{
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);

		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);	


		//
		// Text/info
		//
		this->font.Print(10, 20, this->text);

		//
		// Progress bar
		//

		// the progress background
		Draw2DRect(
			PROGRESS_STARTX,
			PROGRESS_STARTY,
			PROGRESS_ENDX,
			PROGRESS_ENDY,
			PROGRESS_BACK_COLOR0,
			PROGRESS_BACK_COLOR1);
		// this is the total progress bar
		Draw2DRect(
			PROGRESS_STARTX,
			PROGRESS_STARTY,
			Interpolate<int>(
			PROGRESS_STARTX,
			PROGRESS_ENDX,
			(float)(percent/100.0f)),
			PROGRESS_ENDY,
			PROGRESS_COLOR0,
			PROGRESS_COLOR1);

		// darw borders for the progress bar		
		MatrixOp::Ortho::Begin();
			glDisable(GL_TEXTURE_2D);
			glLineWidth(0.3f);
			glBegin(GL_LINE_STRIP);
				PROGRESS_BORDER_COLOR.MakeCurrent();
				glVertex2d( PROGRESS_STARTX,	PROGRESS_STARTY);
				PROGRESS_BORDER_COLOR.MakeCurrent();
				glVertex2d( PROGRESS_ENDX,		PROGRESS_STARTY);
				PROGRESS_BORDER_COLOR.MakeCurrent();
				glVertex2d( PROGRESS_ENDX,		PROGRESS_ENDY);
				PROGRESS_BORDER_COLOR.MakeCurrent();
				glVertex2d( PROGRESS_STARTX,	PROGRESS_ENDY);
				PROGRESS_BORDER_COLOR.MakeCurrent();
				glVertex2d( PROGRESS_STARTX,	PROGRESS_STARTY);
			glEnd();
			glLineWidth(1.0f);
			glEnable(GL_TEXTURE_2D);
		MatrixOp::Ortho::End();

		//
		// Loading image
		//

		Draw2DRect( (DSys::sv_iScreenWidth/2.0f), (DSys::sv_iScreenWidth/3.0f),128,64,tex);

		glPopAttrib();

		DSys::Window::SwapGLBuffers();
	}

	LoadingScreen::~LoadingScreen()
	{
		this->tex.Delete();
	}
}//namespace DGL