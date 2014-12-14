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

#ifndef ___DGENTEXTURE_H
#define ___DGENTEXTURE_H

/*
TODO List:
- ...
*/


namespace DGL {

	class GenTexture {
		protected:
			int			dimx,dimy;
			GLuint		Texture;

		private:
			GLuint EmptyTexture(){
				GLuint txtnumber;
				unsigned int* data;

				data = (unsigned char*)new GLuint[((dimx * dimy)* 4 * sizeof(unsigned char))];

				ZeroMemory(data,((dimx * dimy)* 4 * sizeof(unsigned char)));

				glGenTextures(1, &txtnumber);
				glBindTexture(GL_TEXTURE_2D, txtnumber);
				glTexImage2D(GL_TEXTURE_2D, 0, 4, dimx, dimy, 0,
					GL_RGBA, GL_UNSIGNED_BYTE, data);
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				delete [] data;

				return txtnumber;
			}
		public:
			GenTexture(int dimx, int dimy){
				this-> dimx = dimx;
				this-> dimy = dimy;
			}
			void ViewOrtho(){
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				glOrtho( 0, DSys::sv_iScreenWidth , DSys::sv_iScreenHeight, 0, -1, 1 );
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
			}

			void ViewPerspective(){
				glMatrixMode( GL_PROJECTION );
				glPopMatrix();
				glMatrixMode( GL_MODELVIEW );
			}

			void BeginTrap(){
				glViewport( 0, 0, dimx, dimy);
			}
			void EndTrap(){
				glBindTexture(GL_TEXTURE_2D,Texture);
				glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, dimx, dimy, 0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glViewport(0 , 0, sv_iScreenWidth.floatval(), sv_iScreenHeight.floatval());
			}
			inline GLuint GetTexture(){
				return Texture;
			}
	};
}//namespace DGL

#endif // ___DGENTEXTURE_H