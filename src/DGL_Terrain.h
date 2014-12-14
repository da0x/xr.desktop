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

#ifndef ___DGL_TERRAIN_H
#define ___DGL_TERRAIN_H

/*
TODO List:
- ...
*/


namespace DGL {

	class TerrainVertex {
		protected:
			Vertex			Pos;
			Color			Color;
			float			pTexCoord0[2];
			float			pTexCoord1[2];
			Vector	normal;
		public:
			TerrainVertex(): Color(Color::ColorWhite()) { }
			TerrainVertex(float *pos, float *texCoord0, float *texCoord1, float *norm, float *color = NULL){
				Set( pos, texCoord0, texCoord0, norm, color);
			}
			
			void SetTexCoord0(float s, float t){
				pTexCoord0[0] = s;
				pTexCoord0[1] = t;
			}
			void SetTexCoord0(const float *st){
				pTexCoord0[0] = st[0];
				pTexCoord0[1] = st[1];
			}
			void SetTexCoord1(float s, float t){
				pTexCoord1[0] = s;
				pTexCoord1[1] = t;
			}
			void SetTexCoord1(const float *st){
				pTexCoord1[0] = st[0];
				pTexCoord1[1] = st[1];
			}
			
			void Set( float *pos, float *texCoord0, float *texCoord1, float *norm, float *color = NULL){
				int i,c;
				Pos.Set(pos);
				normal.Set(norm);

				c = 2;
				for ( i=0; i< c; i++){
					pTexCoord0[i] = texCoord0[i];
					pTexCoord1[i] = texCoord1[i];
				}

				if(color)
					Color.Set(color);
				else
					Color = Color::ColorWhite();
			}
			__forceinline void PopMultitexture(){
				Color.MakeCurrent();
				glNormal3fv(normal);
				glMultiTexCoord2fvARB( GL_TEXTURE0_ARB, pTexCoord0);
				glMultiTexCoord2fvARB( GL_TEXTURE1_ARB, pTexCoord1);
				Pos.Pop();
			}

			__forceinline void Pop(){
				Color.MakeCurrent();
				glNormal3fv(normal);
				glTexCoord2fv(pTexCoord0);
				Pos.Pop();
			}

			__forceinline float *GetPosPtr(){return Pos;}
			__forceinline float *GetTexCoord0Ptr(){return pTexCoord0;}
			__forceinline float *GetTexCoord1Ptr(){return pTexCoord1;}
			__forceinline float *GetNormalPtr(){return normal;}
			__forceinline float *GetColorPtr(){return Color;}
	};



	class Terrain {
	private:
		static const int	_MapSizeX = 256;	// Size Of Map ( x dimention )
		static const int	_MapSizeZ = 256;	// Size Of Map ( z dimention )
		bool				ready;				// Ready?
		float				terrainScale;		// every thing depends on this one
		int					stepLength;			// Step SkyboxLength
		float				heightFactor;		// Ratio That The Y Is Scaled According To The X And Z
		bool				wireFrame;			// Wire Frame Rendering?
		bool				drawDetailTexture;	// Draw Detail?
		unsigned char		*heightMapData;		// Holds The Height Map Data
		TerrainVertex		*vertexArray;		// Vertex data
		int					detailStep;			// 1 = Max Detail. use the cvar "r_terrainLowestDetail"

		DGL::Texture		texTerrain;
		DGL::Texture		texDetail;

		GLuint				list;


		int VertexHeight(int X, int Z){
			int x = X % this->_MapSizeX;
			int z = Z % this->_MapSizeZ;

			return this->heightMapData[x + (z * this->_MapSizeX)];
		}

		// Loads The .RAW File And Stores It In this->heightMapData
		void LoadRAW(char *name){
			FILE *fp;
			int nSize = this->_MapSizeX*this->_MapSizeZ;
			this->heightMapData = new unsigned char [ nSize ];
			if(!this->heightMapData){
				LogError("DTarrain_s: Cannot find enough memory to load terrain");
				return;
			}

			fp = fopen( name, "rb" );
			if ( fp == NULL ){
				LogError("DTarrain_s: Cannot Find The SkyboxHeight Map!");
				return;
			}

			fread(this->heightMapData, 1, nSize, fp );

			int result = ferror( fp );
			if (result)
				LogError("DTarrain_s: Failed To Get Data!");

			// Close The File
			fclose(fp);
			LogPrint("RAW file '%s' Loaded Successfully",name);
		}

	public:

