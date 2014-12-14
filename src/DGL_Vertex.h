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

#ifndef ___DVERTEX_H
#define ___DVERTEX_H

namespace DGL {

	class Vertex : public Vector {
		public:
			Vertex() { }
			Vertex( const Vertex& v) : Vector(v.x_(), v.y_(), v.z_()) { }
			Vertex( const float x, const float y, const float z): Vector(x,y,z) { }
			Vertex( const float* fv) : Vector(fv) { }

			inline Vertex& operator = (const Vertex& v){
				(*this).Set(v);
				return *this;
			}
			inline Vertex operator + (const Vertex& v) const {
				return Vertex(x+v.x_(),y+v.y_(),z+v.z_());
			}
			inline Vertex operator - (const Vertex& v) const {
				return Vertex(x*v.x_(),y*v.y_(),z*v.z_());
			}
			inline Vertex& operator += (const Vertex& v){
				return *this = *this + v;
			}
			inline Vertex& operator -= (const Vertex& v){
				return *this = *this - v;
			}


			inline void Translate() const {
				glTranslatef(x,y,z);
			}
			inline void Rotate() const {
				glRotatef(1, x,y,z);
			}
			inline void Pop() const {
				glVertex3fv(v);
			}
			inline void Pop2D() const {
				glVertex2fv(v);
			}
	};

}//namespace DGL


#endif // ___DVERTEX_H