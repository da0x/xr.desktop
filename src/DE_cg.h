#ifndef ___DE_CG_H
#define ___DE_CG_H

#include <cg\cg.h>									// Cg Header
#include <cg\cggl.h>								// Cg OpenGL Specific Header

#pragma comment( lib, "cg.lib" )					// Search For Cg.lib While Linking
#pragma comment( lib, "cggl.lib" )					// Search For CgGL.lib While Linking

namespace cg {

	const static	vbuffer_size	= 64;							// Defines The vbuffer_size Of The X/Z Axis Of The Mesh
	bool			cg_enable = true;						// Toggle Cg Program On / Off, Space Pressed?
	GLfloat			mesh[vbuffer_size][vbuffer_size][3];					// Our Static Mesh
	GLfloat			wave_movement = 0.0f;					// Our Variable To Move The Waves Across The Mesh
	CGcontext		cgContext;								// A Context To Hold Our Cg Program(s)
	CGprogram		cgProgram;								// Our Cg Vertex Program
	CGprofile		cgVertexProfile;						// The Profile To Use For Our Vertex Shader
	CGparameter		position, color, modelViewMatrix, wave;	// The Parameters Needed For Our Shader

	class Room {
		private:
			DGL::Texture	texture;
			DGL::Texture cromEnv;
			GLuint	list;

		public:
			const static ROOM_LEN = 20;
			const static ROOM_HIT = 10;
			const static ROOM_STP = 1;
			const static ROOM_TEX_REPEAT = 4;

