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

#ifndef ___DFX_H
#define ___DFX_H

/*
TODO List:
- ...
*/


namespace DGL {
	
//public:
	/*
	=============================
	---------------------------
			DrawTools
	---------------------------
	=============================
	*/

	/*
	--------------
	Draw 2d Rect
	--------------
	*/

	static void Draw2DRect(
			int			x,
			int			y,
			int			w,
			int			h,
			GLuint		Texture,
			float		alpha=-1
	){
		MatrixOp::Ortho::Begin();
		glBindTexture(GL_TEXTURE_2D, Texture);
		Color color;
		if(alpha == -1){
			color = Color::ColorWhite();
		} else {
			color.Set(1,1,1,alpha);
		}

		color.MakeCurrent();
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2d(1,1); glVertex2f(x+w,y-h); // Top Right
			glTexCoord2d(0,1); glVertex2f(x-w,y-h); // Top Left
			glTexCoord2d(1,0); glVertex2f(x+w,y+h); // Bottom Right
			glTexCoord2d(0,0); glVertex2f(x-w,y+h); // Bottom Left
		glEnd();

		MatrixOp::Ortho::End();
	}


	static void Draw2DRect(
		int			x,
		int			y,
		int			w,
		int			h,
		GLuint		Texture,
		float		s1,
		float		t1,
		float		s2,
		float		t2,
		float		s3,
		float		t3,
		float		s4,
		float		t4
		){
		MatrixOp::Ortho::Begin();
		glBindTexture(GL_TEXTURE_2D, Texture);
		Color::ColorWhite().MakeCurrent();

		glBegin(GL_QUADS);
		glTexCoord2f(s1,t1);
		glVertex2d( x, y);
		glTexCoord2f(s2,t2);
		glVertex2d( x+w, y);
		glTexCoord2f(s3,t3);
		glVertex2d( x+w, y+h);
		glTexCoord2f(s4,t4);
		glVertex2d( x, y+h);
		glEnd();

		MatrixOp::Ortho::End();
	}

	static void Draw2DRect(
		int			x,
		int			y,
		int			w,
		int			h,
		const
		Color		color
		){
		MatrixOp::Ortho::Begin();
		glPushAttrib(GL_ENABLE_BIT);
		color.MakeCurrent();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_TEXTURE_2D);
		//
		glBegin(GL_QUADS);
		glVertex2d( x,	y);
		glVertex2d( x+w,y);
		glVertex2d( x+w,y+h);
		glVertex2d( x,	y+h);
		glEnd();
		//
		glPopAttrib();

