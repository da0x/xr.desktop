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

#ifndef ___MATERIAL_H
#define ___MATERIAL_H


// Undone!



namespace DGL {

	class Material {
		public:
			Material() :	emission(0.0f, 0.0f, 0.0f, 1.0f),
							ambient(0.2f, 0.2f, 0.2f, 1.0f),
							diffuse(0.8f, 0.8f, 0.8f, 1.0f),
							specular(0.0f, 0.0f, 0.0f, 1.0f),
							shininess(0.0f),
							face(GL_FRONT)
			{
			}
			Material(Material &mat) :	emission(mat.emission),
										ambient(mat.ambient),
										diffuse(mat.diffuse),
										specular(mat.specular),
										shininess(mat.shininess),
										face(mat.face)
			{
			}
			Material(
					Color Emission,
					Color Ambient,
					Color Diffuse,
					Color Specular,
					float Shininess,
					bool doubleFace) :	emission(Emission),
										ambient(Ambient),
										diffuse(Diffuse),
										specular(Specualr),
										shininess(Shininess),
										face(doubleFace ? GL_FRONT_AND_BACK : GL_FRONT)
			{
			}
			Material& operator = (Material& other)
			{
				this->emission = other.emission;
				this->ambient = other.ambient;
				this->diffuse = other.diffuse;
				this->specular = other.specular;
				this->shininess = other.shininess;
				this->face = other.face;
				return *this;
			}

			// sets up opengl to this material properties
			void Setup() const
			{
				glMaterialfv(this->face, GL_EMISSION, this->emission);
				glMaterialfv(this->face, GL_AMBIENT, this->ambient);
				glMaterialfv(this->face, GL_DIFFUSE, this->diffuse);
				glMaterialfv(this->face, GL_SPECULAR, this->specular);
				glMaterialf(this->face, GL_SHININESS, this->shininess );
			}

			// resets opengl material state
			static void _Reset()
			{
				// default values taken from the red book
				glMaterialfv(GL_FRONT, GL_AMBIENT, Color(0.2f, 0.2f, 0.2f, 1.0f));
				glMaterialfv(GL_FRONT, GL_DIFFUSE, Color(0.8f, 0.8f, 0.8f, 1.0f));
				glMaterialfv(GL_FRONT, GL_SPECULAR, Color(0.0f, 0.0f, 0.0f, 1.0f));
				glMaterialf(GL_FRONT, GL_SHININESS, float(0) );
			}

			enum Param {
				EMISSION,
				AMBIENT,
				DIFFUSE,
				AMBIENT_AND_DIFFUSE,
				SPECULAR,
				SHININESS
			};

			void Set(Param param, const Color& c)
			{
				switch (param)
				{
					case SHININESS:
						this->emission = c;
						break;
					case AMBIENT:
						this->ambient = c;
						break;
					case DIFFUSE:
						this->diffuse = c;
						break;
					case AMBIENT_AND_DIFFUSE:
						this->ambient = c;
						this->diffuse = c;
						break;
					case SPECULAR:
						this->specular = c;
						break;
				}
			}

			void Set(Param param, float f)
			{
				switch (param)
				{
					case SHININESS:
						this->shininess = f;
						break;
				}
			}

			enum HintParam {
				SINGLE_FACE,
				DOUBLE_FACE
			};

			void Hint(HintParam hint)
			{
				switch (hint)
				{
					case SINGLE_FACE:
						face = GL_FRONT;
						break;
					case DOUBLE_FACE:
						face = GL_FRONT_AND_BACK:
						break;
				}
			}

		private:
			Color	emission;
			Color	ambient;
			Color	diffuse;
			Color	specular;
			float	shininess;
			GLenum	face;
	};

}//namespace DGL


#endif // ___MATERIAL_H