			void Init(){
				texture.Build("textures/atech1_d.jpg");
				cromEnv.Build("textures/chrome_env.jpg");

				list = glGenLists(1);
				glNewList(list, GL_COMPILE);
					glColor4fv(DGL::Color::ColorWhite());
					glMaterialfv(GL_FRONT, GL_AMBIENT, DGL::Color(0.6f, 0.6f, 0.6f, 1.0f ));
					glMaterialfv(GL_FRONT, GL_DIFFUSE, DGL::Color(0.8f, 0.9f, 0.9f, 1.0f ));
					glMaterialfv(GL_FRONT, GL_SPECULAR, DGL::Color(0.6f, 0.6f, 0.6f, 1.0f ));
					glMaterialf(GL_FRONT, GL_SHININESS, 10.0f );

					{ // upper and lower surfaces
						glPushMatrix();
						glTranslatef(-ROOM_LEN/2.0f, 0.0f, -ROOM_LEN/2.0f);
						for(int k =0; k< ROOM_LEN; k+=ROOM_STP){
							for(int i =0; i< ROOM_LEN; i+=ROOM_STP){
								glBegin(GL_QUADS); // lower
									glNormal3f(0.0f, 1.0f, 0.0f);
									glTexCoord2f(i*ROOM_TEX_REPEAT/(float)ROOM_LEN , k*ROOM_TEX_REPEAT/(float)ROOM_LEN);
									glVertex3f( i, -ROOM_HIT/2.0f, k);
									glTexCoord2f(i*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN);
									glVertex3f( i, -ROOM_HIT/2.0f, k+ROOM_STP);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN);
									glVertex3f( i+ROOM_STP, -ROOM_HIT/2.0f, k+ ROOM_STP);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k)*ROOM_TEX_REPEAT/(float)ROOM_LEN);
									glVertex3f( i+ROOM_STP, -ROOM_HIT/2.0f, k);
								glEnd();
								glBegin(GL_QUADS); // upper
									glNormal3f(0.0f,-1.0f, 0.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k)*ROOM_TEX_REPEAT/(float)ROOM_LEN);
									glVertex3f( i, ROOM_HIT/2.0f, k);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k)*ROOM_TEX_REPEAT/(float)ROOM_LEN);
									glVertex3f( i+ROOM_STP, ROOM_HIT/2.0f, k);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN);
									glVertex3f( i+ROOM_STP, ROOM_HIT/2.0f, k+ ROOM_STP);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (k+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN);
									glVertex3f( i, ROOM_HIT/2.0f, k+ROOM_STP);
								glEnd();
							}
						}
						glPopMatrix();
					}
					{ // fornt and back surfaces
						glPushMatrix();
						glTranslatef(-ROOM_LEN/2.0f, -ROOM_HIT/2.0f, 0.0f);
						for (int j=0; j< ROOM_HIT; j+= ROOM_STP){
							for (int i=0; i< ROOM_LEN; i+=ROOM_STP){
								glBegin(GL_QUADS); // front
									glNormal3f(0.0f, 0.0f, 1.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j, -ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);;
									glVertex3f( i+ROOM_STP, j, -ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j+ROOM_STP, -ROOM_LEN/2.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j+ROOM_STP, -ROOM_LEN/2.0f);
								glEnd();
								glBegin(GL_QUADS); // back
									glNormal3f(0.0f, 0.0f, -1.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j, ROOM_LEN/2.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j+ROOM_STP, ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j+ROOM_STP, ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j, ROOM_LEN/2.0f);
								glEnd();
							}
						}
						glPopMatrix();
					}
					{ // left and right surfaces
						glPushMatrix();
						glRotatef(90, 0.0f, 1.0f, 0.0f);
						glTranslatef(-ROOM_LEN/2.0f, -ROOM_HIT/2.0f, 0.0f);
						for (int j=0; j< ROOM_HIT; j+= ROOM_STP){
							for (int i=0; i< ROOM_LEN; i+=ROOM_STP){
								glBegin(GL_QUADS); // left
									glNormal3f(0.0f, 0.0f, 1.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j, -ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);;
									glVertex3f( i+ROOM_STP, j, -ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j+ROOM_STP, -ROOM_LEN/2.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j+ROOM_STP, -ROOM_LEN/2.0f);
								glEnd();
								glBegin(GL_QUADS); // right
									glNormal3f(0.0f, 0.0f, -1.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j, ROOM_LEN/2.0f);
									glTexCoord2f((i)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i, j+ROOM_STP, ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j+ROOM_STP, ROOM_LEN/2.0f);
									glTexCoord2f((i+ROOM_STP)*ROOM_TEX_REPEAT/(float)ROOM_LEN , (j)*ROOM_TEX_REPEAT/(float)ROOM_HIT);
									glVertex3f( i+ROOM_STP, j, ROOM_LEN/2.0f);
								glEnd();
							}
						}
						glPopMatrix();
					}
				glEndList();
			}

			void Render(){
				glBindTexture(GL_TEXTURE_2D, texture);
                glCallList(list);

				glBindTexture(GL_TEXTURE_2D, this->cromEnv);
				glPushAttrib( GL_ENABLE_BIT);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_BLEND);
				glBlendFunc( GL_DST_COLOR, GL_ONE);
				glCallList(list);
				glPopAttrib();
			}

			void Destroy(){
				texture.Delete();
				cromEnv.Delete();
				glDeleteLists(list, 1);
			}

	};

	class cgDemo :
		public Demo::Scene {
		private:

			union {	
				struct {
					
						Vector	m_vLightPosition;
					GLfloat		m_fIsDirectional;
				};
				GLfloat			m_fvLightPosition[4];
			};
			GLfloat				m_fLightIntensity;
			float				m_fSeconds;

			Room				room;
			DGL::ModelMilkshape		quad;
//			DGL::Camera				camera;
			DGL::FireEngine			fireEngine;

			bool					cg_loaded;
		public:
			void Init(){
				room.Init();
				m_fSeconds = 0;
				// setup light0
				m_fIsDirectional = 1.0f;
				glLightfv(GL_LIGHT0, GL_AMBIENT, DGL::Color(0.5f, 0.3f, 0.1f, 1.0f));
				glLightfv(GL_LIGHT0, GL_DIFFUSE, DGL::Color(0.8f, 0.7f, 0.5f, 1.0f));
				glLightfv(GL_LIGHT0, GL_SPECULAR, DGL::Color(1.0f, 1.0f, 1.0f, 1.0f));
				glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.01f);
				glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);
				glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0005f);
				glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, DGL::Color(0.0f,0.0f,0.0f,1.0f));
				glEnable(GL_LIGHT0);

				this->fireEngine.Init(
					Vector(0,0,0),
					Vector(0,5,0),
					Vector(0,0,0),
					DGL::Color(1.0f,1.0f,1.0f,1.0f),
					DGL::Color::ColorRed(),
					0.25f,
					2,
					0);

				this->quad.LoadModelData( "models/quad.ms3d");


				// vertex program initializing...
				// Create Our Mesh
				for (int x = 0; x < vbuffer_size; x++)
					{
					for (int z = 0; z < vbuffer_size; z++)
						{
						mesh[x][z][0] = (float) (vbuffer_size / 2) - x; // We Want To Center Our Mesh Around The Origin
						mesh[x][z][1] = 0.0f; // Set The Y Values For All Points To 0
						mesh[x][z][2] = (float) (vbuffer_size / 2) - z; // We Want To Center Our Mesh Around The Origin
						}
					}

				this->cg_loaded = false;
				// Setup Cg
				cgContext = cgCreateContext();							// Create A New Context For Our Cg Program(s)
				// Validate Our Context Generation Was Successful
				if (cgContext == NULL)
					{
					MessageBox(NULL, "Failed To Create Cg Context", "Error", MB_OK);
					return; // We Cannot Continue
					}

				cgVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);				// Get The Latest GL Vertex Profile
				// Validate Our Profile Determination Was Successful
				if (cgVertexProfile == CG_PROFILE_UNKNOWN)
					{
					MessageBox(NULL, "Invalid profile type", "Error", MB_OK);
					return; // We Cannot Continue
					}

				cgGLSetOptimalOptions(cgVertexProfile); // Set The Current Profile

				// Load And Compile The Vertex Shader From File
				cgProgram = cgCreateProgramFromFile(cgContext, CG_SOURCE, "shaders/wave.cg", cgVertexProfile, "main", 0);
				// Validate Success
				if (cgProgram == NULL)
					{
					// We Need To Determine What Went Wrong
					CGerror Error = cgGetError();

					// Show A Message Box Explaining What Went Wrong
					MessageBox(NULL, cgGetErrorString(Error), "Error", MB_OK);
					return; // We Cannot Continue
					}

				// Load The Program
				cgGLLoadProgram(cgProgram);

				// Get Handles To Each Of Our Parameters So That
				// We Can Change Them At Will Within Our Code
				position	= cgGetNamedParameter(cgProgram, "IN.position");
				color		= cgGetNamedParameter(cgProgram, "IN.color");
				wave		= cgGetNamedParameter(cgProgram, "IN.wave");
				modelViewMatrix	= cgGetNamedParameter(cgProgram, "ModelViewProj");

				this->cg_loaded = true;
				}

			void Update(float msec){
				this->fireEngine.Update(msec);

				m_fSeconds += msec / 1000.0f;
				Vector enginePos(
							(float)sin(m_fSeconds)*room.ROOM_LEN*0.25f,
							(float)sin(m_fSeconds*0.26f)*room.ROOM_HIT*0.25f-2.0f,
							(float)cos(m_fSeconds*0.6f)*room.ROOM_LEN*0.25f);
				m_vLightPosition.Set(enginePos);
				this->fireEngine.UpdatePosition(enginePos);
				this->quad.Update(msec);

				// get current frustum for model clipping
				DGL::Model::_FrustumUpdate();

				// update for cg
				if (DSys::Input::KeyDown(VK_HOME) )
					{
					cg_enable=!cg_enable;
					}
			}

			void Render(){
				glClearColor(0,0,0,1);
				glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

				// setup the lights
				glLightfv(GL_LIGHT0, GL_POSITION, m_fvLightPosition);

				// render the scene
//				room.Render();

				// render the inner model
				glPushMatrix();
				glScalef(0.1f,0.1f,0.1f);
				glRotatef(m_fSeconds*100, 0, 1, 0);
				glTranslatef(0, (float)sin(m_fSeconds)*2, 0);
				this->quad.Render();
				glPopMatrix();
				
				// fire :)
				this->fireEngine.Render();

				// sup?
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);					// Draw Our Mesh In Wireframe Mode
				// Set The Modelview Matrix Of Our Shader To Our OpenGL Modelview Matrix
				glTranslatef(0,-5,0);
				cgGLSetStateMatrixParameter(modelViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

				if (this->cg_loaded && cg_enable)
					{
					cgGLEnableProfile(cgVertexProfile);					// Enable Our Vertex Shader Profile
					// Bind Our Vertex Program To The Current State
					cgGLBindProgram(cgProgram);
					// Set The Drawing Color To Light Green (Can Be Changed By Shader, Etc...)
					cgGLSetParameter4f(color, 0.5f, 1.0f, 0.5f, 1.0f);
					}

				// Start Drawing Our Mesh
				for (int x = 0; x < vbuffer_size - 1; x++)
					{// Draw A Triangle Strip For Each Column Of Our Mesh
					glBegin(GL_TRIANGLE_STRIP);
					for (int z = 0; z < vbuffer_size - 1; z++)
						{// Set The Wave Parameter Of Our Shader To The Incremented Wave Value From Our Main Program
						cgGLSetParameter3f(wave, wave_movement, 1.0f, 1.0f);
						glVertex3f(mesh[x][z][0], mesh[x][z][1], mesh[x][z][2]);
						// Draw Vertex
						glVertex3f(mesh[x+1][z][0], mesh[x+1][z][1], mesh[x+1][z][2]);
						// Draw Vertex
						wave_movement += 0.00001f;
						// Increment Our Wave Movement
						if (wave_movement > (2*Pi))
							// Prevent Crashing	
							wave_movement = 0.0f;
						}
					glEnd();
					}

				if (this->cg_loaded && cg_enable)
					cgGLDisableProfile(cgVertexProfile);					// Disable Our Vertex Profile

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);					// Draw Our Mesh In Wireframe Mode

				}

			void Destroy(){
				room.Destroy();
				quad.Destroy();
				this->fireEngine.Destroy();

				// clean cg
				cgDestroyContext(cgContext);							// Destroy Our Cg Context And All Programs Contained Within
				this->cg_loaded = false;

			}

			void Restart()
			{
			}
	};
} // namespace DGL




#endif // ___DE_CG_H