		MatrixOp::Ortho::End();
	}


	static void Draw2DRect(
		int			x0,
		int			y0,
		int			x1,
		int			y1,
		const
		Color		color0,
		const
		Color		color1
		){
		MatrixOp::Ortho::Begin();
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			color0.MakeCurrent();
			glVertex2d( x0, y0);
			color0.MakeCurrent();
			glVertex2d( x1, y0);
			color1.MakeCurrent();
			glVertex2d( x1, y1);
			color1.MakeCurrent();
			glVertex2d( x0, y1);
		glEnd();
		glEnable(GL_TEXTURE_2D);
		MatrixOp::Ortho::End();
	}
	/*
	-----------------
    Draw 2D Line
	-----------------
	*/
	static void Draw2DLine(
		int			x0,
		int			y0,
		int			x1,
		int			y1,
		float		width,
		const
		Color		color
		){
		MatrixOp::Ortho::Begin();
		glDisable(GL_TEXTURE_2D);
		color.MakeCurrent();
		glLineWidth(width);
		glBegin(GL_LINES);
			glVertex2d( x0, y0);
			glVertex2d( x1, y1);
		glEnd();
		glEnable(GL_TEXTURE_2D);

		MatrixOp::Ortho::End();
	}

	/*
	--------------
	Draw 3d Rect
	--------------
	*/

	static void Draw3DRect(
		Vector		pos,
		float		w,
		float		h,
		GLuint		Texture
		){
		w/=2.0f;
		h/=2.0f;

		GLboolean lighting= glIsEnabled(GL_LIGHTING);
		if(lighting)
			glDisable(GL_LIGHTING);

		MatrixOp::BalloonPoint::Begin(pos);

			glBindTexture(GL_TEXTURE_2D,Texture);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			Color::ColorWhite().MakeCurrent();

			glBegin(GL_QUADS);
				glNormal3f(0.0f,0.0f,1.0f);
				glTexCoord2f(0.0f,0.0f);
				glVertex2f(-w,-h);
				glNormal3f(0.0f,0.0f,1.0f);
				glTexCoord2f(1.0f,0.0f);
				glVertex2f( w,-h);
				glNormal3f(0.0f,0.0f,1.0f);
				glTexCoord2f(1.0f,1.0f);
				glVertex2f( w, h);
				glNormal3f(0.0f,0.0f,1.0f);
				glTexCoord2f(0.0f,1.0f);
				glVertex2f(-w, h);
			glEnd();

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_BLEND);

		MatrixOp::BalloonPoint::End();


		if(lighting)
			glEnable(GL_LIGHTING);
	}
	static void Draw3DSolidRect(
				Vertex		pos,
				float		w,
				float		h,
				GLuint		Texture
		){
		w/=2.0f;
		h/=2.0f;

		GLboolean lighting= glIsEnabled(GL_LIGHTING);
		if(lighting)
			glDisable(GL_LIGHTING);

		glPushMatrix();
		pos.Translate();

		Color::ColorWhite().MakeCurrent();
		glBindTexture(GL_TEXTURE_2D,Texture);
		glBegin(GL_QUADS);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(0.0f,0.0f);
			glVertex2f(-w,-h);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(1.0f,0.0f);
			glVertex2f( w,-h);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(1.0f,1.0f);
			glVertex2f( w, h);
			glNormal3f(0.0f,0.0f,1.0f);
			glTexCoord2f(0.0f,1.0f);
			glVertex2f(-w, h);
		glEnd();

		if(lighting)
			glEnable(GL_LIGHTING);
		glPopMatrix();
	}
	static void DrawCupe(){
		glBegin(GL_QUADS);							// Start Drawing Quads
			// Front Face
			glNormal3f( 0.0f, 0.0f, 1.0f);					// normal Pointing Towards Viewer
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Point 1 (Front)
			glNormal3f( 0.0f, 0.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Point 2 (Front)
			glNormal3f( 0.0f, 0.0f, 1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Point 3 (Front)
			glNormal3f( 0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Point 4 (Front)
			// Back Face
			glNormal3f( 0.0f, 0.0f,-1.0f);					// normal Pointing Away From Viewer
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Point 1 (Back)
			glNormal3f( 0.0f, 0.0f,-1.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Point 2 (Back)
			glNormal3f( 0.0f, 0.0f,-1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Point 3 (Back)
			glNormal3f( 0.0f, 0.0f,-1.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Point 4 (Back)
			// Top Face
			glNormal3f( 0.0f, 1.0f, 0.0f);					// normal Pointing Up
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Point 1 (Top)
			glNormal3f( 0.0f, 1.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Point 2 (Top)
			glNormal3f( 0.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Point 3 (Top)
			glNormal3f( 0.0f, 1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Point 4 (Top)
			// Bottom Face
			glNormal3f( 0.0f,-1.0f, 0.0f);					// normal Pointing Down
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Point 1 (Bottom)
			glNormal3f( 0.0f,-1.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Point 2 (Bottom)
			glNormal3f( 0.0f,-1.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Point 3 (Bottom)
			glNormal3f( 0.0f,-1.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Point 4 (Bottom)

			// Right face
			glNormal3f( 1.0f, 0.0f, 0.0f);					// normal Pointing Right
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Point 1 (Right)
			glNormal3f( 1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Point 2 (Right)
			glNormal3f( 1.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Point 3 (Right)
			glNormal3f( 1.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Point 4 (Right)
			// Left Face
			glNormal3f(-1.0f, 0.0f, 0.0f);					// normal Pointing Left
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Point 1 (Left)
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Point 2 (Left)
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Point 3 (Left)
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Point 4 (Left)
		glEnd();	
	}

	/*
	--------------------
	Quadric Objects
	--------------------
	*/

	extern GLUquadricObj *quadricObj;
	void InitQuadrics(); // should be called first

	__forceinline static  void DrawCylinder(
		GLdouble            baseRadius, 
		GLdouble            topRadius, 
		GLdouble            height){
		gluCylinder(quadricObj,
			baseRadius,
			topRadius,
			height,
			32,
			32);
	}
	__forceinline static void DrawCone(
		GLdouble			baseRadius,
		GLdouble			height){
		DrawCylinder(baseRadius, 0, height);
	}
	__forceinline static void DrawDisk(
		GLdouble            innerRadius, 
		GLdouble            outerRadius){
		gluDisk(quadricObj,innerRadius,outerRadius,32,32);
	}
	__forceinline static void DrawSphere(GLdouble radius){
		gluSphere(quadricObj,radius,32,32);
	}
	__forceinline static void DrawPartialDisk(
		GLdouble            innerRadius, 
		GLdouble            outerRadius,
		GLdouble            startAngle,
		GLdouble            sweepAngle){
		gluPartialDisk(quadricObj,innerRadius,outerRadius,32,32,startAngle,sweepAngle);
	}

	

	/* 
	----------------------------
	Auxiliary Library Draw Tools
	----------------------------
	*/
	/*
	__forceinline static void DrawTorus(GLdouble pipeRadius, GLdouble ringRadius){
		auxSolidTorus(pipeRadius, ringRadius);
	}
	__forceinline static void DrawIcosahedron(GLdouble radius){
		auxSolidIcosahedron(radius);
	}
	__forceinline static void DrawOctahedron(GLdouble radius){
		auxSolidOctahedron(radius);
	}
	__forceinline static void DrawTetrahedron(GLdouble radius){
		auxSolidTetrahedron(radius);
	}
	__forceinline static void DrawDodecahedron(GLdouble radius){
		auxSolidDodecahedron(radius);
	}
	__forceinline static void DrawTeapot(GLdouble radius){
		auxSolidTeapot(radius);
	}
	*/
	/*
	===========================================================

	Underwater Perspective

	===========================================================
	*/

	class UnderwaterPerspective{
		float		ctime;
		float		sequenceTime;
		int			width;	// in pixels
		int			height; // in pixels
		float		aspect;
		float		fovdepth;	// in angles
	public:
		UnderwaterPerspective():
			ctime(0),
			sequenceTime(750),
			width(60),
			fovdepth(2.0f)
		{	}

		void Update(float msec){
			ctime += msec;
		}

		void ModifyPerspective(int screenWidth, int screenHeight, int fovY, int r_min, int r_max){
			aspect = (screenWidth + width*(float)sin(ctime/sequenceTime)) / screenHeight;

			float fovy = fovY + fovdepth*(float)cos(ctime/sequenceTime);
			
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			gluPerspective (
				fovy,
				aspect,
				r_min,
				r_max);

			glMatrixMode(GL_MODELVIEW);
//			glLoadIdentity();
		}

		void ResetPerspective(int screenWidth, int screenHeight, int fovY, int r_min, int r_max){
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			
			gluPerspective (
				fovY,
				screenWidth/screenHeight,
				r_min,
				r_max);

			glMatrixMode(GL_MODELVIEW);
//			glLoadIdentity();
		}
	};


	class UserMessage {
	public:
		enum EStyle {
			STYLE_FADE,
			STYLE_POPUP
		};
	private:
		enum EStatus{
			WAITING,
			STARTING,
			SHOWING,
			ENDING
		};

		char		buffer[1024];
		Font		font;
		float		showTime;
		float		eventTime;
		EStatus		status;
		float		alpha;
		EStyle		style;
		float		size;
		Color		color;

	public:
		UserMessage() : showTime(0),eventTime(0),status(WAITING),alpha(0) { }
		void ShowMessage( const char *fmt, float Size, EStyle Style, float time, Color color, ...){
			static va_list ap;
			va_start(ap, color);
			vsprintf(buffer, fmt, ap);
			va_end(ap);
			showTime = time*1000.0f;
			eventTime = 0;
			style = Style;
			size = Size;
			status = STARTING;
			this->color = color;
		}
		void SetFont(Font &newFont){
			this->font = newFont;
			this->font.SetStyle(Font::STYLE_CENTER|Font::STYLE_SHADOWS);
		}
		void Update(float msec){
			switch(status){
				case STARTING:
					{
						eventTime += msec;
						if(eventTime>=750.0f){
							eventTime = 0;
							alpha = 1;
							status = SHOWING;
						} else {
							alpha = Interpolate<float>(0,1,eventTime / 750.0f);
						}
					}
					break;
				case SHOWING:
					{
						showTime -= msec;
						if(showTime<=0){
							status = ENDING;
						}
					}
					break;
				case ENDING:
					{
						eventTime += msec;
						if(eventTime>= 1000.0f){
							status = WAITING;
						} else {
							alpha = Interpolate<float>(1,0,eventTime / 1000.0f);
						}
					}
					break;
				case WAITING:
					break;
			}
		}
		void Render(){
			float _size;
			if(status != WAITING){
				color.a = alpha;
				switch (style) {
					case STYLE_FADE:
						_size = size;
						break;
					case STYLE_POPUP:
						_size = size*alpha;
						break;
				}
				this->font.SetSize(_size);
				this->font.SetColor(color);
				this->font.Print(310,350,buffer);
			}
		}
		void StopMessage(){
			status = WAITING;
		}
	};
		
}//namespace DGL {


#endif // ___DFX_H