		void LoadTerrain(char *Tname){
			DMacro_TraceEnter(DTerrain::LoadTerrain);

			LogPrint("-------- Loading Terrain --------");
			LoadRAW(va("maps/%s.raw", Tname));
			this->texTerrain.Build(va("textures/terrain/%sTexture.jpg",Tname));
			this->texDetail.Build(va("textures/terrain/%sDetail.jpg",Tname));

			LogPrint("Building Terrain...");
			BuildTerrain();
			LogPrint("Done Loading Terrain");
			LogPrint("---------------------------------");
			DMacro_TraceLeave();
		}

		bool BuildTerrain(){
			DMacro_TraceEnter(DTerrain___BuildTerrain);
			int i,j;

			this->vertexArray = new TerrainVertex[this->_MapSizeX*this->_MapSizeZ];
			if(!this->vertexArray ){
				LogError("Not enough memory to store verticies of terrain");
				return false;
			}

			Vector pos;
			Vector texCoord0;	// terrain texture
			Vector texCoord1;	// detail texture

			for (j=0; j< this->_MapSizeZ; j++){
				for (i=0; i< this->_MapSizeX; i++){
					pos.x_() = float(i * this->stepLength);
					pos.y_() = float(VertexHeight(j,i)/ 255.0f)*this->heightFactor;
					pos.z_() = float(j * this->stepLength);

					texCoord0.x_() = float((j)/(float)this->_MapSizeZ);
					texCoord0.y_() = float((this->_MapSizeX-i)/(float)this->_MapSizeX);

					texCoord1.x_() = float(i/6.0f);
					texCoord1.y_() = float(j/6.0f);

					// find the normal
					Vector v1,v2,normal;
					v1.x_() = float((i+1) * this->stepLength);
					v1.y_() = float(VertexHeight(i+1,j)/ 255.0f)*this->heightFactor;
					v1.z_() = float(j * this->stepLength);

					v2.x_() = float(i * this->stepLength);
					v2.y_() = float(VertexHeight(i,j+1)/ 255.0f)*this->heightFactor;
					v2.z_() = float((j+1) * this->stepLength);

					v1 = pos - v1;
					v2 = pos - v2;
					normal = (v2 * v1).normal();

					this->vertexArray[i + (j * this->_MapSizeX)].Set(pos, texCoord0, texCoord1, normal);
				}
			}
			// no need for the height map
			delete [] this->heightMapData;
			this->heightMapData = NULL;
		
			this->list = glGenLists(1);
			glNewList(this->list, GL_COMPILE);
			{
				// Now we add the opengl arrays
				if(Extensions::IsEnabled_GL_EXT_vertex_array()){
					// vertecies
					glVertexPointerEXT( 3, GL_FLOAT, sizeof(TerrainVertex), this->_MapSizeX*this->_MapSizeZ, this->vertexArray);
					glEnableClientState( GL_VERTEX_ARRAY_EXT);
					
					// colors
					glColorPointerEXT( 4, GL_UNSIGNED_BYTE, sizeof(TerrainVertex), this->_MapSizeX*this->_MapSizeZ, (float*)this->vertexArray+3);
					glEnableClientState( GL_COLOR_ARRAY);

					if(Extensions::IsEnabled_GL_ARB_multitexture()){
						// 1st texture coordinates
						glClientActiveTextureARB( GL_TEXTURE0_ARB);
						glTexCoordPointerEXT( 2, GL_FLOAT, sizeof(TerrainVertex), this->_MapSizeX*this->_MapSizeZ, (float*)this->vertexArray+7);
						glEnableClientState( GL_TEXTURE_COORD_ARRAY);
						
						// 2nd texture coordinates
						glClientActiveTextureARB( GL_TEXTURE1_ARB); 
						glTexCoordPointerEXT( 2, GL_FLOAT, sizeof(TerrainVertex), this->_MapSizeX*this->_MapSizeZ, (float*)this->vertexArray+9);
						glEnableClientState( GL_TEXTURE_COORD_ARRAY);

						glClientActiveTextureARB( GL_TEXTURE0_ARB);
					} else {
						glEnable( GL_TEXTURE_2D);
						glTexCoordPointerEXT( 2, GL_FLOAT, sizeof(TerrainVertex), this->_MapSizeX*this->_MapSizeZ, (float*)this->vertexArray+7);
						glEnableClientState( GL_TEXTURE_COORD_ARRAY);
					}

					// normals
					glNormalPointerEXT( GL_FLOAT, sizeof(TerrainVertex), this->_MapSizeX*this->_MapSizeZ, (float*)this->vertexArray+11);
					glEnableClientState( GL_NORMAL_ARRAY_EXT);

					if(Extensions::IsEnabled_GL_EXT_compiled_vertex_array()){
						// Lock the array
						glLockArraysEXT( 0, this->_MapSizeX*this->_MapSizeZ);
					}
				}

				// begin drawing
				this->detailStep	= 8;

				if(Extensions::IsEnabled_GL_ARB_multitexture()){
					// texture unit 1
					glActiveTextureARB(GL_TEXTURE0_ARB);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, this->texTerrain);

					if(Extensions::IsEnabled_GL_EXT_texture_env_combine()){
						glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
						glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);
					} else if(Extensions::IsEnabled_GL_EXT_texture_env_add()){
						glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					}

					// texture unit 2
					glActiveTextureARB(GL_TEXTURE1_ARB);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, this->texDetail);

