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
#include "DGL_Tomas_M3ds.h"


namespace DGL {
	namespace Tomas {


		M3ds::M3ds(const char *filename) : Model() {
			log.open("3dsLoader.log", std::ios::out | std::ios::app );
			log << "==================\n";
			log << "Loading model: " << filename << "\n";
			std::ifstream instream(filename,std::ios::binary);
			if(!instream){
				log << "Error: cannot load model, file not found\n";
				log.close();
				return;
			}
			Load(instream);
			log.close();
		}

		M3ds::M3ds(std::istream &instream) : Model() {
			Load(instream);
		}

		void M3ds::ReadChunks(std::istream &instream,int chunklength)
		{
			int bytesread=0;
			bool done=false;
			while(!instream.eof() && !done && bytesread < chunklength)
			{
				GLushort id=0x6666;
				GLuint length=12345;
				instream.read((char *)&id,2);
				instream.read((char *)&length,4);
				bytesread+=length;
				switch(id)
				{
					case(0x0001): {
						GLfloat unknown;
						instream.read((char *)&unknown,4);
						log << "Unknown==" << unknown << "\n";
									} break;
					case(0x0002): {
						GLuint version=666;
						instream.read((char *)&version,4);
						log << "Version " << version << "\n";
									} break;
					case(0x0010): {
						GLfloat rgb[3];
						instream.read((char *)rgb,12);
						log << "Colour: (" << rgb[0] << "," << rgb[1] << ",";
						log << rgb[2] << ")\n";
						//	Mesh::materials_.back().Set(rgb);
									} break;
					case(0x0011): {
						GLubyte rgb[3];
						instream.read((char *)rgb,3);
						log << "Colour: (" << (int)rgb[0] << "," << (int)rgb[1] << ",";
						log << (int)rgb[2] << ")\n";
						GetCurrentMaterial().Set(rgb);
									} break;
					case(0x0012): {
						GLubyte rgb[3];
						instream.read((char *)rgb,3);
						log << "Lin Colour: (" << (int)rgb[0] << "," << (int)rgb[1];
						log << "," << (int)rgb[2] << ")\n";
						//	Mesh::materials_.back().Set(rgb);
									} break;
					case(0x0013): {
						GLfloat rgb[3];
						instream.read((char *)rgb,12);
						log << "Lin Colour: (" << rgb[0] << "," << rgb[1] << ",";
						log << rgb[2] << ")\n";
						//	Mesh::materials_.back().Set(rgb);
									} break;
					case(0x0030): {
						GLushort percent;
						instream.read((char *)&percent,2);
						log << "Percent==" << percent << "s\n";
						GetCurrentMaterial().Set(percent);
									} break;
					case(0x0031): {
						GLfloat percent;
						instream.read((char *)&percent,4);
						log << "Percent==" << percent << "f\n";
						GetCurrentMaterial().Set(percent);
									} break;
					case(0x0100): {
						GLfloat scale;
						instream.read((char *)&scale,4);
						log << "Scale==" << scale << "\n";
									} break;
					case(0x1100): {
						log << "Bitmap: ";

						GLubyte *name = new GLubyte[length-6];
						instream.read((char *)name,length-6);
						delete name;
						log << name << "\n";
									} break;
					case(0x1200): {
						log << "Background color\n";
						GetCurrentMaterial().SetParameter(Material::None);
						log << "*** BACKGROUND COLOR BEGINS ***\n";
						ReadChunks(instream,length-6);
						log << "*** BACKGROUND COLOR ENDS ***\n";
									} break;
					case(0x1300): { 
						log << "Gradient\n";
						GetCurrentMaterial().SetParameter(Material::None);
						GLfloat midpoint;
						instream.read((char *)&midpoint,4);
						log << "*** GRADIENT BEGINS ***\n";
						ReadChunks(instream,length-10);
						log << "*** GRADIENT ENDS ***\n";
									} break;
					case(0x1400): {
						GLfloat bias;
						instream.read((char *)&bias,4);
						log << "Shadow bias==" << bias << "\n";
									} break;
					case(0x1420): {
						GLushort mapsize;
						instream.read((char *)&mapsize,2);
						log << "Shadow map size==" << mapsize << "\n";
									} break;
					case(0x1450): {
						GLfloat filter;
						instream.read((char *)&filter,4);
						log << "Shadow filter==" << filter << "\n";
									} break;
					case(0x1460): {
						GLfloat bias;
						instream.read((char *)&bias,4);
						log << "Ray bias==" << bias << "\n";
									} break;
					case(0x1500): {
						log << "O_CONSTS???\n";
						GLfloat plane[3];
						instream.read((char *)plane,12);
									} break;
					case(0x2100): {
						log << "SCENE: Ambient light, ignoring\n";
						instream.ignore(length-6);
									} break;
					case(0x2200): {
						log << "SCENE: Fog, ignoring\n";
						instream.ignore(length-6);
									} break;
					case(0x2300): {
						log << "SCENE: Distance cue, ignoring\n";
						instream.ignore(length-6);
									} break;
					case(0x2302): {
						log << "SCENE: Layer fog, ignoring\n";
						instream.ignore(length-6);
									} break;
					case(0x3000): {
						log << "SCENE: Default view, ignoring\n";
						instream.ignore(length-6);
									} break;
					case(0x3d3d): {
						log << "Mesh data, length==" << length << "\n";
						log << "*** MESH DATA BEGINS ***\n";
						ReadChunks(instream,length-6);
						log << "*** MESH DATA ENDS ***\n";
									} break;
					case(0x3d3e): {
						GLuint version;
						instream.read((char *)&version,4);
						log << "Mesh version: " << version << "\n";
									} break;
					case(0x4000): {
						log << "Object name: ";
						GLubyte ch;
						std::string name;
						int namelen=0;
						do {
							namelen++;
							instream.read((char *)&ch,1);
							name+=ch;
						} while(ch);
						log << name.c_str() << "\n";
						log << "*** MESH BEGIN ***\n";
						AddMesh(name.c_str());
						ReadChunks(instream,length-6-namelen);
						log << "*** MESH END ***\n";
									} break;
					case(0x4100): {
						log << "Named triangle object, length==" << length << "\n";
						log << "*** NAMED TRIANGLE OBJECT BEGINS ***\n";
						ReadChunks(instream,length-6);
						log << "*** NAMED TRIANGLE OBJECT ENDS ***\n";
									} break;
					case(0x4110): {
						GLushort npoints;
						instream.read((char *)&npoints,2);
						log << "Reading " << npoints << " vertices...\n";
						Mesh &mesh=GetCurrentMesh();
						for(GLushort n=0;n<npoints;n++) {
							GLfloat v[3];
							instream.read((char *)v,12);
							//   	  GLfloat tmp=v[2];
							//   	  v[2]=v[1];
							//   	  v[1]=tmp;
							mesh.AddVertex(Vector(v));
						}
									} break;
					case(0x4111): {
						log << "Vertex flag array: Ignoring\n";
						instream.ignore(length-6);
									} break;
					case(0x4120): {
						GLushort nfaces;
						instream.read((char *)&nfaces,2);
						log << "Reading " << nfaces << " faces... (length==";
						log << length << ")\n";
						Mesh &mesh=GetCurrentMesh();
						for(GLushort n=0;n<nfaces;n++) {
							GLushort v[3],flags;
							instream.read((char *)v,6);
							instream.read((char *)&flags,2);
							mesh.AddTriangle(Triangle(v));
						}
						log << "*** FACE DATA BEGINS ***\n";
						ReadChunks(instream,length-8-8*nfaces);
						log << "*** FACE DATA ENDS ***\n";
									} break;
					case(0x4130): {
						log << "Material group: (length==" << length << ")\n";
						std::string name;
						GLubyte ch;
						do {
							instream.read((char *)&ch,1);
							name+=ch;
						} while(ch);
						log << name.c_str() << "\n";
						GLint mat=GetMaterialID(name.c_str());
						GLushort nfaces;
						instream.read((char *)&nfaces,2);
						log << "  Reading " << nfaces << " faces...\n";
						Mesh &mesh=GetCurrentMesh();
						for(GLushort n=0;n<nfaces;n++) {
							GLushort facenum;
							instream.read((char *)&facenum,2);
							mesh.SetMaterial(facenum,mat);
						}
									} break;
					case(0x4140): {
						GLushort ntexc;
						instream.read((char *)&ntexc,2);
						log << "Reading " << ntexc << " texture coordinates...\n";
						Mesh &mesh=GetCurrentMesh();
						for(GLushort n=0;n<ntexc;n++) {
							GLfloat u,v;
							instream.read((char *)&u,4);
							instream.read((char *)&v,4);
							mesh.AddTexCoord(Vector(u,v,0.0));
						}
									} break;
					case(0x4150): {
						log << "Smooth group, length==" << length << "\n";
						Mesh &mesh=GetCurrentMesh();
						for(GLushort i=0;i<mesh.NFaces();i++) {
							GLuint group;
							instream.read((char *)&group,4);
							mesh.SetSmoothGroups(i,group);
						}
						log << "\n";
									} break;
					case(0x4160): {
						GLfloat matrix[16];
						for(unsigned int i=0;i<16;i++)
							if(i==3 || i==7 || i==11 || i==15) {
								if(i<15)
									matrix[i]=0.0;
								else
									matrix[i]=1.0;
							} else
								instream.read((char *)&(matrix[i]),4);
							log << "Read local mesh transformation\n";
							for(unsigned int i=0;i<16;i++) {
								log << " " << matrix[i];
							}
							log << "\n";
							//GetCurrentMesh().SetTransform(Matrix(matrix));
									} break;
					case(0x4165): {
						GLubyte index;
						instream.read((char *)&index,1);
						log << "Mesh color==" << (int)index << "?\n"; 
									} break;
					case(0x4170): {
						log << "Texture info\n";
						instream.ignore(length-6);  // FIXME !!!
									} break;
					case(0x4600): {
						log << "Direct light: Ignoring\n";
						instream.ignore(length-6);
									} break;
					case(0x4700): {
						log << "Camera: Ignoring\n";
						instream.ignore(length-6);
									} break;
					case(0x7001): {
						log << "Viewport layout: Ignoring\n";
						instream.ignore(length-6);
									} break;
					case(0xa000): {
						GLubyte *name = new GLubyte[length-6];
						instream.read((char *)name,length-6);
						log << "Material name: " << name << "\n";
						AddMaterial((char *)name);
						delete name;
									} break;
					case(0xa010): {
						log << "Ambient\n ";
						GetCurrentMaterial().SetParameter(Material::Ambient);
						ReadChunks(instream,length-6);
									} break;
					case(0xa020): {
						log << "Diffuse\n ";
						GetCurrentMaterial().SetParameter(Material::Diffuse);
						ReadChunks(instream,length-6);
									} break;
					case(0xa030): {
						log << "Specular\n ";
						GetCurrentMaterial().SetParameter(Material::Specular);
						ReadChunks(instream,length-6);
									} break;
					case(0xa040): {
						log << "Shininess\n ";
						GetCurrentMaterial().SetParameter(Material::Shininess);
						ReadChunks(instream,length-6);
									} break;
					case(0xa041): {
						log << "Shin2pct\n ";
						GetCurrentMaterial().SetParameter(Material::None);
						ReadChunks(instream,length-6);
									} break;
					case(0xa050): {
						log << "Transparency\n ";
						GetCurrentMaterial().SetParameter(Material::Transparency);
						ReadChunks(instream,length-6);
									} break;
					case(0xa052): {
						log << "XPFall\n ";
						GetCurrentMaterial().SetParameter(Material::None);
						ReadChunks(instream,length-6);
									} break;
					case(0xa053): {
						log << "RefBlur\n ";
						GetCurrentMaterial().SetParameter(Material::None);
						ReadChunks(instream,length-6);
									} break;
					case(0xa080): {
						log << "Material is self-illuminated (or something)\n";
									} break;
					case(0xa081): {
						log << "Material is two-sided\n";
						GetCurrentMaterial().SetTwoSided(true);
									} break;
					case(0xa084): {
						log << "SelfIlPct\n ";
						GetCurrentMaterial().SetParameter(Material::None);
						ReadChunks(instream,length-6);
									} break;
					case(0xa085): {
						log << "Wire..?\n";
									} break;
					case(0xa087): {
						GLfloat size;
						instream.read((char *)&size,4);
						log << "Wire size==" << size << "?\n";
									} break;
					case(0xa08c): {
						log << "Soft phong\n";
									} break;
					case(0xa08e): {
						log << "Wire abs..?\n";
									} break;      
					case(0xa100): {
						GLushort shading;
						instream.read((char *)&shading,2);
						log << "Shading==" << shading << "\n";
									} break;
					case(0xa200): {
						log << "Texture map!!\n";
						GetCurrentMaterial().SetParameter(Material::Texture);
						ReadChunks(instream,length-6);
									} break;
					case(0xa220): {
						log << "Reflection map\n";
						GetCurrentMaterial().SetParameter(Material::Texture); // FIXME
						GetCurrentMaterial().SetTextureMode(Material::TextureModulate); //FIXME
						ReadChunks(instream,length-6);
									} break;
					case(0xa250): {
						log << "Use RefBlur..?\n";
									} break;
					case(0xa300): {
						log << "File name: ";
						GLubyte *name = new GLubyte[length-6];
						instream.read((char *)name,length-6);
						log << name << "\n";
						GetCurrentMaterial().Set((char *)name);
									} break;
					case(0xa320): {
						log << "Texture data, length==" << length << "\n";
						instream.ignore(length-6);
									} break;
					case(0xa351): {
						log << "Texture flags==";
						GLushort flags;
						instream.read((char *)&flags,2);
						log << flags << "\n";
						if(flags&128)
							GetCurrentMaterial().SetTextureMode(Material::TextureModulate);
						else
							GetCurrentMaterial().SetTextureMode(Material::TextureReplace);
						// 16 or 1 is clamp..?
									} break;
					case(0xa353): {
						log << "Texture blur==";
						GLfloat blur;
						instream.read((char *)&blur,4);
						log << blur << "\n";
									} break;
					case(0xafff): {
						log << "Material entry, length==" << length << "\n";
						log << "*** MATERIAL BEGINS ***\n";
						ReadChunks(instream,length-6);
						log << "*** MATERIAL ENDS ***\n";
									} break;
					case(0xb000): {
						log << "Key frame data\n";
						log << "*** KFDATA BEGINS ***\n";
						ReadChunks(instream,length-6);
						log << "*** KFDATA ENDS ***\n";
									} break;
					case(0xb002): {
						log << "Object node tag, length==" << length << "\n";
						log << "*** NODE TAG BEGINS ***\n";
						ReadChunks(instream,length-6);
						log << "*** NODE TAG ENDS ***\n";
									} break;
					case(0xb008): {
						log << "Key frame Segment, length==" << length << "\n";
						GLushort start,end;
						instream.read((char *)&start,2);
						instream.read((char *)&end,2);
						log << "Start==" << start << " end==" << end << "\n";
						instream.ignore(length-10); // FIXME
									} break;
					case(0xb009): {
						GLushort curtime;
						instream.read((char *)&curtime,2);
						log << "Current time frame==" << curtime << "\n";
						instream.ignore(length-8); // FIXME
									} break;
					case(0xb010): {
						GLubyte *name = new GLubyte[length-12];
						GLushort flags1,flags2,hierarchy;
						instream.read((char *)name,length-12);
						instream.read((char *)&flags1,2);
						instream.read((char *)&flags2,2);
						instream.read((char *)&hierarchy,2);
						log << "Node name: " << name << "\n";
						delete name;
									} break;
					case(0xb013): {
						GLfloat pivot[3];
						instream.read((char *)pivot,12);
						log << "Pivot: (" << pivot[0] << "," << pivot[1] << ",";
						log << pivot[2] << ")\n";
									} break;
					case(0xb014): {
						log << "Bounding box!!? length==" << length << "\n";
						//	ReadChunks(instream,length-6);
						//	instream.ignore(length-6); // FIXME
						// Probably: minx,miny,minz,maxx,maxy,maxz (i.e. an AABB)
						GLfloat bb[6];
						instream.read((char *)bb,24);
						SetBoundingBox(bb[0],bb[1],bb[2],bb[3],bb[4],bb[5]);
									} break;
					case(0xb020): {
						GLushort flags,unknown[4],keys,unknown2;
						instream.read((char *)&flags,2);
						instream.read((char *)unknown,8);
						instream.read((char *)&keys,2);
						instream.read((char *)&unknown2,2);
						for(GLushort k=0;k<keys;k++) {
							GLushort framenum;
							GLuint unknown;
							GLfloat pos[3];
							instream.read((char *)&framenum,2);
							instream.read((char *)&unknown,4);
							instream.read((char *)pos,12);
							log << "Frame " << framenum << ", pos: (" << pos[0] << ",";
							log << pos[1] << "," << pos[2] << ")\n";
						}
									} break;
					case(0xb021): {
						GLushort flags,unknown[4],keys,unknown2;
						instream.read((char *)&flags,2);
						instream.read((char *)unknown,8);
						instream.read((char *)&keys,2);
						instream.read((char *)&unknown2,2);
						for(GLushort k=0;k<keys;k++) {
							GLushort framenum;
							GLuint unknown;
							GLfloat axis[3],rotation;
							instream.read((char *)&framenum,2);
							instream.read((char *)&unknown,4);
							instream.read((char *)&rotation,4);
							instream.read((char *)axis,12);
							log << "Frame " << framenum << ", rotation " << rotation;
							log << ", axis: (" << axis[0] << ",";
							log << axis[1] << "," << axis[2] << ")\n";
						}	
									} break;
					case(0xb022): {
						GLushort flags,unknown[4],keys,unknown2;
						instream.read((char *)&flags,2);
						instream.read((char *)unknown,8);
						instream.read((char *)&keys,2);
						instream.read((char *)&unknown2,2);
						for(GLushort k=0;k<keys;k++) {
							GLushort framenum;
							GLuint unknown;
							GLfloat scale[3];
							instream.read((char *)&framenum,2);
							instream.read((char *)&unknown,4);
							instream.read((char *)scale,12);
							log << "Frame "
								<< framenum
								<< ", scale: ("
								<< scale[0]
								<< ","
								<< scale[1]
								<< ","
								<< scale[2]
								<< ")\n";
						}
									} break;
					case(0xb00a): {
						log << "KFhdr\n";
						GLushort revision,animlen;
						GLubyte *filename = new GLubyte[length-10];
						instream.read((char *)&revision,2);
						instream.read((char *)filename,length-10);
						instream.read((char *)&animlen,2);
						log << " Revision==" << revision << "\n";
						log << " File name: " << filename << "\n";
						log << " Anim length==" << animlen << "\n";
						delete filename;
									} break;
					default:
						log << "ID=0x" << std::ios::hex << id << std::ios::dec << " len==" << length << "\n";
						instream.ignore(length-6);
				}// switch
			}// while
		}// ReadChunks

		void M3ds::Load(std::istream &instream)
		{
			GLushort id=0x6666;
			GLuint length=12345;

			instream.read((char *)&id,2);
			instream.read((char *)&length,4);
			if(id!=0x4d4d) {
				log << "Not a 3DS file!\n";
				return;
			}
			log << "3DS file of length " << length << "\n";

			ReadChunks(instream,length-6);
			CreateDisplayLists();
		}

	} // namespace Tomas
} // namespace DGL
