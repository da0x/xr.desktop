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
#include "DGL_Color.h"
#include "DGL_Frustum.h"
#include "DGL_Extensions.h"
#include "DGL_Shader.h"
#include "DGL_Model.h"
#include "DGL_Extensions.h"
#include "DGL_Font.h"

namespace DGL {

	Frustum		Model::_frustum;

	Model::Model() : position(0,0,0), angles(0,0,0)
		{
		this->loaded = false;
		this->numMeshes = 0;
		this->meshes = NULL;
		this->numMaterials = 0;
		this->materials = NULL;
		this->numTriangles = 0;
		this->triangles = NULL;
		this->numVertices = 0;
		this->vertices = NULL;
		this->radius = 0;
		}

	Model::~Model()
		{
		if(this->loaded)
			this->Destroy();
		}

	void Model::Destroy()
		{
		this->shaderLib.DestroyShaders();
		this->shaderLib.DestroyLibrary();

		int i;
		for ( i = 0; i < this->numMeshes; i++ )
			delete[] this->meshes[i].triangleIndices;
		for ( i = 0; i < this->numMaterials; i++ )
			{
			delete[] this->materials[i].textureFilename;
			}

		this->numMeshes = 0;
		delete[] this->meshes;		

		this->numMaterials = 0;
		delete[] this->materials;

		this->numTriangles = 0;
		delete[] this->triangles;

		this->numVertices = 0;
		delete[] this->vertices;

		this->loaded = false;
		this->radius = 0;
		}

	void Model::Update(float msec)
		{
		this->shaderLib.UpdateShaders(msec);
		}

	void Model::Render() const
	{
		if(! this->loaded )
			{
			return RenderModelCoords();
			}

        if( this->isInFrustum() )
		{
			glPushMatrix();

			glTranslatef( this->position.x_(), this->position.y_(), this->position.z_());
			glRotatef( this->angles.x_(), 1,0,0);
			glRotatef( this->angles.y_(), 0,1,0);
			glRotatef( this->angles.z_(), 0,0,1);

			// Draw by group
			for ( int i = 0; i < this->numMeshes; i++ )
			{
				const int materialIndex = this->meshes[i].materialIndex;
				const Model::Material *mat;

				if ( materialIndex >= 0 )
				{
					mat = &this->materials[materialIndex];
					glMaterialfv( GL_FRONT, GL_AMBIENT, mat->ambient );
					glMaterialfv( GL_FRONT, GL_DIFFUSE, mat->diffuse );
					glMaterialfv( GL_FRONT, GL_SPECULAR, mat->specular );
					glMaterialfv( GL_FRONT, GL_EMISSION, mat->emissive );
					glMaterialf( GL_FRONT, GL_SHININESS, mat->shininess );
					
					mat->shader->Begin();
					if ( mat->shader->Multitexture() &&	DGL::Extensions::IsEnabled_GL_ARB_multitexture() )
					{
						this->renderMeshMT( i );
					}
					else
					{
						this->renderMesh( i );
					}
					mat->shader->End();
				}
				else
				{
					// Material properties?
					glPushAttrib( GL_ENABLE_BIT);
					glDisable( GL_TEXTURE_2D );
					this->renderMesh( i );
					glPopAttrib();
				}
			}

			glPopMatrix();

		}// in frustum
	}

	void Model::renderMeshMT(int _meshID) const
	{
		glBegin( GL_TRIANGLES );
		{
			for ( int j = 0; j < this->meshes[_meshID].numTriangles; j++ )
			{
				const int triangleIndex = this->meshes[_meshID].triangleIndices[j];
				const Triangle* pTri = &this->triangles[triangleIndex];

				for ( int k = 0; k < 3; k++ )
				{
					const int index = pTri->vertexIndices[k];

					glNormal3fv( pTri->vertexNormals[k] );
					glMultiTexCoord2fARB( GL_TEXTURE0_ARB, pTri->texCoordS[k], pTri->texCoordT[k] );
					glMultiTexCoord2fARB( GL_TEXTURE1_ARB, pTri->texCoordS[k], pTri->texCoordT[k] );
					glVertex3fv( this->vertices[index].location );
				}
			}
		}
		glEnd();
	}

	void Model::renderMesh(int _meshID) const
	{
		glBegin( GL_TRIANGLES );
		{
			for ( int j = 0; j < this->meshes[_meshID].numTriangles; j++ )
			{
				int triangleIndex = this->meshes[_meshID].triangleIndices[j];
				const Triangle* pTri = &this->triangles[triangleIndex];

				for ( int k = 0; k < 3; k++ )
				{
					const int index = pTri->vertexIndices[k];

					glNormal3fv( pTri->vertexNormals[k] );
					glTexCoord2f( pTri->texCoordS[k], pTri->texCoordT[k] );
					glVertex3fv( this->vertices[index].location );
				}
			}
		}
		glEnd();
	}

	void Model::InitShaders( std::string modelFilename )
		{
		// first load the shader library
		this->shaderLib.LoadLibrary( va("%s.shader", modelFilename.c_str()));

		// get each material its own shader,
		// this way the shaderLib can know which shaders in the scripts are used
		for ( int i = 0; i < this->numMaterials; i++ )
			{
			if ( *this->materials[i].textureFilename )
				{
				char *s = &this->materials[i].textureFilename[2]; // skip the first 2 chars, ".\" string
				this->materials[i].shader = this->shaderLib.GetShaderForTexture( va("textures/%s", s));
				}
			else
				{
				// if the material doesn't have a texture filename, then load the default texture.
				this->materials[i].shader = this->shaderLib.GetShaderForTexture( "textures/default.jpg");
				}
			}

		// now init the shaders we're using only. this should be done internally in shaderLib
		this->shaderLib.InitShaders();
		}

	void Model::RenderModelCoords() const
		{
		glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glPushMatrix();

			glTranslatef(this->position.x_(), this->position.y_(), this->position.z_());
			glRotatef(this->angles.x_(), 1,0,0);
			glRotatef(this->angles.y_(), 0,1,0);
			glRotatef(this->angles.z_(), 0,0,1);

			// x axis
			DGL::Color::ColorRed().MakeCurrent();
			glBegin(GL_LINES);
				glVertex3f( 0, 0, 0);
				glVertex3f( 1, 0, 0);
			glEnd();

			// y axis
			DGL::Color::ColorGreen().MakeCurrent();
			glBegin(GL_LINES);
				glVertex3f( 0, 0, 0);
				glVertex3f( 0, 1, 0);
			glEnd();

			// z axis
			DGL::Color::ColorBlue().MakeCurrent();
			glBegin(GL_LINES);
				glVertex3f( 0, 0, 0);
				glVertex3f( 0, 0, 1);
			glEnd();

		glPopMatrix();
		glPopAttrib();
		}

	void Model::RenderModelName(DGL::Font &font) const
		{
		if(this->isInFrustum())
			{
			DGL::MatrixOp::BalloonPoint::Begin(this->position);
			font.Print3D( Vector::vzero(), "Model: %s", this->modelname);
			DGL::MatrixOp::BalloonPoint::End();
			}
		}

	void Model::calculateRadius()
		{
		this->radius = 0;
		for (int i=0; i< this->numVertices; i++)
			{
			if( this->vertices[i].location.length() > this->radius )
				this->radius = this->vertices[i].location.length();
			}
		}

	bool Model::isInFrustum() const
		{
		return Model::_frustum.SphereInFrustum( this->position, this->radius );
		}

	void Model::_FrustumUpdate()
		{
		Model::_frustum.Extract();
		}

}//namespace DGL

