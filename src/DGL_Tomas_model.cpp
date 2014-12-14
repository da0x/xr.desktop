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

#include "DShared.h"
#include "DGL_Main.h"
#include "DGL_Matrix.h"
#include "DGL_Texture.h"
#include "DGL_Tomas_Model.h"


namespace DGL {
	namespace Tomas {

		std::vector<Model::Mesh> Model::meshes_;
		std::vector<Model::Material> Model::materials_;
		std::map<std::string,unsigned int> Model::materialnames_;

		void Model::Init() {
			dlist_=0;
			showbound_=false;
		}

		void Model::Destroy() {
			for(unsigned int i=0;i<textures_.size();i++)
				textures_[i].Delete();
			glDeleteLists(dlist_,1);
		}

		Model::Model() {
			dlist_=0;
			showbound_=false;
		}

		Model::~Model() {
			Destroy();
		}

		void Model::SetBoundingBox(float minx,float miny,float minz,
			float maxx,float maxy,float maxz)
		{
			boundcoords_.clear();
			boundcoords_.push_back(Vector(minx,maxy,minz));
			boundcoords_.push_back(Vector(minx,miny,minz));
			boundcoords_.push_back(Vector(maxx,maxy,minz));
			boundcoords_.push_back(Vector(maxx,miny,minz));
			boundcoords_.push_back(Vector(maxx,maxy,maxz));
			boundcoords_.push_back(Vector(maxx,miny,maxz));
			boundcoords_.push_back(Vector(minx,maxy,maxz));
			boundcoords_.push_back(Vector(minx,miny,maxz));
			boundcoords_.push_back(Vector(minx,maxy,minz));
			boundcoords_.push_back(Vector(minx,miny,minz));
		}
		
		void Model::ShowBoundingBox(bool show=true) {
			showbound_=show;
		}

		Model::Mesh &Model::GetCurrentMesh() {
			return meshes_.back();
		}

		unsigned int Model::GetNMeshes() {
			return meshes_.size();
		}

		void Model::AddMesh(const char *name) {
			meshes_.push_back(Mesh(name));
		}

		Model::Material &Model::GetCurrentMaterial() {
			return materials_.back();
		}

		void Model::AddMaterial(const char *name) {
			materialnames_[std::string(name)]=materials_.size();
			materials_.push_back(Material());
		}

		int Model::GetMaterialID(const char *name) {
			return materialnames_[name];
		}

		void Model::CreateDisplayLists() {
			dlist_=glGenLists(1);
			glNewList(dlist_,GL_COMPILE);
			for(unsigned int i=0;i<meshes_.size();i++) {
				glPushMatrix();
				meshes_[i].Draw();
				glPopMatrix();
			}
			glEndList();
			meshes_.clear();
			materials_.clear();
			materialnames_.clear();  
		}

		Model::Triangle::Triangle(GLushort *v) {
			v1_=v[0]; v2_=v[1]; v3_=v[2];
			groups_=0; material_=-1;
		}

		Model::Mesh::Mesh(const char *name=NULL) {
			if(name)
				name_=name;
		}

		Model::Mesh::~Mesh() {
		}

		void Model::Mesh::SetTransform(const Matrix &mat) {
			matrix_=mat;
		}

		void Model::Mesh::AddVertex(const Vector &vec) {
			v_.push_back(Vertex(vec));
		}

		void Model::Mesh::AddTexCoord(const Vector &vec) {
			texv_.push_back(vec);
		}

		void Model::Mesh::CalculateNormal(const GLushort face) {
			Triangle &tri=tri_[face];
			Vector &v1=v_[tri.v1_].coords_,&v2=v_[tri.v2_].coords_;
			Vector &v3=v_[tri.v3_].coords_;
			Vector va=v1-v2,vb=v2-v3;
			tri.normal_=va * vb; // cross
			tri.normal_.normalize();
		}

		void Model::Mesh::AddTriangle(const Triangle &tri) {
			tri_.push_back(tri);
			CalculateNormal(tri_.size()-1);
			v_[tri.v1_].usedby_.push_back(tri_.size()-1);
			v_[tri.v2_].usedby_.push_back(tri_.size()-1);
			v_[tri.v3_].usedby_.push_back(tri_.size()-1);
		}

		void Model::Mesh::SetMaterial(GLushort face,GLint mat) {
			tri_[face].material_=mat;
		}

