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

#ifndef __DMATH_VECTOR_H
#define __DMATH_VECTOR_H

namespace DMath
{

	/*
	================================================
	Vector classes
	================================================
	*/

#	ifndef _vDot
#	  define _vDot ^			// HACK: dot product operator 
#	else
#	  error '_vDot' is already defined!
#	endif

	class Vector {
	public:
		// constructors
		inline Vector();													// constructor
		inline Vector( const Vector& v );									// copy constructor
		inline Vector(const float x, const float y, const float z);			// float values
		inline Vector( const float *v );									// float array
		// copy values from array/ values
		inline void Set(const float x, const float y, const float z);		// classic set
		inline void Set( const float *v );									// copy from array
		// index operator
		inline			float& operator [](int index);						// index operator
		inline const	float& operator [](int index) const;				// index operator
		// float pointer operators
		inline operator			float *();
		inline operator const	float *() const;
		// member access 
		inline float&	x_();
		inline float&	y_();
		inline float&	z_();
		inline float	x_() const;
		inline float	y_() const;
		inline float	z_() const;
		// positive/nigative operators
		inline Vector operator +();											// positive
		inline Vector operator -();											// nigative
		// boolean operators
		inline bool operator == (const Vector& v) const;					// equal?
		inline bool operator != (const Vector& v) const;					// not equal?
		inline		operator bool() const;									// has value?
		inline bool	operator !()	const;									// doesn't have a value?
		// general operators
		inline Vector operator + (const Vector& v) const;					// add
		inline Vector operator - (const Vector& v) const;					// subtract
		inline Vector operator * (const Vector& v) const;					// multiply
		inline Vector operator * (const float s) const;						// scale
		inline Vector operator * (const int s) const;						// scale
		inline Vector operator / (const float s) const;						// 1/scale
		inline Vector operator / (const int s) const;						// 1/scale
		// extended ( reversed )
		inline friend Vector operator*(const float &f, const Vector &v);	// scale ( float * Vector )
		inline friend Vector operator/(const float &f, const Vector &v);	// scale ( float / Vector )
		inline friend Vector operator*(const int &f, const Vector &v);		// scale ( int * Vector )
		inline friend Vector operator/(const int &f, const Vector &v);		// scale ( int / Vector )
		// extended ( general + equal )
		inline Vector& operator  = (const Vector& v);						// equal
		inline Vector& operator -= (const Vector& v);						// -=
		inline Vector& operator += (const Vector& v);						// +=
		inline Vector& operator *= (const Vector& v);						// *= Vector
		inline Vector& operator *= (const float s);							// *= float
		inline Vector& operator /= (const float s);							// /= float
		inline Vector& operator *= (const int s);							// *= int
		inline Vector& operator /= (const int s);							// /= int

		// General Vector Operations
		inline float dot(const Vector& v) const;							// dot product		
		inline float operator _vDot (const Vector& v) const;				// dot product ( use _vDot )

		inline float length() const;										// length
		inline float lengthsquared() const;									// length (fast)

		inline float distance(const Vector& v) const;						// distance from this to v
		inline float distancesquared(const Vector& v) const;				// distance from this to v (fast)

		inline Vector&	normalize();										// reduce this to unit
		inline Vector	normal() const;										// get the unit

		inline char *string() const;										// convert to string

		inline Vector& clear();												// clear (TODO: replace by (*= 0)

		// instances for known normals
		inline static Vector vzero();	// zero vector
		inline static Vector xnorm();	// x normal
		inline static Vector ynorm();	// y normal
		inline static Vector znorm();	// z normal

	public:
		union
		{
			struct
			{
				float	x,
					y,
					z;
			};
			float		v[3];
		};
	};
} // namespace DMath

#endif // __DMATH_VECTOR_H