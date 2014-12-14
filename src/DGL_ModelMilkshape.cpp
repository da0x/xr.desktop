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
#include "DGL_Extensions.h"
#include "DGL_Shader.h"
#include "DGL_Model.h"
#include "DGL_ModelMilkshape.h"
#include <fstream>

namespace DGL
	{

	ModelMilkshape::ModelMilkshape()
		{
		}

	ModelMilkshape::~ModelMilkshape()
		{
		}

	/* 
		MS3D STRUCTURES
	*/

	// byte-align structures
	#ifdef _MSC_VER
	#	pragma pack( push, packing )
	#	pragma pack( 1 )
	#	define PACK_STRUCT
	#elif defined( __GNUC__ )
	#	define PACK_STRUCT	__attribute__((packed))
	#else
	#	error you must byte-align these structures with the appropriate compiler directives
	#endif

	typedef unsigned char	byte;
	typedef unsigned short	word;

	// File header
	struct MS3DHeader
		{
		char m_ID[10];
		int m_version;
		} PACK_STRUCT;

	// Vertex information
	struct MS3DVertex
		{
		byte m_flags;
		float m_vertex[3];
		char m_boneID;
		byte m_refCount;
		} PACK_STRUCT;

	// Triangle information
	struct MS3DTriangle
		{
		word m_flags;
		word m_vertexIndices[3];
		float m_vertexNormals[3][3];
		float m_s[3], m_t[3];
		byte m_smoothingGroup;
		byte m_groupIndex;
		} PACK_STRUCT;

	// Material information
	struct MS3DMaterial
		{
		char m_name[32];
		float m_ambient[4];
		float m_diffuse[4];
		float m_specular[4];
		float m_emissive[4];
		float m_shininess;		// 0.0f - 128.0f
		float m_transparency;	// 0.0f - 1.0f
		byte m_mode;			// 0, 1, 2 is unused now
		char m_texture[128];
		char m_alphamap[128];
		} PACK_STRUCT;

	//	Joint information
	struct MS3DJoint
		{
		byte m_flags;
		char m_name[32];
		char m_parentName[32];
		float m_rotation[3];
		float m_translation[3];
		word m_numRotationKeyframes;
		word m_numTranslationKeyframes;
		} PACK_STRUCT;

	// Keyframe data
	struct MS3DKeyframe
		{
		float m_time;
		float m_parameter[3];
		} PACK_STRUCT;

	// Default alignment
	#ifdef _MSC_VER
	#	pragma pack( pop, packing )
	#endif

	#undef PACK_STRUCT

	bool ModelMilkshape::LoadModelData( std::string filename )
		{
		LogPrint("Loading Milkshape3D Model %s.",filename.c_str());
		this->modelname = filename;

		std::ifstream inputFile( filename.c_str(), std::ios::in | std::ios::binary /*| ios::nocreate*/ );
		if ( !inputFile )
			{
			LogPrint("Error Loading Milkshape3D Model: File Not Found %s.", filename.c_str());
			return false;	// "Couldn't open the model file."
			}

		inputFile.seekg( 0, std::ios::end );
		long fileSize = inputFile.tellg();
		inputFile.seekg( 0, std::ios::beg );

		LogPrint("Allocating %d For Model Data", fileSize);

		byte *pBuffer = new byte[fileSize];
		inputFile.read( (char *)pBuffer, fileSize );
		inputFile.close();

		const byte *pPtr = pBuffer;
		MS3DHeader *pHeader = ( MS3DHeader* )pPtr;
		pPtr += sizeof( MS3DHeader );

		LogPrint("HeaderID: %s", pHeader->m_ID);
		LogPrint("Version: %d", pHeader->m_version);

		if ( strncmp( pHeader->m_ID, "MS3D000000", 10 ) != 0 )
			{
			LogPrint("Error Loading Milkshape3D Model: Not a valid Milkshape3D model file.");
			return false; // "Not a valid Milkshape3D model file."
			}


		if ( pHeader->m_version < 3 || pHeader->m_version > 4 )
			{
			LogPrint("Error Loading Milkshape3D Model: Unhandled file version. Only Milkshape3D Version 1.3 and 1.4 are supported.");
			return false;
			}

		int nVertices		= *( word* )pPtr; 
		this->numVertices	= nVertices;
		this->vertices		= new Model::Vertex[nVertices];
		pPtr				+= sizeof( word );

		int i;
		for ( i = 0; i < nVertices; i++ )
			{
			MS3DVertex *pVertex = ( MS3DVertex* )pPtr;
			this->vertices[i].boneID = pVertex->m_boneID;
			memcpy( this->vertices[i].location, pVertex->m_vertex, sizeof( float )*3 );
			pPtr += sizeof( MS3DVertex );
			}

		int nTriangles		= *( word* )pPtr;
		this->numTriangles	= nTriangles;
		this->triangles		= new Model::Triangle[nTriangles];
		pPtr				+= sizeof( word );

		for ( i = 0; i < nTriangles; i++ )
			{
			MS3DTriangle *pTriangle = ( MS3DTriangle* )pPtr;
			int vertexIndices[3] = { pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2] };
			float texCoordT_fixed[3] = { 1.0f-pTriangle->m_t[0], 1.0f-pTriangle->m_t[1], 1.0f-pTriangle->m_t[2] };

			memcpy( this->triangles[i].vertexNormals, pTriangle->m_vertexNormals, sizeof( float )*3*3 );
			memcpy( this->triangles[i].texCoordS, pTriangle->m_s, sizeof( float )*3 );
			memcpy( this->triangles[i].texCoordT, texCoordT_fixed, sizeof( float )*3 );
			memcpy( this->triangles[i].vertexIndices, vertexIndices, sizeof( int )*3 );
			pPtr += sizeof( MS3DTriangle );
			}

		int nGroups		= *( word* )pPtr;
		this->numMeshes	= nGroups;
		this->meshes	= new Model::Mesh[nGroups];
		pPtr			+= sizeof( word );

		for ( i = 0; i < nGroups; i++ )
			{
			pPtr += sizeof( byte );	// flags
			pPtr += 32;				// name

			word nTriangles = *( word* )pPtr;
			pPtr += sizeof( word );
			int *pTriangleIndices = new int[nTriangles];

			for ( int j = 0; j < nTriangles; j++ )
				{
				pTriangleIndices[j] = *( word* )pPtr;
				pPtr += sizeof( word );
				}

			char materialIndex = *( char* )pPtr;
			pPtr += sizeof( char );
		
			this->meshes[i].materialIndex = materialIndex;
			this->meshes[i].numTriangles = nTriangles;
			this->meshes[i].triangleIndices = pTriangleIndices;
			}

		int nMaterials		= *( word* )pPtr;
		this->numMaterials	= nMaterials;
		this->materials		= new Model::Material[nMaterials];
		pPtr				+= sizeof( word );

		for ( i = 0; i < nMaterials; i++ )
			{
			MS3DMaterial *pMaterial = ( MS3DMaterial* )pPtr;
			memcpy( this->materials[i].ambient, pMaterial->m_ambient, sizeof( float )*4 );
			memcpy( this->materials[i].diffuse, pMaterial->m_diffuse, sizeof( float )*4 );
			memcpy( this->materials[i].specular, pMaterial->m_specular, sizeof( float )*4 );
			memcpy( this->materials[i].emissive, pMaterial->m_emissive, sizeof( float )*4 );
		
			this->materials[i].shininess		= pMaterial->m_shininess;
			this->materials[i].textureFilename	= new char[strlen( pMaterial->m_texture ) + 1];
			strcpy( this->materials[i].textureFilename, pMaterial->m_texture );

			pPtr += sizeof( MS3DMaterial );
			}

		delete[] pBuffer;
		
		this->calculateRadius();

		LogPrint("\tFound : %d Vertex.",	this->numVertices);
		LogPrint("\tFound : %d Triangle.",	this->numTriangles);
		LogPrint("\tFound : %d Material.",	this->numMaterials);
		LogPrint("\tFound : %d Meshes.",	this->numMeshes);

		// load the shaders
		this->InitShaders( filename);

		this->loaded = true;
		LogPrint("Model Loaded Successfully");
		return true;
		}
}//namespace DGL
