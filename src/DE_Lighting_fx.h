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

#ifndef ___DLIGHING_FX_H
#define ___DLIGHING_FX_H


#include <shlobj.h>  
#include <shlguid.h>  
#include <shellapi.h> 
#include <Shobjidl.h>
#include <list>
#include "DGL_Train.h"
#include "DGL_Font.h"

#include "core_shell.hpp"
#include "core_shell_contextmenu.hpp"

namespace DGL {

	DSys::Var_float		r_iconSizeRatio		("r_iconSizeRatio", "0.8");
	DSys::Var_float		r_lightDepthRatio	("r_lightDepthRatio", "0.6");
	DSys::Var_int		r_effect			("r_effect", "0");
	DSys::Var_float		r_testVal			("r_testVal", "0.0");
	DSys::Var_float		r_freeCam			("r_freeCam", "0");


	namespace effect { enum effect_s
		{
		none,
		fire,
		stars
		}; /* effect_s */ } /* effect */

	namespace select { enum selectible
		{
		wall_bottom,
		wall_front,
		wall_left,
		wall_right,
		wall_back,
		icon,
		}; /* selectible */ } /* select */

	class Icon
		{
		private: 
				 static Texture iconFace;
				 static Texture iconSide;
				 static GLuint	FaceList;
				 static GLuint	SideList;

				 Texture icon;
				 
				 Vector position;
				 Vector orientation;
				 std::string filename;
				 std::string path;

				 DSys::Physics::dynamic::item body;

				 float scale;
				 static const float width;
				 static const float height;
				 static const float length;
		public:
			static void GlobalInit()
				{				
				iconFace.Build( "textures/windows/icon_face.png" );
				iconSide.Build( "textures/windows/icon_side.png" );

				FaceList = glGenLists(1);
				glNewList(FaceList, GL_COMPILE);
					glColor4fv(Color::ColorWhite());

					// up
					glBegin(GL_QUADS);
					glNormal3fv( Vector::ynorm() );
					glTexCoord2f( 0, 0);
					glVertex3f( -width , height, length);
					glNormal3fv( Vector::ynorm() );
					glTexCoord2f( 1, 0);
					glVertex3f( width , height, length);
					glNormal3fv( Vector::ynorm() );
					glTexCoord2f( 1, 1);
					glVertex3f( width , height, -length);
					glNormal3fv( Vector::ynorm() );
					glTexCoord2f( 0, 1);
					glVertex3f( -width , height, -length);
					glEnd();
					
					// down
					glBegin(GL_QUADS);
					glNormal3fv( -Vector::ynorm() );
					glTexCoord2f( 0, 0);
					glVertex3f( -width , -height, -length);
					glNormal3fv( -Vector::ynorm() );
					glTexCoord2f( 1, 0);
					glVertex3f( width , -height, -length);
					glNormal3fv( -Vector::ynorm() );
					glTexCoord2f( 1, 1);
					glVertex3f( width , -height, length);
					glNormal3fv( -Vector::ynorm() );
					glTexCoord2f( 0, 1);
					glVertex3f( -width , -height, length);
					glEnd();
					
				glEndList();

				SideList = glGenLists(1);
				glNewList(SideList, GL_COMPILE);
					glColor4fv(Color::ColorWhite());
					// front
					glBegin(GL_QUADS);
					glNormal3fv( Vector::znorm() );
					glTexCoord2f( 0, 0);
					glVertex3f( -width , -height, length);
					glNormal3fv( Vector::znorm() );
					glTexCoord2f( 1, 0);
					glVertex3f( width , -height, length);
					glNormal3fv( Vector::znorm() );
					glTexCoord2f( 1, 1);
					glVertex3f( width , height, length);
					glNormal3fv( Vector::znorm() );
					glTexCoord2f( 0, 1);
					glVertex3f( -width , height, length);
					glEnd();

					// back
					glBegin(GL_QUADS);
					glNormal3fv( -Vector::znorm() );
					glTexCoord2f( 0, 0);
					glVertex3f( width , -height, -length);
					glNormal3fv( -Vector::znorm() );
					glTexCoord2f( 1, 0);
					glVertex3f( -width , -height, -length);
					glNormal3fv( -Vector::znorm() );
					glTexCoord2f( 1, 1);
					glVertex3f( -width , height, -length);
					glNormal3fv( -Vector::znorm() );
					glTexCoord2f( 0, 1);
					glVertex3f( width , height, -length);
					glEnd();

					// left
					glBegin(GL_QUADS);
					glNormal3fv( -Vector::xnorm() );
					glTexCoord2f( 0, 0);
					glVertex3f( -width , -height, -length);
					glNormal3fv( -Vector::xnorm() );
					glTexCoord2f( 1, 0);
					glVertex3f( -width , -height, length);
					glNormal3fv( -Vector::xnorm() );
					glTexCoord2f( 1, 1);
					glVertex3f( -width , height, length);
					glNormal3fv( -Vector::xnorm() );
					glTexCoord2f( 0, 1);
					glVertex3f( -width , height, -length);
					glEnd();
					
					// right
					glBegin(GL_QUADS);
					glNormal3fv( Vector::xnorm() );
					glTexCoord2f( 0, 0);
					glVertex3f( width , -height, length);
					glNormal3fv( Vector::xnorm() );
					glTexCoord2f( 1, 0);
					glVertex3f( width , -height, -length);
					glNormal3fv( Vector::xnorm() );
					glTexCoord2f( 1, 1);
					glVertex3f( width , height, -length);
					glNormal3fv( Vector::xnorm() );
					glTexCoord2f( 0, 1);
					glVertex3f( width , height, length);
					glEnd();
				glEndList();
				}

