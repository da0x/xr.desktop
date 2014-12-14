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

#ifndef ___DGL_MATRIX_H
#define ___DGL_MATRIX_H


namespace DGL {
	
	class Matrix {
		public:
			/////////////////////
			// Data
			float matrix[16];


			///////////////////////
			// Constructors creates an identity matrix by default
			inline Matrix()	
			{
				for(unsigned int c=0;c<4;c++)
					for(unsigned int r=0;r<4;r++)
						if(c==r)
							this->matrix[r+c*4]=1.0;
						else
							this->matrix[r+c*4]=0.0;
			}
			inline Matrix(const Matrix& mat)
			{
				this->Set(mat.matrix);
			}
			inline Matrix(const float *mat)
			{
				Set(mat);
			}

			//////////////////////
			// operator =
			inline Matrix& operator = (const Matrix& mat)
			{
				this->Set((float *)&mat);
				return *this;
			}

			//////////////////////
			// Set
			inline void Set(const float *mat)
			{
				for(unsigned int i=0;i<16;i++)
					this->matrix[i]=mat[i];
			}

			//////////////////////
			// Multiply to opengl current matrix
			inline void MultGL() const
			{
				glMultMatrixf(this->matrix);
			}
	};

}// namespace DGL

#endif // ___DGL_MATRIX_H