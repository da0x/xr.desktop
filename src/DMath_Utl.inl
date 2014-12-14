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

	inline float DegToRad( float &a )
		{
		return a *= Pi / 180.0f;
		}

	inline float RadToDeg( float &a )
		{
		return a *= 180.0f / Pi;
		}

	/*------------------
		Clamp
	------------------*/

	template <class Type>
	inline
	static
	Type Clamp ( 
		Type min,
		Type max,
		Type &val)
		{ 
		return val = val < min ? min : val > max ? max : val;
		}


	/*------------------
		Min
	------------------*/

	template <class Type>
	inline
	static
	Type Min ( Type a, Type b )
		{
		return a > b ? b : a;
		}

	/*------------------
		Max
	------------------*/

	template <class Type>
	inline
	static
	Type Max ( Type a, Type b )
		{
		return a < b ? b : a;
		}

	/*------------------
		Interpolate
	------------------*/

	template <class Type>
	inline
	static
	Type Interpolate (
				Type n1,
				Type n2,
				float ratio)
		{
			Type out;
			const float m_ratio = 1 - ratio;
				out = n1 * m_ratio + n2 * ratio;
			return out;
		}

	template <class Type>
	inline
	static
	Type Interpolate2 (
				Type p1,
				Type p2,
				Type p3,
				float ratio)
		{
		float			ratio_p2;
		float m_ratio,	m_ratio_p2;

		m_ratio = 1 - ratio;
		m_ratio_p2 = m_ratio * m_ratio;
		ratio_p2 = ratio * ratio;

		return (Type) (p1*m_ratio_p2 + p2*2*ratio*m_ratio + p3*ratio_p2);
		}

	template <class Type>
	inline
	static
	Type Interpolate3 (
				Type p1,
				Type p2,
				Type p3,
				Type p4,
				float ratio)
		{
		float			ratio_p3;
		float m_ratio,	m_ratio_p3;

		m_ratio = 1 - ratio;
		m_ratio_p3 = m_ratio * m_ratio * m_ratio;
		ratio_p3 = ratio * ratio * ratio;

		return (Type) (p1*m_ratio_p3 + p2*3*ratio*m_ratio*m_ratio + p3*3*ratio*ratio*m_ratio + p4*ratio_p3);
		}

	/*------------------
		Swap
	------------------*/

	template <class Type>
	inline
	static
	void Swap (Type v1,Type v2)
		{
		Type v = v1;
		v1 = v2;
		v2 = v;
		}

	/*------------------
		Abs
	------------------*/

	template <class Type>
	inline
	static
	Type Abs( Type v )
		{
		return ( ( v >= 0 ) ? v : -v );
		}

	/*--------------------
		IsPowerOfTwo
	--------------------*/

	inline
	static
	bool IsPowerOfTwo( int v)
		{
		if( v < 0 )
			return false;
		int p = 1;
		while ( p != v )
			{
			if( p > v )
				return false;
			p *= 2;
			}
		return true;
		}


	/*-----------------------
		DClosestPowerOfTwo
	-----------------------*/

	inline static int ClosestPowerOfTwo(int v)
		{
		if(IsPowerOfTwo(v))
			return v;

		int p,_p;
		p = 1;
		while ( p < v )
			{
			_p = p;
			p *= 2;
			}
		return ( p );// for lower size: Abs<int>(p-v) < Abs<int>(_p-v) ? p : _p );
		}