		void Model::Mesh::SetSmoothGroups(GLushort face,GLuint groups) {
			tri_[face].groups_=groups;
		}

		unsigned int Model::Mesh::NFaces() {
			return tri_.size();
		}

		unsigned int Model::Mesh::NVertices() {
			return v_.size();
		}

		void Model::Mesh::Draw() {
			//   glBlendEquation(GL_FUNC_ADD);
			//   glBlendFunc(GL_ZERO,GL_SRC_ALPHA);
			//   glBlendFunc(GL_ZERO,GL_DST_ALPHA);
			//   glBlendEquation(GL_FUNC_SUBTRACT);
			//   glBlendFunc(GL_SRC_ALPHA,GL_ONE);

			matrix_.MultGL();

			Material defaultmaterial;

			defaultmaterial.Select();
			GLint lastmaterial=-1;

			glPushAttrib(GL_ENABLE_BIT|GL_TEXTURE_BIT|GL_COLOR_BUFFER_BIT);
			glBlendFunc(GL_ONE,GL_ZERO);

			if(texv_.size())
				glEnable(GL_TEXTURE_2D);

			glBegin(GL_TRIANGLES);
			for(unsigned int i=0;i<tri_.size();i++) {
				Triangle &tri=tri_[i];
				if(tri.material_!=lastmaterial) {
					lastmaterial=tri.material_;
					glEnd();
					if(lastmaterial>=0)
						materials_[lastmaterial].Select();
					else
						defaultmaterial.Select();
					glBegin(GL_TRIANGLES);
				}
				Vector &v1=v_[tri.v1_].coords_,&v2=v_[tri.v2_].coords_;
				Vector &v3=v_[tri.v3_].coords_;
				std::vector<GLushort> &usedby1=v_[tri.v1_].usedby_;
				std::vector<GLushort> &usedby2=v_[tri.v2_].usedby_;
				std::vector<GLushort> &usedby3=v_[tri.v3_].usedby_;
				Vector normal;

				//       if(!tri.groups_)
				// 	normal=tri.normal_;
				//       else {
				for(unsigned int j=0;j<usedby1.size();j++) 
					if(!tri.groups_ || tri.groups_&tri_[usedby1[j]].groups_)
						normal+=tri_[usedby1[j]].normal_;
				normal.normalize();
				//       }
				glNormal3f(normal.x_(),normal.y_(),normal.z_());
				if(tri.v1_<texv_.size())
					glTexCoord2f(texv_[tri.v1_].x_(),texv_[tri.v1_].y_());
				glVertex3f(v1.x_(),v1.y_(),v1.z_());

				//      if(tri.groups_) {
				normal.Set(0.0,0.0,0.0);
				for(unsigned int j=0;j<usedby2.size();j++) 
					if(!tri.groups_ || tri.groups_&tri_[usedby2[j]].groups_)
						normal+=tri_[usedby2[j]].normal_;
				normal.normalize();
				//      }
				glNormal3f(normal.x_(),normal.y_(),normal.z_());
				if(tri.v2_<texv_.size())
					glTexCoord2f(texv_[tri.v2_].x_(),texv_[tri.v2_].y_());
				glVertex3f(v2.x_(),v2.y_(),v2.z_());

				//      if(tri.groups_) {
				normal.Set(0.0,0.0,0.0);
				for(unsigned int j=0;j<usedby3.size();j++) 
					if(!tri.groups_ || tri.groups_&tri_[usedby3[j]].groups_)
						normal+=tri_[usedby3[j]].normal_;
				normal.normalize();
				//      }
				glNormal3f(normal.x_(),normal.y_(),normal.z_());
				if(tri.v3_<texv_.size())
					glTexCoord2f(texv_[tri.v3_].x_(),texv_[tri.v3_].y_());
				glVertex3f(v3.x_(),v3.y_(),v3.z_());
			}
			glEnd();
			glPopAttrib();
			glDepthMask(GL_TRUE);
		}

		Model::Material::Material() :
				twosided_(false),
				alpha_(1.0f),
				param_(None), 
				texmode_(TextureReplace)
		{
			ambient_[0]=0.2f;
			ambient_[1]=0.2f;
			ambient_[2]=0.2f;
			ambient_[3]=1.0f;
			diffuse_[0]=0.8f;
			diffuse_[1]=0.8f;
			diffuse_[2]=0.8f;
			diffuse_[3]=1.0f;
			specular_[0]=0.0f;
			specular_[1]=0.0f;
			specular_[2]=0.0f;
			specular_[3]=1.0f;
			shininess_=0.0f;
		}