			void Init( std::string path, std::string filename )
					{
					this->icon.BuildIcon( path.c_str() );
					this->path = path;
					this->filename = filename;
					this->scale = 1;
					/*
					if( this->filename[ this->filename.size() - 4 ] == '.' )
						{
						this->filename.erase( this->filename.size() - 4,  this->filename.size() );
						}
					*/
					Vector pos(0,3,0),hungVec;
					bool hung;

					std::ifstream in(std::string("user\\")+this->filename+".dat",std::ios::binary);
					if(!in)
						{
						hung = false;
						pos = Vector(0,3,0);
						}
					else
						{
						in >> pos.x >> pos.y >> pos.z;
						in >> hung;
						in >> hungVec.x >> hungVec.y >> hungVec.z;
						}

					if( !this->filename.compare( "blank-world-map.jpg" ) ||
						!this->filename.compare( "firefox_logo.png" ) ||
						!this->filename.compare( "firefox.jpg" ) )
						{ // create a sphere type icon.
						this->body.init( pos, (width)*scale );
						}
					else
						{
						this->body.init( pos, Vector(width*scale,height*scale,length*scale) );
						}

					if(hung)
						this->body.hang(hungVec);

					}

				void Resize( float Delta )
					{
					scale += Delta*0.001f;
					scale = Clamp<float>( 0.5f, 2.0f, scale );
					this->body.resize( Vector(width*scale,height*scale,length*scale) );
					}

				void Drag( Vector Mouse3D )
					{
					this->body.drag( Mouse3D );
					}

				void DragOff()
					{
					this->body.drag_off();
					}

				void Hang( Vector Location )
					{
					this->body.hang( Location );
					}

				void Render() const 
					{
					float Mat[16];
					this->body.matrix( Mat );
					glPushMatrix();
					glMultMatrixf( Mat );

					switch( this->body.actor_type() )
						{
						case DSys::Physics::dynamic::item::type::box:
							glScalef( scale, scale, scale );
							glBindTexture(GL_TEXTURE_2D, iconFace);
							glCallList(FaceList);

							glBindTexture(GL_TEXTURE_2D, iconSide);
							glCallList(SideList);

							// icon
							glPushAttrib( GL_LIGHTING_BIT );
							glDisable( GL_LIGHTING );
							glEnable( GL_BLEND );
							glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
							glBindTexture(GL_TEXTURE_2D, this->icon);
							
							glColor4f(1.0f, 1.0f, 1.0f, 1.0f );
							
							// icon down
							glRotatef( 180, 0,1,0 );
							glBegin(GL_TRIANGLE_STRIP);
							glTexCoord2f( 0, 0); glNormal3fv( -Vector::ynorm() );
							glVertex3f( width*r_iconSizeRatio , -height-0.01f, length*r_iconSizeRatio);
							glTexCoord2f( 1, 0); glNormal3fv( -Vector::ynorm() );
							glVertex3f( -width*r_iconSizeRatio , -height-0.01f, length*r_iconSizeRatio);
							glTexCoord2f( 0, 1); glNormal3fv( -Vector::ynorm() );
							glVertex3f( width*r_iconSizeRatio , -height-0.01f, -length*r_iconSizeRatio);
							glTexCoord2f( 1, 1); glNormal3fv( -Vector::ynorm() );
							glVertex3f( -width*r_iconSizeRatio , -height-0.01f, -length*r_iconSizeRatio);
							glEnd();

							// icon up						
							glRotatef(180,0,0,1);
							
							glBegin(GL_TRIANGLE_STRIP);
							glTexCoord2f( 0, 0); glNormal3fv( -Vector::ynorm() );
							glVertex3f( width*r_iconSizeRatio , -height-0.01f, length*r_iconSizeRatio);
							glTexCoord2f( 1, 0); glNormal3fv( -Vector::ynorm() );
							glVertex3f( -width*r_iconSizeRatio , -height-0.01f, length*r_iconSizeRatio);
							glTexCoord2f( 0, 1); glNormal3fv( -Vector::ynorm() );
							glVertex3f( width*r_iconSizeRatio , -height-0.01f, -length*r_iconSizeRatio);
							glTexCoord2f( 1, 1); glNormal3fv( -Vector::ynorm() );
							glVertex3f( -width*r_iconSizeRatio , -height-0.01f, -length*r_iconSizeRatio);
							glEnd();

							glDisable( GL_BLEND );
							glPopAttrib();
							break;

						case DSys::Physics::dynamic::item::type::sphere:
							glBindTexture(GL_TEXTURE_2D, this->icon);
							DGL::DrawSphere(width*scale);
							break;
						}
					glPopMatrix();					
					}

