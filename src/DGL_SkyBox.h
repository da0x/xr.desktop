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

#ifndef ___DSKYBOX_H
#define ___DSKYBOX_H

/*
TODO List:
- ...
*/

namespace DGL {
	class SkyBox {
		private:
			float SkyboxWidth,SkyboxHeight,SkyboxLength;
			DGL::Texture	top,bottom,left,right,front,back;
		public:

			void InitSkybox(char *name){
				LogPrint(va("SkyBox '%s' Initializations:",name));


				DGL::Texture::Filter skyfilter(Texture::Filter::FilterName::TRILINEAR, true);

				top		.Build(va("textures/skies/%sTop.jpg", name), skyfilter);
				bottom	.Build(va("textures/skies/%sBottom.jpg", name), skyfilter);
				left	.Build(va("textures/skies/%sLeft.jpg", name), skyfilter);
				right	.Build(va("textures/skies/%sRight.jpg", name), skyfilter);
				front	.Build(va("textures/skies/%sFront.jpg", name), skyfilter);
				back	.Build(va("textures/skies/%sBack.jpg", name), skyfilter);

				SkyboxWidth		= 12.0f;
				SkyboxLength	= 12.0f;
				SkyboxHeight	= 8.0f;
			}

			void Render(){
				DMacro_TraceEnter(SkyBox::Render);
				float x,y,z;

				glPushAttrib(GL_ENABLE_BIT|GL_LIGHTING_BIT);
				glDisable(GL_LIGHTING);
				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);

				x = - this->SkyboxWidth  / 2.0f;
				y = - this->SkyboxHeight / 2.0f;
				z = - this->SkyboxLength / 2.0f;

				Color::ColorWhite().MakeCurrent();
				
				glPushMatrix();
				float m[4][4];
				glGetFloatv(GL_MODELVIEW_MATRIX, (float *)m);
				m[3][0] = m[3][1] = m[3][2] = 0.0f;
				glLoadMatrixf( (float *)m);
				
				
				
				glBindTexture( GL_TEXTURE_2D, this->back);