		void Model::Material::SetAlpha(const GLfloat alpha) {
			alpha_=alpha;
			ambient_[3]=alpha;
			diffuse_[3]=alpha;
			specular_[3]=alpha;
		}

		void Model::Material::SetAmbient(const GLfloat *ambient) {
			for(unsigned int i=0;i<3;i++)
				ambient_[i]=ambient[i];
		}

		void Model::Material::SetDiffuse(const GLfloat *diffuse) {
			for(unsigned int i=0;i<3;i++)
				diffuse_[i]=diffuse[i];
		}

		void Model::Material::SetSpecular(const GLfloat *specular) {
			for(unsigned int i=0;i<3;i++)
				specular_[i]=specular[i];
		}

		void Model::Material::SetShininess(const GLfloat shininess) {
			shininess_=shininess;
		}

		void Model::Material::SetParameter(Parameter param) {
			param_=param;
		}

		void Model::Material::Set(const GLfloat *values) {
			switch(param_)
			{
				case(Ambient):
					SetAmbient(values);
					break;
				case(Diffuse):
					SetDiffuse(values);
					break;
				case(Specular):
					SetSpecular(values);
					break;
				default:
					return;
			}
		}

		void Model::Material::SetTextureMode(TextureMode flag) {
			texmode_=flag;
			if(!texid_)
				return;
			glBindTexture(GL_TEXTURE_2D,texid_);
			switch(flag)
			{
				case(TextureModulate):
					glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
					break;
				case(TextureReplace):
					glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
					break;
				default:
					break;
			}
			glBindTexture(GL_TEXTURE_2D,0);
		}

		void Model::Material::Set(char *name) {
			std::ofstream log;
			log.open("TextureLoader.log", std::ios::app | std::ios::out );

			if(param_==Texture) {
				texid_.Build(name);
				if(!texid_) {
					log << "Loading of texture " << name << " failed!\n";
					glBindTexture(GL_TEXTURE_2D,0);
					return;
				} else
					log << "Loaded " << name << " to texture " << texid_ << "\n";
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
				// Should depend on texture flags...
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
				if(texmode_==TextureModulate)
					glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
				else
					glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
				log << " ENV_MODE is ";
				if(texmode_==TextureModulate)
					log << "Modulate\n";
				else if(texmode_==TextureReplace)
					log << "Replace\n";
				else
					log << "unknown\n";
				glBindTexture(GL_TEXTURE_2D,0);

				log.close();
				return;
			}
		}

		void Model::Material::Set(const GLubyte *values) {
			GLfloat fvalues[3];
			for(unsigned int i=0;i<3;i++)
				fvalues[i]=((GLfloat)values[i])/255.0;
			Set(fvalues);
		}

		void Model::Material::Set(const GLfloat value) {
			if(param_==Shininess)
				SetShininess(value);
			else if(param_==Transparency)
				SetAlpha(1.0-value/100.0);
		}

		void Model::Material::Set(const GLushort value) {
			if(param_==Transparency)
				SetAlpha(1.0-((GLfloat)value)/100.0);
			else {
				GLfloat fvalue=((GLfloat)value)/255.0;
				Set(fvalue);
			}
		}

		void Model::Material::SetTwoSided(const bool twosided) {
			twosided_=twosided;
		}

		void Model::Material::Select() {
			GLenum face;
			if(twosided_)
				face=GL_FRONT_AND_BACK;
			else
				face=GL_FRONT;
			glMaterialfv(face,GL_AMBIENT,ambient_);
			glMaterialfv(face,GL_DIFFUSE,diffuse_);
			glMaterialfv(face,GL_SPECULAR,specular_);
			glMaterialf(face,GL_SHININESS,shininess_);
			glColor4fv(ambient_);  // Ambient or diffuse? That's the question...
			if(texid_)
				glBindTexture(GL_TEXTURE_2D,texid_);
			else
				glBindTexture(GL_TEXTURE_2D,0);
			//  if(alpha_<1.0)
			//    glDepthMask(GL_FALSE);
			//  else
			//    glDepthMask(GL_TRUE);
			//  glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		}

	} // namespace Tomas
} // namespace DGL