				void SetPose( Vector Position, Vector Orientation )
					{
					this->position = Position;
					this->orientation = Orientation;
					}

				void identity()
					{
					this->body.identity();
					}

				void Save()
					{
					std::ofstream out(std::string("user\\")+this->filename+".dat",std::ios::binary);

					out << this->body.position().x << " " << this->body.position().y << " " << this->body.position().z << std::endl;
					out << this->body.hung() << std::endl;
					out << this->body.hung_vector().x << " " << this->body.hung_vector().y << " " << this->body.hung_vector().z << std::endl;
					}

				void Destroy()
					{
					this->Save();

					this->iconFace.Delete();
					this->iconSide.Delete();
					this->icon.Delete();
					}

				std::string Filename() const { return this->filename; }
				std::string Path() const { return this->path; }
				Vector Position() const { return this->body.position(); }
		};
		const float Icon::width = 0.8f;
		const float Icon::height = 0.05f;
		const float Icon::length = 0.8f;
		Texture Icon::iconFace;
		Texture Icon::iconSide;
		GLuint	Icon::FaceList;
		GLuint	Icon::SideList;

	class Room {
		private:
			Texture	floor;
			Texture front;
			Texture back;
			Texture left;
			Texture right;

			GLuint	list;

		public:
			float ROOM_LEN;
			float ROOM_HIT;
			float ROOM_STP;
			float ROOM_TEX_REPEAT;

