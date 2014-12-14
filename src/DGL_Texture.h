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

#ifndef ___DGL_TEXTURE_H
#define ___DGL_TEXTURE_H

namespace DGL {
	/*
	=============================================================

		TEXTURES

	=============================================================
	*/

	class Texture {
		public:
			/////////////////
			// Member Classes
			class Filter;

			/////////////////
			// Public Functions: Build
			void Build( const char *filename, Filter = auxDefaultFilter );
			void BuildIcon( const char * filename, Filter glFilter = auxDefaultFilter );
			void Delete();

			/////////////////
			// Screen Shot
			static void ScreenShot();

			/////////////////
			// operator GLuint
			inline operator GLuint () const
			{
				return this->glTexture;
			}

			inline const GLuint* operator &() const
				{
				return &this->glTexture;
				}

			inline Texture& operator = ( const Texture& right)
				{
				this->glTexture = right.glTexture;
				return *this;
				}

			Texture() : owner(false), glTexture(0)
				{
				}


		public:

			static void Init();

			/////////////////////////////////////
			// Filter : filter defining object
			class Filter {
				public:

					struct FilterName
						{
						enum
							{
							NEAREST		= 0x0001,
							LINEAR		= 0x0002,
							BILINEAR	= 0x0003,
							TRILINEAR	= 0x0004
							};
						};

					Filter(int FilterName = auxDefault, bool noRepeat = false);
					Filter(Filter&);

						/***************************************************\
						/  SetParametrs :									\
						/	Supported Parameters and there default values	\
  						/	GL_TEXTURE_MAG_FILTER							\
						/	GL_TEXTURE_MIN_FILTER							\
						/	GL_TEXTURE_WRAP_S								\
						/	GL_TEXTURE_WRAP_T								\
						/***************************************************/

					void SetParameter(GLenum target, GLenum pname, GLint param);
					GLint GetParameter(GLenum pname);

					///////////////////////////
					// Use : sets the parameters to opengl's current texture
					void Use() const;

					///////////////////////////
					// IsMipmap : Filter requires using mipmaps or normal texture.
					bool IsMipmap() const;

					///////////////////////////
					// auxDefault : Default filter
					static const int auxDefault = FilterName::TRILINEAR;
				private:
					bool	textureGenMipmap;
					int		textureMagFilter;
					int		textureMinFilter;
					int		textureWarpS;
					int		textureWarpT;
			};

	private:
		bool			owner;
		GLuint			glTexture;
		std::string 	path;
		static	Filter	auxDefaultFilter;
	};
}

#endif // ___DGL_FRUSTUM_H