					if(Extensions::IsEnabled_GL_EXT_texture_env_combine()){
						glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
						glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
					} else if(Extensions::IsEnabled_GL_EXT_texture_env_add()){
						glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
					}
				} else {
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, this->texTerrain);
				}

				Color::ColorWhite().MakeCurrent();

				glPushMatrix();
				glTranslatef( -this->_MapSizeX/ 2.0f, 0, -this->_MapSizeZ/ 2.0f);

				int i,j;
				glBegin(GL_TRIANGLES);
				if(Extensions::IsEnabled_GL_ARB_multitexture()){
					for(i =0 ; i<this->_MapSizeX-this->detailStep; i+= this->detailStep){
						glBegin(GL_TRIANGLE_STRIP);
						for(j= 0; j<this->_MapSizeZ-this->detailStep; j+= this->detailStep){
							this->vertexArray[(i+this->detailStep)+(j)* this->_MapSizeX].PopMultitexture();
							this->vertexArray[(i)+(j) *this->_MapSizeX].PopMultitexture();
						}
						glEnd();
					}
				} else {
					for(i =0 ; i<this->_MapSizeX-this->detailStep; i+= this->detailStep){
						glBegin(GL_TRIANGLE_STRIP);
						for(j= 0; j<this->_MapSizeZ-this->detailStep; j+= this->detailStep){
							this->vertexArray[(i+this->detailStep)+(j)* this->_MapSizeX].Pop();
							this->vertexArray[(i)+(j) *this->_MapSizeX].Pop();
						}
						glEnd();
					}
				}
				glEnd();

				glPopMatrix();

				if(Extensions::IsEnabled_GL_ARB_multitexture())
				{
					glActiveTextureARB(GL_TEXTURE1_ARB);
					glDisable(GL_TEXTURE_2D);
					glActiveTextureARB(GL_TEXTURE0_ARB);
					// restore env mode
					glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				}
			}
			glEndList();

			DMacro_TraceLeave();
			this->ready = true;
			return true;
		}
		
		void Render(){
			if(!this->ready) return;
			DMacro_TraceEnter(DTerrain::Render);

			glCallList(this->list);
			
			DMacro_TraceLeave();
		}
		
		void ClipPoint(Vector p){
			int i = p.x_() / this->stepLength;
			int j = p.z_() / this->stepLength;
			if( i<= this->_MapSizeX && i>=0 &&
				i<= this->_MapSizeZ && j>=0)
				/*			VectorAdd(
				this->vertexArray[i + (j * this->_MapSizeX)].pPos,
				this->vertexArray[i + (j * this->_MapSizeX)].pNormal,
				p);*/
				//			if(p[1] < this->vertexArray[i + (j * this->_MapSizeX)].pPos[1]+1)
				p.y_() = this->vertexArray[i + (j * this->_MapSizeX)].GetPosPtr()[1]+1;
		}


		Terrain(){
			initvars();
		}		

		void initvars(){
			this->ready				= false;
			this->wireFrame			= false;
			this->drawDetailTexture	= true;
			this->heightMapData		= NULL;
			this->terrainScale		= 0.5f;
			this->stepLength		= 3.0f		*this->terrainScale;
			this->heightFactor		= 75.0f		*this->terrainScale;
			this->detailStep		= 1;
		}

		void Destroy()
		{
			if(this->ready)
			{
				this->ready = false;
				delete [] this->heightMapData;
				delete [] this->vertexArray;
				
				this->texTerrain.Delete();
				this->texDetail.Delete();

				glDeleteLists(this->list , 1);
			}
		}
	};

}//namespace DGL


#endif // ___DGL_TERRAIN_H