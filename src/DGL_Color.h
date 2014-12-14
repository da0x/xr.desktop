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

#ifndef ___DCOLOR_H
#define ___DCOLOR_H

namespace DGL {

	class Color {
		public:
			union {
				struct {
					float r,g,b,a;
				};
				float v[4];
			};
		
			Color() { }
			Color( const Color& c) : r(c.r), g(c.g), b(c.b), a(c.a) { }
			Color(		const float red,
						const float green,
						const float blue,
						const float alpha = 1) :
				r(red),
				g(green),
				b(blue),
				a(alpha)
			{ }
			Color( const float *v): r(v[0]), g(v[1]), b(v[2]), a(v[3]) { }
			explicit Color(	const unsigned char red,
							const unsigned char green,
							const unsigned char blue,
							const unsigned char alpha = 255) :
				r(red /255.0f),
				g(green /255.0f),
				b(blue /255.0f),
				a(alpha /255.0f)
			{ }

			inline void Set( const float r, const float g, const float b, const float a = 1){
				this->r = r;
				this->g = g;
				this->b = b;
				this->a = a;
			}
			inline void Set( const float *v){
				this->v[0] = v[0];
				this->v[1] = v[1];
				this->v[2] = v[2];
				this->v[3] = v[3];
			}
			inline void Set(
					const unsigned char red,
					const unsigned char green,
					const unsigned char blue,
					const unsigned char alpha = 255)
			{
				this->r = red / 255.0f;
				this->g = green / 255.0f;
				this->b = blue / 255.0f;
				this->a = alpha / 255.0f;
			}

			inline Color& operator = ( const Color& c){
				this->r = c.r;
				this->g = c.g;
				this->b = c.b;
				this->a = c.a;
				return *this;
			}

			inline bool operator == ( const Color& c) const {
				return ( r == c.r && g == c.g && b == c.b && a == c.a );
			}

			inline bool operator != ( const Color& c) const {
				return ! ( *this == c );
			}

			inline Color operator -() const {
				return Color(1-r, 1-g, 1-b, 1-a);
			}

			inline Color operator + ( const Color& c) const {
				return Color( r + c.r , g + c.g , b + c.b , a + c.a );
			}

			inline Color operator * ( const Color& c) const {
				return Color( r * c.r , g * c.g , b * c.b , a * c.a );
			}

			inline Color operator * ( const float s) const {
				return Color( r * s, g * s, b * s, a * s );
			}

			inline Color& operator *= ( const Color& c){
				return ( *this = *this * c );
			}

			inline Color& operator *= ( const float s){
				return ( *this = *this * s );
			}

			inline Color& operator += ( const Color& c){
				return ( *this = *this + c );
			}

			inline float& operator [](int index){
				return this->v[index];
			}

			inline const float& operator [](int index) const{
				return this->v[index];
			}

			inline operator float * ()
			{
				return this->v;
			}

			inline operator const float*() const {
				return this->v;
			}

			inline Color CombineWith ( const Color& c , float ratio) const {
				return Interpolate<Color>(*this, c, ratio);
			}

			inline void MakeCurrent() const {
				glColor4fv(v);
			}

			inline void MakeCurrentRGB() const {
				glColor3fv(v);
			}

			static Color ColorBlack			(){ return Color(0.00f, 0.00f, 0.00f, 1.00f); }
			static Color ColorWhite			(){ return Color(1.00f, 1.00f, 1.00f, 1.00f); }
			static Color ColorYellow		(){ return Color(1.00f, 1.00f, 0.00f, 1.00f); }
			static Color ColorBlue			(){ return Color(0.00f, 0.00f, 1.00f, 1.00f); }
			static Color ColorCyan			(){ return Color(0.50f, 0.75f, 1.00f, 1.00f); }
			static Color ColorGreen			(){ return Color(0.00f, 1.00f, 0.00f, 1.00f); }
			static Color ColorLightOrange	(){ return Color(1.00f, 0.68f, 0.00f, 1.00f); }
			static Color ColorOrange		(){ return Color(1.00f, 0.43f, 0.00f, 1.00f); }
			static Color ColorRed			(){ return Color(1.00f, 0.00f, 0.00f, 1.00f); }
			static Color ColorLightRed		(){ return Color(1.00f, 0.20f, 0.20f, 1.00f); }
			static Color ColorDim			(){ return Color(0.00f, 0.00f, 0.00f, 0.25f); }
			static Color ColorGold			(){ return Color(1.00f, 0.8395314f, 0.4488235f, 1.00f); }
			static Color ColorGray			(){ return Color(0.50f, 0.50f, 0.50f, 0.50f); }

	};


}//namespace DGL


#endif // ___DCOLOR_H