			void Init(){
				floor.Build("textures/windows/floor.jpg");
				front.Build("textures/windows/front.jpg");
				back.Build("textures/windows/back.jpg");
				left.Build("textures/windows/left.jpg");
				right.Build("textures/windows/right.jpg");

				ROOM_LEN = 24;
				ROOM_HIT = 20;
				ROOM_STP = 1;
				ROOM_TEX_REPEAT = 1;

					// up down
				DSys::Physics::CreatePlane( Vector(0.0f,-1.0f,0.0f), -ROOM_HIT/2 );
				DSys::Physics::CreatePlane( Vector(0.0f,1.0f,0.0f), -ROOM_HIT/2 );
					// front back
				DSys::Physics::CreatePlane( Vector(0.0f,0.0f,1.0f), -ROOM_HIT/2 );
				DSys::Physics::CreatePlane( Vector(0.0f,0.0f,-1.0f), -ROOM_HIT/2 );
					// left right
				DSys::Physics::CreatePlane( Vector(1.0f,0.0f,0.0f), -ROOM_LEN/2 );
				DSys::Physics::CreatePlane( Vector(-1.0f,0.0f,0.0f), -ROOM_LEN/2 );

				list = glGenLists(1);
				glNewList(list, GL_COMPILE);
					glColor4fv(Color::ColorWhite());
					glMaterialfv(GL_FRONT, GL_AMBIENT, Color(0.6f, 0.6f, 0.6f, 1.0f ));
					glMaterialfv(GL_FRONT, GL_DIFFUSE, Color(0.9f, 0.9f, 0.9f, 1.0f ));
					glMaterialfv(GL_FRONT, GL_SPECULAR, Color(0.6f, 0.6f, 0.6f, 1.0f ));
					glMaterialf(GL_FRONT, GL_SHININESS, 100.0f );

					{ // upper and lower surfaces
						glPushMatrix();
						glBindTexture( GL_TEXTURE_2D, this->floor );
						glLoadName( select::wall_bottom );
						glTranslatef(-ROOM_LEN/2.0f, 0.0f, -ROOM_HIT/2.0f);
						for(float k =0; k< ROOM_HIT; k+=ROOM_STP){
							for(float i =0; i< ROOM_LEN; i+=ROOM_STP){
								glBegin(GL_QUADS); // lower
									glNormal3f(0.0f, 1.0f, 0.0f);
									glTexCoord2f(1-(i*ROOM_TEX_REPEAT/(float)ROOM_LEN) , k*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, -ROOM_HIT/2.0f, k);
									glTexCoord2f(1-(i*ROOM_TEX_REPEAT/(float)ROOM_LEN) , (k+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, -ROOM_HIT/2.0f, k+ROOM_STP);
									glTexCoord2f(1-(i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, -ROOM_HIT/2.0f, k+ ROOM_STP);
									glTexCoord2f(1-(i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, -ROOM_HIT/2.0f, k);
								glEnd();
								/*
								glBegin(GL_QUADS); // upper
									glNormal3f(0.0f,-1.0f, 0.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, ROOM_HIT/2.0f, k);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, ROOM_HIT/2.0f, k);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, ROOM_HIT/2.0f, k+ ROOM_STP);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, ROOM_HIT/2.0f, k+ROOM_STP);
								glEnd();
								*/
							}
						}
						glPopMatrix();
					}
					{ // fornt and back surfaces
						glPushMatrix();
						glTranslatef(-ROOM_LEN/2.0f, -ROOM_HIT/2.0f, 0.0f);
						glBindTexture(GL_TEXTURE_2D,  this->back );
						glLoadName( select::wall_back );
						for (float j=0; j< ROOM_HIT; j+= ROOM_STP){
							for (float i=0; i< ROOM_LEN; i+=ROOM_STP){
								glBegin(GL_QUADS); // front
									glNormal3f(0.0f, 0.0f, 1.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j, -ROOM_HIT/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);;
									glVertex3f( i+ROOM_STP, j, -ROOM_HIT/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j+ROOM_STP, -ROOM_HIT/2.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j+ROOM_STP, -ROOM_HIT/2.0f);
								glEnd();
								}
							}								

						glBindTexture( GL_TEXTURE_2D, this->front );
						glLoadName( select::wall_front );
						for (float j=0; j< ROOM_HIT; j+= ROOM_STP){
							for (float i=0; i< ROOM_LEN; i+=ROOM_STP){

								glBegin(GL_QUADS); // back
									glNormal3f(0.0f, 0.0f, -1.0f);
									glTexCoord2f(1-(i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j, ROOM_HIT/2.0f);
									glTexCoord2f(1-(i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j+ROOM_STP, ROOM_HIT/2.0f);
									glTexCoord2f(1-(i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j+ROOM_STP, ROOM_HIT/2.0f);
									glTexCoord2f(1-(i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j, ROOM_HIT/2.0f);
								glEnd();
							}
						}
						glPopMatrix();
					}
					{ // left and right surfaces
						glPushMatrix();
						glRotatef(90, 0.0f, 1.0f, 0.0f);
						glTranslatef(-ROOM_HIT/2.0f, -ROOM_HIT/2.0f, 0.0f);

						glBindTexture( GL_TEXTURE_2D, this->right );
						glLoadName( select::wall_right );
						for (float j=0; j< ROOM_HIT; j+= ROOM_STP){
							for (float i=0; i< ROOM_HIT; i+=ROOM_STP){
								glBegin(GL_QUADS); // left
									glNormal3f(0.0f, 0.0f, 1.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_HIT , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j, -ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);;
									glVertex3f( i+ROOM_STP, j, -ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j+ROOM_STP, -ROOM_LEN/2.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_HIT , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j+ROOM_STP, -ROOM_LEN/2.0f);
								glEnd();
								}
							}
						
						glBindTexture( GL_TEXTURE_2D, this->left );
						glLoadName( select::wall_left );
						for (float j=0; j< ROOM_HIT; j+= ROOM_STP){
							for (float i=0; i< ROOM_HIT; i+=ROOM_STP){
								glBegin(GL_QUADS); // right
									glNormal3f(0.0f, 0.0f, -1.0f);
									glTexCoord2f(1-(i)*ROOM_TEX_REPEAT/(float)ROOM_HIT , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j, ROOM_LEN/2.0f);
									glTexCoord2f(1-(i)*ROOM_TEX_REPEAT/(float)ROOM_HIT , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j+ROOM_STP, ROOM_LEN/2.0f);
									glTexCoord2f(1-(i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j+ROOM_STP, ROOM_LEN/2.0f);
									glTexCoord2f(1-(i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j, ROOM_LEN/2.0f);
								glEnd();
							}
						}
						glPopMatrix();
					}
				glEndList();
			}

			void Render(){
                glCallList(list);
				/*
				glBindTexture(GL_TEXTURE_2D, this->cromEnv);
				glPushAttrib( GL_ENABLE_BIT);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_BLEND);
				glBlendFunc( GL_DST_COLOR, GL_ONE);
				glCallList(list);
				glPopAttrib();*/
			}

			void Destroy(){
				this->floor.Delete();
				this->left.Delete();
				this->right.Delete();
				this->front.Delete();
				this->back.Delete();
				glDeleteLists(list, 1);
			}

	};


	class Light_t
		{
			GLuint	glLight;

			union
				{	
				struct
					{
					Vector		m_vLightPosition;
					GLfloat		m_fIsDirectional;
					};
				GLfloat			m_fvLightPosition[4];
				};

			GLfloat				m_fLightIntensity;

		public:
			Light_t( GLuint glLight )
				{
				this->glLight = glLight;
				}

			void setup()
				{
				// setup light
				this->m_fIsDirectional = 1.0f;
				glLightfv(this->glLight, GL_AMBIENT, Color(0.6f, 0.5f, 0.5f, 1.0f));
				glLightfv(this->glLight, GL_DIFFUSE, Color(1.0f, 1.0f, 1.0f, 1.0f));
				glLightfv(this->glLight, GL_SPECULAR, Color(0.80f, 0.8f, 0.80f, 5.0f));
				glLightf(this->glLight, GL_CONSTANT_ATTENUATION, 0.1f);
				glLightf(this->glLight, GL_LINEAR_ATTENUATION, 0.2f);
				glLightf(this->glLight, GL_QUADRATIC_ATTENUATION, 0.005f);
				glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Color(0.0f,0.0f,0.0f,1.0f));
				glEnable(this->glLight);
				}

			void use( Vector Location )
				{
				this->m_vLightPosition = Location;
				
				// setup the lights
				glLightfv(this->glLight, GL_POSITION, m_fvLightPosition);
				}
		};

	class Lighting_fx :
		public Demo::Scene {
		private:
		/*
			union {	
				struct {
					
						Vector	m_vLightPosition;
					GLfloat		m_fIsDirectional;
				};
				GLfloat			m_fvLightPosition[4];
			};
			GLfloat				m_fLightIntensity;
		*/
			Room								room;
			typedef std::vector<Icon>			list_type;
			list_type							icons;
			std::vector<list_type::size_type>	selected;
			Camera								camera;
			FireEngine							fireEffect;
			FadeScreen							fadein;
			std::auto_ptr<Train>				train;
			std::auto_ptr<Light_t>				light;
			GLFT_Font							font;

		public:
			void Init(){
				Icon::GlobalInit();
				room.Init();
				font.open("arial.ttf",16);

				light.reset( new Light_t(GL_LIGHT0) );
				light->setup();
				
				// cull back face
				glCullFace(GL_BACK);
				glEnable( GL_CULL_FACE );

				this->fireEffect.Init(
					Vector(-2.5,-4,0),
					Vector(0,5,0),
					Vector(0,0,0),
					DGL::Color(1.0f,1.0f,1.0f,1.0f),
					DGL::Color::ColorRed(),
					0.25f,
					2,
					0);

//				this->ballTexture.Build( "textures/beachball.jpg" );
//				this->light.reset( new Light_t( GL_LIGHT1 ) );
//				this->ball.init( Vector(0,0,0), 1 );
//				this->light->setup();
//				this->quad.LoadModelData( "models/quad.ms3d");
				LoadIcons();

				this->fadein.Init(Color(0.0f,0.0f,0.0f,1.0f),Color(0.0f,0.0f,0.0f,0.0f), 3000);
				
				this->camera.Set( this->camera.POSITION, Vector(0.0f, -4.456252f , -9.260684f) );
				this->camera.Set( this->camera.DIRECTION, Vector(0.0f, -0.525082f , 0.972132f) );

				Vector Position,Direction;				
				Position.Set(0.0f, 3.520488f , -3.668031f);
				Direction.Set(0.0f, -0.966629f , 0.356128f);
				this->GoTo( Position, Direction );
			}

			void LoadIcons()
				{
				
				   WIN32_FIND_DATA ffd;
				   LARGE_INTEGER filesize;
				   TCHAR szDir[MAX_PATH];
				   size_t length_of_arg;
				   HANDLE hFind = INVALID_HANDLE_VALUE;
				   DWORD dwError=0;

				   // Find the first file in the directory.

				   DSys::Logger::Print("Listing Desktop Items");

				   CHAR desk[1024];
				   LPITEMIDLIST deskItemId = new ITEMIDLIST;

				   SHGetSpecialFolderLocation( DSys::Window::GetHWND(), CSIDL_DESKTOPDIRECTORY, &deskItemId );
					SHGetPathFromIDList( deskItemId, desk );

				   std::string DesktopFolder = desk;
				   DesktopFolder += "\\";
				   hFind = FindFirstFile( (DesktopFolder+"*").c_str(), &ffd);

				   FindNextFile(hFind, &ffd); // skip the .. folder
				   while (FindNextFile(hFind, &ffd) != 0)
				   	{
				      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				      {
					  DSys::Logger::Print( "  %s   <DIR>", ffd.cFileName);
				      }
				      else
				      {
				         filesize.LowPart = ffd.nFileSizeLow;
				         filesize.HighPart = ffd.nFileSizeHigh;
				        DSys::Logger::Print( "  %s   %ld bytes", ffd.cFileName, filesize.QuadPart);
				      }
					  
					this->icons.push_back( Icon() );
					this->icons.back().Init( DesktopFolder+ffd.cFileName, ffd.cFileName );
				   }
				   
				   FindClose(hFind);
				}


			void Update(float msec){
				switch( r_effect )
					{
					case effect::fire:
						this->fireEffect.Update(msec);
						break;
					}

				if( this->train.get() )
					{
					Vector Position, Direction;
					this->train.get()->Update( msec, Position, Direction );
					this->camera.Set( this->camera.POSITION, Position );
					this->camera.Set( this->camera.DIRECTION, Direction );

					if( this->train.get()->Done() )
						this->train.reset();
					}

				this->fadein.Update(msec);

				if( DSys::Input::KeyChar(KP_P) )
					{
					DSys::Logger::Print("P Pressed");
					for( list_type::iterator Iter = this->icons.begin(); Iter != this->icons.end(); ++Iter )
						{
						Iter->identity();
						}
					}

				// get current frustum for model clipping
//				DGL::Model::_FrustumUpdate();
				}

			POINT mouse;	
			GLint viewport[4];
			GLdouble modelview[16];
			GLdouble projection[16];
			
			Vector Mouse3D_light;
			Vector Mouse3D_box;			// box surface 3d position.
			Vector Mouse3D_real;		// real world mouse position.
			float Mat[16];
			float dz;

			mutable GLfloat winX;
			mutable GLfloat winY;
			mutable GLfloat winZ;
			
			mutable GLdouble posX;
			mutable GLdouble posY;
			mutable GLdouble posZ;

			void Render(){
				glClear(GL_DEPTH_BUFFER_BIT/*|GL_COLOR_BUFFER_BIT*/);
				if( !r_freeCam )
					this->camera.Setup();

				// render the scene
				room.Render();

				mouse.x = DSys::Input::MouseX();
				mouse.y = DSys::Input::MouseY();

					// Fixme: Do once per actual matrix.
				glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
				glGetDoublev( GL_PROJECTION_MATRIX, projection );
				glGetIntegerv( GL_VIEWPORT, viewport );

				winX = (float)mouse.x;
				winY = (float)viewport[3] - (float)mouse.y;

					// read pixel z
				glReadPixels( (int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

					// get light location
				gluUnProject( winX, winY, winZ-r_lightDepthRatio/10.0f, modelview, projection, viewport, &posX, &posY, &posZ);
				Mouse3D_light.Set(posX, posY,posZ);
				
					// get box mouse location.
				gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
				Mouse3D_box.Set(posX, posY,posZ);
				
					// render icons
				for( list_type::iterator Iter = this->icons.begin(); Iter != this->icons.end(); ++Iter )
					{
					Iter->Render();
					}

					// update pixel z
				glReadPixels( (int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

					// get real mouse location.
				gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
				Mouse3D_real.Set(posX, posY,posZ);

				
					// update the light
				this->light->use( Mouse3D_light );

				switch( r_effect )
					{
					case effect::fire:
						this->fireEffect.UpdatePosition( Mouse3D_light );
						break;
					}

				static int selected = -1;

				
				if( DSys::Input::MouseDown( 0 ) )
					{
					if( selected < 0 )
						{ // if this is the first frame, do select.
						selected = this->select(mouse);
						}

					if( selected >= select::icon )
						{
						this->icons[selected-select::icon].Drag( Mouse3D_real );
						}
					}
				else
					{
					if( selected >= 0 )
						{
						if( selected >= select::icon )
							{
							Icon& SelectedIcon = this->icons[selected-select::icon];
							SelectedIcon.DragOff();

								// hang the item where it was released.
							if( Mouse3D_box.y > 1 - this->room.ROOM_HIT/2 )
								this->icons[selected-select::icon].Hang( Mouse3D_box );
							}

						selected = -1;
						}
					}


				if( DSys::Input::DoubleClick() )
					{
					int selected = this->select(mouse);
					
					Vector Position;
					Vector Direction;

					switch( selected )
						{
						case select::wall_bottom:
							Position.Set(0.0f, 3.520488f , -3.668031f);
							Direction.Set(0.0f, -0.966629f , 0.356128f);

							this->GoTo( Position, Direction );
							break;
						case select::wall_front:
							Position.Set(0.0f, -4.456252f , -9.260684f);
							Direction.Set(0.0f, -0.525082f , 0.972132f);
							
							this->GoTo( Position, Direction );
							break;
						case select::wall_left:
							Position.Set(-7.832385f , -3.82581f , 0.0f);
							Direction.Set(0.977891f , -0.5206595f , 0.0f);
							
							this->GoTo( Position, Direction );
							break;
						case select::wall_right:
							Position.Set(7.832385f , -3.82581f , 0.0f);
							Direction.Set(-0.977891f , -0.506595f , 0.0f);
							
							this->GoTo( Position, Direction );
							break;
						case select::wall_back:
							Position.Set(0.0f, -4.456252f , 9.260684f);
							Direction.Set(0.0f, -0.525082f , -0.972132f);
							
							this->GoTo( Position, Direction );
							break;
						default:
							if( selected >= select::icon )
								{
								xr::core::shell::open( this->icons[selected-select::icon].Path() );
								}
							break;
						}
					}

					// get the selected icon.
				int selected_icon = this->select(mouse);

				dz = DSys::Input::MouseZDelta();
				if( dz != 0 )
					{
					if( selected_icon >= select::icon )
						{ // if wheel moved over an item, resize.
						this->icons[selected_icon-select::icon].Resize( dz );
						}
					}

				if( DSys::Input::MouseChar( 1 ) )
					{ // right click
					if( selected_icon >= select::icon )
						{
						xr::core::shell::contextmenu( this->icons[selected_icon-select::icon].Path(), DSys::Window::GetHWND(), std::make_pair( mouse.x, mouse.y ) );
						}
					}


				// fire :)
				
				switch( r_effect )
					{
					case effect::fire:
						this->fireEffect.Render();
						break;
					}


				DGL::MatrixOp::Ortho::Begin();
				list_type::iterator Iter;

				unsigned int stringwidth;
				unsigned int stringheight = font.getHeight();

				GLdouble		objx, objy, objz;
				Vector 			Obj3D;
				Vector 			Obj2D;
				float			DistanceSQ;
				const float 	DistanceSQC = 30;
				float 			Scale;

				for( Iter = this->icons.begin(); Iter != this->icons.end(); ++Iter )
					{
					Obj3D = Iter->Position();
					DistanceSQ = Obj3D.distancesquared( Mouse3D_box );
					if( DistanceSQ < DistanceSQC )
						{ // draw if mouse is nearby.
						gluProject( Obj3D.x, Obj3D.y, Obj3D.z, this->modelview, this->projection, this->viewport, &objx, &objy, &objz );
						stringwidth = this->font.calcStringWidth(Iter->Filename().c_str());


						Obj2D.x = objx;
						Obj2D.y = viewport[3] - objy + 50;

						Scale = 1-(DistanceSQ / DistanceSQC);

						Obj2D.x -= (stringwidth/2.0f)*Scale;
						Obj2D.y -= (stringheight/2.0f)*Scale;

//						DGL::Draw2DRect( Obj2D.x-5, Obj2D.y-3, stringwidth*Scale+10, stringheight*Scale+6, DGL::Color( 0.0f,0.0f,0.0f,0.3f) );
						DGL::Color::ColorBlack().MakeCurrent();
						this->font.drawText(
							Obj2D.x+2,
							Obj2D.y+2,
							Iter->Filename(),
							Scale);
						DGL::Color::ColorWhite().MakeCurrent();
						this->font.drawText(
							Obj2D.x,
							Obj2D.y,
							Iter->Filename(),
							Scale);
						}
					}

					// make sure the item currently under the cursor pops up clearly.
				if( selected_icon >= select::icon )
					{
					Obj3D = this->icons[selected_icon-select::icon].Position();
					std::string filename = this->icons[selected_icon-select::icon].Filename();

					gluProject( Obj3D.x, Obj3D.y, Obj3D.z, this->modelview, this->projection, this->viewport, &objx, &objy, &objz );
					stringwidth = this->font.calcStringWidth(filename.c_str());

					Obj2D.x = objx;
					Obj2D.y = viewport[3] - objy + 50;

					DistanceSQ = Obj3D.distancesquared( Mouse3D_box );
					Scale = 1-(DistanceSQ / DistanceSQC);

					Obj2D.x -= (stringwidth/2.0f)*Scale;
					Obj2D.y -= (stringheight/2.0f)*Scale;

					DGL::Draw2DRect( Obj2D.x-5, Obj2D.y-3, stringwidth*Scale+10, stringheight*Scale+6, DGL::Color( 0.0f,0.0f,0.0f,0.5f) );
//					DGL::Color::ColorBlack().MakeCurrent();
//					this->font.drawText(
//						Obj2D.x+2,
//						Obj2D.y+2,
//						filename,
//						Scale);
					DGL::Color::ColorWhite().MakeCurrent();
					this->font.drawText(
						Obj2D.x,
						Obj2D.y,
						filename,
						Scale);
					}

				DGL::MatrixOp::Ortho::End();

//				this->ballEngineRight.Render();
//				this->fadein.Render();
			}


			int select( POINT mouse )
				{ // perform icon selection
				
				// find out which item we wanna drag.
				GLuint	buffer[1024];							// Set Up A Selection Buffer
				GLint	hits;								// The Number Of Objects That We Selected		
	
				// The Size Of The Viewport. [0] Is <x>, [1] Is <y>, [2] Is <length>, [3] Is <width>
				GLint	viewport[4];

				// This Sets The Array <viewport> To The Size And Location Of The Screen Relative To The Window
				glGetIntegerv(GL_VIEWPORT, viewport);
				glSelectBuffer(1024, buffer);						// Tell OpenGL To Use Our Array For Selection		
				
				// Puts OpenGL In Selection Mode. Nothing Will Be Drawn. Object ID's and Extents Are Stored In The Buffer.
				(void) glRenderMode(GL_SELECT);

				glInitNames();								// Initializes The Name Stack
				glPushName(0);								// Push 0 (At Least One Entry) Onto The Stack		
				
				glMatrixMode(GL_PROJECTION);				// Selects The Projection Matrix
				glPushMatrix();								// Push The Projection Matrix
				glLoadIdentity();							// Resets The Matrix

				// This Creates A Matrix That Will Zoom Up To A Small Portion Of The Screen, Where The Mouse Is.
				gluPickMatrix((GLdouble) mouse.x, (GLdouble) (viewport[3]-mouse.y), 1.0f, 1.0f, viewport);		
	
				// Apply The Perspective Matrix
				gluPerspective( gl_fov, DSys::sv_iScreenWidth/DSys::sv_iScreenHeight, gl_near, gl_far);
				glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
				
				this->room.Render();
				for( int i =0; i<this->icons.size(); i++ )
					{
					glLoadName( select::icon+i );
					this->icons[i].Render();
					}

				glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
				glPopMatrix();										// Pop The Projection Matrix
				glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
				hits=glRenderMode(GL_RENDER);						// Switch To Render Mode, Find Out How Many		

				if (hits > 0)								// If There Were More Than 0 Hits
					{
					int	choose = buffer[3];					// Make Our Selection The First Object
					int	depth = buffer[1];					// Store How Far Away It Is		

					for (int loop = 1; loop < hits; loop++)				// Loop Through All The Detected Hits
						{
							// If This Object Is Closer To Us Than The One We Have Selected
							if (buffer[loop*4+1] < GLuint(depth))
							{
								choose = buffer[loop*4+3];			// Select The Closer Object
								depth = buffer[loop*4+1];			// Store How Far Away It Is
							}       
						}		

					return choose;
					}

				return -1;
				}

			void GoTo( Vector &Pos, Vector &Dir )
				{
				this->train.reset( new DGL::Train );

				Vector CamPos, CamDir;
				this->camera.Get( this->camera.POSITION, CamPos );
				this->camera.Get( this->camera.DIRECTION, CamDir );
				this->train.get()->PushNode( CamPos, CamDir );
				this->train.get()->PushNode( Pos, Dir );
				}

			void Destroy(){
				room.Destroy();
				this->fireEffect.Destroy();
				this->fadein.Destroy();

				// TODO: serialize here.
				
				for( list_type::iterator Iter = this->icons.begin(); Iter != this->icons.end(); ++Iter )
					{
					Iter->Destroy();
					}
			}

			void Restart()
				{
				this->fadein.Restart();
				}
	};
} // namespace DGL




#endif // ___DLIGHING_FX_H