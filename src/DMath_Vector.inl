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

	inline Vector::Vector()
	{
	}
	inline Vector::Vector( const Vector& v ) 
	{
		this->x = v.x_();
		this->y = v.y_();
		this->z = v.z_();
	}
	inline Vector::Vector(const float x, const float y, const float z) 
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}	
	inline Vector::Vector( const float *v )
	{
		this->x = v[0];
		this->y = v[1];
		this->z = v[2];
	}
	inline void Vector::Set(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	inline void Vector::Set( const float *v )
	{
		this->x = v[0];
		this->y = v[1];
		this->z = v[2];
	}
	// member access 
	inline float& Vector::x_()
	{
		return static_cast<float&>(this->x);
	}
	inline float& Vector::y_()
	{
		return static_cast<float&>(this->y);
	}
	inline float& Vector::z_()
	{
		return static_cast<float&>(this->z);
	}
	inline float Vector::x_() const
	{
		return this->x;
	}
	inline float Vector::y_() const
	{
		return this->y;
	}    
	inline float Vector::z_() const
	{
		return this->z;
	}
	inline Vector Vector::operator +()
	{// positive
		return *this;
	}
	inline Vector Vector::operator -()
	{// negative
		static Vector ans;
		ans.x_() = -this->x;
		ans.y_() = -this->y;
		ans.z_() = -this->z;
		return ans;
	}
	inline bool Vector::operator == (const Vector& v) const
	{// equal?
		return ( this->x == v.x_() && this->y == v.y_() && this->z == v.z_() );
	}
	inline bool Vector::operator != (const Vector& v) const
	{// not equal?
		return ( this->x != v.x_() || this->y != v.y_() || this->z != v.z_() );
	}
	inline Vector::operator bool() const
	{// has value?
		if( this->length() )
			return true;
		return false;
	}
	inline bool Vector::operator !()	const
	{// doesn't have a value?
		return ! static_cast<bool>(*this);
	}
	inline Vector Vector::operator + (const Vector& v) const
	{// vector add
		static Vector ans;
		ans.x_() = this->x + v.x_();
		ans.y_() = this->y + v.y_();
		ans.z_() = this->z + v.z_();
		return ans;
	}
	inline Vector Vector::operator - (const Vector& v) const
	{// vector subtract
		static Vector ans;
		ans.x_() = this->x - v.x_();
		ans.y_() = this->y - v.y_();
		ans.z_() = this->z - v.z_();
		return ans;
	}
	inline Vector Vector::operator * (const Vector& v) const
	{// cross product
		static Vector cross;
		cross.x_() = this->y * v.z_() - this->z * v.y_();
		cross.y_() = this->z * v.x_() - this->x * v.z_();
		cross.z_() = this->x * v.y_() - this->y * v.x_();
		return cross;
	}
	inline Vector Vector::operator * (const float s) const
	{// scale
		static Vector ans;
		ans.x_() = this->x * s;
		ans.y_() = this->y * s;
		ans.z_() = this->z * s;
		return ans;
	}
	inline Vector Vector::operator * (const int s) const
	{// scale
		static float _s;
		_s = static_cast<const float>(s);
		return *this * _s;
	}	// scale
	inline Vector Vector::operator / (const float s) const
	{
		static Vector ans;
		ans.x_() = this->x / s;
		ans.y_() = this->y / s;
		ans.z_() = this->z / s;
		return ans;
	}
	inline Vector Vector::operator / (const int s) const
	{
		static float _s;
		_s = static_cast<const float>(s);
		return *this / _s;
	}
	inline Vector operator*(const float &f, const Vector &v)
	{// scale ( float * vector )
		return v*f;
	}
	inline Vector operator/(const float &f, const Vector &v)
	{// scale ( float / Vector )
		return Vector( f/v.x_(), f/v.y_(), f/v.z_() );
	}
	inline Vector operator*(const int &i, const Vector &v)
	{// scale ( int * Vector )
		return v*i;
	}
	inline Vector operator/(const int &i, const Vector &v)
	{// scale ( int / Vector )
		return Vector( i/v.x_(), i/v.y_(), i/v.z_() );
	}
	inline Vector& Vector::operator = (const Vector& v)
	{// equal
		this->x = v.x_();
		this->y = v.y_();
		this->z = v.z_();
		return *this;
	}
	inline Vector& Vector::operator -= (const Vector& v)
	{
		this->x -= v.x_();
		this->y -= v.y_();
		this->z -= v.z_();
		return *this;
	}
	inline Vector& Vector::operator += (const Vector& v)
	{
		this->x += v.x_();
		this->y += v.y_();
		this->z += v.z_();
		return *this;
	}
	inline Vector& Vector::operator *= (const Vector& v)
	{
		*this = *this * v;
		return *this;
	}
	inline Vector& Vector::operator *= (const float s)
	{
		this->x *= s;
		this->y *= s;
		this->z *= s;
		return *this;
	}
	inline Vector& Vector::operator /= (const float s)
	{
		this->x /= s;
		this->y /= s;
		this->z /= s;
		return *this;
	}
	inline Vector& Vector::operator *= (const int s)
	{// *= int
		this->x *= s;
		this->y *= s;
		this->z *= s;
		return *this;
	}
	inline Vector& Vector::operator /= (const int s)
	{// /= int
		this->x /= s;
		this->y /= s;
		this->z /= s;
		return *this;
	}	
	inline float& Vector::operator [](int index)
	{// index
		return this->v[index];
	}
	inline const float& Vector::operator [](int index) const
	{// const index
		return this->v[index];
	}
	inline Vector::operator float *() 
	{// array ptr
		return this->v;
	}
	inline Vector::operator const float *() const
	{
		return this->v;
	}
	inline float Vector::dot(const Vector& v) const
	{// dot product
		static float ans;
		ans = this->x * v.x_() + this->y * v.y_() + this->z * v.z_();
		return ans;
	}
	inline float Vector::operator _vDot (const Vector& v) const
	{
		return this->dot(v);
	}
	inline float Vector::lengthsquared() const
	{// length (fast)
		return static_cast<float> (x*x + y*y + z*z);
	}
	extern "C" double ::sqrt( double );
	inline float Vector::length() const
	{// length
		return static_cast<float> ( sqrt( this->lengthsquared() ) );
	}
	inline float Vector::distancesquared(const Vector& v) const
	{// distance from this to v (fast)
		static Vector answer;
		answer = *this - v;
		return answer.lengthsquared();
	}		
	inline float Vector::distance(const Vector& v) const
	{// distance from this to v
		static Vector answer;
		answer = *this - v;
		return answer.length();
	}	
	inline Vector& Vector::normalize()
	{// reduce to unit
		static float len;
		len = this->length();
		if(len)
		{
			this->x /= len;
			this->y /= len;
			this->z /= len;
		}
		return *this;
	}
	inline Vector Vector::normal() const
	{
		static Vector ans;
		ans = *this;
		return ans.normalize();
	}	
	inline Vector& Vector::clear()
	{// clear
		this->x= this->y= this->z= 0.0f;
		return *this;
	}

	// normal instances
	inline Vector Vector::vzero(){ return Vector(0,0,0); }
	inline Vector Vector::xnorm(){ return Vector(1,0,0); }
	inline Vector Vector::ynorm(){ return Vector(0,1,0); }
	inline Vector Vector::znorm(){ return Vector(0,0,1); }