				glBegin(GL_QUADS);		
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight, z);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z); 
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, this->front);

				glBegin(GL_QUADS);	
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + SkyboxLength);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight, z + SkyboxLength);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z + SkyboxLength); 
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z + SkyboxLength);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, this->top);

				glBegin(GL_QUADS);		
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight,	z);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + SkyboxHeight,	z + SkyboxLength);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z + SkyboxLength); 
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, this->bottom);

				glBegin(GL_QUADS);		
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,					y,	z);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,					y,	z + SkyboxLength);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth,	y,	z + SkyboxLength); 
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth,	y,	z);
				glEnd();
				
				glBindTexture( GL_TEXTURE_2D, this->left);

				glBegin(GL_QUADS);		
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + SkyboxLength);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight,	z + SkyboxLength); 
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight,	z);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, this->right);
				
				glBegin(GL_QUADS);		
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z + SkyboxLength);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight,	z + SkyboxLength); 
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight,	z);
				glEnd();

				glPopAttrib();
				glPopMatrix();

				DMacro_TraceLeave();
			}

			void Destroy()
			{
				this->top.Delete();
				this->bottom.Delete();
				this->left.Delete();
				this->right.Delete();
				this->front.Delete();
				this->back.Delete();
			}
	};



	class SkyBox_HalfDom {
		protected:
			float SkyboxWidth,SkyboxHeight,SkyboxLength;
			DGL::Texture	top,left,right,front,back;
		public:

			void InitSkybox(char *name){
				LogPrint(va("SkyBox '%s' Initializations:",name));


				
				DGL::Texture::Filter skyfilter(Texture::Filter::FilterName::TRILINEAR, true);

				top		.Build(va("textures/skies/%sTop.jpg", name), skyfilter);
				left	.Build(va("textures/skies/%sLeft.jpg", name), skyfilter);
				right	.Build(va("textures/skies/%sRight.jpg", name), skyfilter);
				front	.Build(va("textures/skies/%sFront.jpg", name), skyfilter);
				back	.Build(va("textures/skies/%sBack.jpg", name), skyfilter);


				SkyboxWidth		= 12.0f;
				SkyboxLength	= 12.0f;
				SkyboxHeight	= 6.0f;
			}
			void Render(){
				DMacro_TraceEnter(SkyBox_HalfDom::Render);
				float x,y,z;

				glPushAttrib(GL_ENABLE_BIT|GL_LIGHTING_BIT);
				glDisable(GL_LIGHTING);
				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);

				x = - SkyboxWidth  / 2.0f;
				y = 0.0f;
				z = - SkyboxLength / 2.0f;

				Color::ColorWhite().MakeCurrent();
				
				glPushMatrix();
				float m[4][4];
				glGetFloatv(GL_MODELVIEW_MATRIX, (float *)m);
				m[3][0] = m[3][1] = m[3][2] = 0.0f;
				glLoadMatrixf( (float *)m);
				
				
				glBindTexture( GL_TEXTURE_2D, back);

				glBegin(GL_QUADS);		
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight, z);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z); 
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, front);

				glBegin(GL_QUADS);	
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + SkyboxLength);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight, z + SkyboxLength);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z + SkyboxLength); 
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z + SkyboxLength);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, top);

				glBegin(GL_QUADS);		
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight,	z);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + SkyboxHeight,	z + SkyboxLength);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z + SkyboxLength); 
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z);
				glEnd();
				
				glBindTexture( GL_TEXTURE_2D, left);

				glBegin(GL_QUADS);		
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + SkyboxLength);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight,	z + SkyboxLength); 
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight,	z);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, right);
				
				glBegin(GL_QUADS);		
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z + SkyboxLength);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight,	z + SkyboxLength); 
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight,	z);
				glEnd();

				glPopAttrib();
				glPopMatrix();

				DMacro_TraceLeave();
			}

			void Destroy()
			{
				this->top.Delete();
				this->left.Delete();
				this->right.Delete();
				this->front.Delete();
				this->back.Delete();
			}
	};



	////////////////////////////////
	// static skybox
	class SkyBox_static {
		protected:
			float			SkyboxWidth,SkyboxHeight,SkyboxLength;
			DGL::Texture	top,left,right,front,back;
		public:

			void InitSkybox(char *name, float width, float length, float height){
				LogPrint(va("SkyBox '%s' Initializations:",name));


				
				DGL::Texture::Filter skyfilter(Texture::Filter::FilterName::TRILINEAR, true);

				top		.Build(va("textures/skies/%sTop.jpg", name), skyfilter);
				left	.Build(va("textures/skies/%sLeft.jpg", name), skyfilter);
				right	.Build(va("textures/skies/%sRight.jpg", name), skyfilter);
				front	.Build(va("textures/skies/%sFront.jpg", name), skyfilter);
				back	.Build(va("textures/skies/%sBack.jpg", name), skyfilter);


				SkyboxWidth		= width;
				SkyboxLength	= length;
				SkyboxHeight	= height;
			}
			void Render(){
				DMacro_TraceEnter(SkyBox_static::Render);
				float x,y,z;

				glPushAttrib(GL_ENABLE_BIT|GL_LIGHTING_BIT);
				glDisable(GL_LIGHTING);
				glDisable(GL_CULL_FACE);
//				glDisable(GL_DEPTH_TEST);

				x = - SkyboxWidth  / 2.0f;
				y = 0.0f;
				z = - SkyboxLength / 2.0f;

				Color::ColorWhite().MakeCurrent();
				
				glBindTexture( GL_TEXTURE_2D, back);

				glBegin(GL_QUADS);		
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight, z);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z); 
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, front);

				glBegin(GL_QUADS);	
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + SkyboxLength);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight, z + SkyboxLength);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z + SkyboxLength); 
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z + SkyboxLength);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, top);

				glBegin(GL_QUADS);		
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight,	z);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + SkyboxHeight,	z + SkyboxLength);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z + SkyboxLength); 
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight, z);
				glEnd();
				
				glBindTexture( GL_TEXTURE_2D, left);

				glBegin(GL_QUADS);		
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + SkyboxLength);
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight,	z + SkyboxLength); 
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + SkyboxHeight,	z);
				glEnd();

				glBindTexture( GL_TEXTURE_2D, right);
				
				glBegin(GL_QUADS);		
				glTexCoord2f(0.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(x + SkyboxWidth, y,			z + SkyboxLength);
				glTexCoord2f(1.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight,	z + SkyboxLength); 
				glTexCoord2f(0.0f, 1.0f); glVertex3f(x + SkyboxWidth, y + SkyboxHeight,	z);
				glEnd();

				glPopAttrib();
				glPopMatrix();

				DMacro_TraceLeave();
			}

			void Destroy()
			{
				this->top.Delete();
				this->left.Delete();
				this->right.Delete();
				this->front.Delete();
				this->back.Delete();
			}
	};
}//namespace DGL

#endif // ___DSKYBOX_H