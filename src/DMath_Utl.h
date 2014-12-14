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

#ifndef ___DMATH_MAIN_H
#define ___DMATH_MAIN_H


namespace DMath 
{

	static const float Pi = 3.14159265358979323846f;
	
	inline float DegToRad( float &a );
	inline float RadToDeg( float &a );

	/*------------------
		Clamp
	------------------*/

	template <class Type> inline static Type Clamp ( Type min, Type max, Type &val);

	/*------------------
		Min
	------------------*/

	template <class Type> inline static Type Min ( Type a, Type b );

	/*------------------
		Max
	------------------*/

	template <class Type> inline static Type Max ( Type a, Type b );

	/*------------------
		Interpolate
	------------------*/

	template <class Type> inline static Type Interpolate ( Type, Type, float);
	template <class Type> inline static Type Interpolate2( Type, Type, Type, float);
	template <class Type> inline static Type Interpolate3( Type, Type, Type, Type, float);

	/*------------------
		Swap
	------------------*/

	template <class Type> inline static void Swap (Type v1,Type v2);

	/*------------------
		Abs
	------------------*/

	template <class Type> inline static Type Abs( Type v );

	/*--------------------
		IsPowerOfTwo
	--------------------*/

	inline static bool IsPowerOfTwo( int v);

	/*-----------------------
		ClosestPowerOfTwo
	-----------------------*/

	inline static int ClosestPowerOfTwo(int v);

} // namespace DMath

#endif // ___DMATH_MAIN_H