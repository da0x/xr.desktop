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

#ifndef ___DGL_CAMERA_H
#define ___DGL_CAMERA_H


namespace DGL {
		
	//////////////////////////////
	// Camera
		class Camera {
			private:
				Vector		eye;
				Vector		direction;
				Vector		up;

				mutable float matrix[16];

			public:
				Camera():
						eye(0,0,0),
						direction(0,0,-1),
						up(0,1,0)
				{
				}
						
				inline void Setup() const
				{
					Vector x;
					Vector y;
					Vector z;


					// Make rotation matrix

					// Z vector
					z = direction * -1;
					// Y vector
					y = Vector(0,1,0);
					// X vector = Y cross Z
					x = y * z;
					// Recompute Y = Z cross X
					y = z * x;
					// mpichler, 19950515
					// cross product gives area of parallelogram, which is < 1.0 for
					// non-perpendicular unit-length vectors; so normalize the vectors
					static_cast<void>( x.normalize() );
					static_cast<void>( y.normalize() );
					static_cast<void>( z.normalize() );

					#define _M(row,col)  matrix[col*4+row] 
						_M(0, 0) = x.x_(); 
						_M(0, 1) = x.y_(); 
						_M(0, 2) = x.z_(); 
						_M(0, 3) = 0.0; 
						_M(1, 0) = y.x_(); 
						_M(1, 1) = y.y_(); 
						_M(1, 2) = y.z_(); 
						_M(1, 3) = 0.0; 
						_M(2, 0) = z.x_(); 
						_M(2, 1) = z.y_(); 
						_M(2, 2) = z.z_(); 
						_M(2, 3) = 0.0; 
						_M(3, 0) = 0.0; 
						_M(3, 1) = 0.0; 
						_M(3, 2) = 0.0; 
						_M(3, 3) = 1.0;
					#undef _M 


					glLoadIdentity();
					glMultMatrixf(matrix);					
					// Translate Eye to Origin
					glTranslatef(-eye.x_(), -eye.y_(), -eye.z_()); 
				}

				// Moves up according to camera coords
				inline void MoveCameraUp(float f)
				{
					eye += up.normal() * f;
				}

				// Moves right according to camera coords
				inline void MoveCameraRight(float f)
				{
					eye += ( direction * up ).normal() * f;
				}

				// Moves farward according to camera coords
				inline void MoveCameraFarward(float f)
				{
					eye += direction.normal() * f;
				}

				inline void MovePlaneUp(float f)
				{
					eye += Vector::ynorm() * f;
				}

				inline void MovePlaneRight(float f)
				{
					Vector right = direction * up;
					right.y_() = 0.0f; // reset to horizantal plane
					right.normalize();
					eye += right * f;
				}

				inline void MovePlaneFarward(float f)
				{
					Vector dir = direction;
					dir.y_() = 0.0f; // reset to horizantal plane
					dir.normalize();
					eye += dir * f;
				}

				inline void RotateUp(float f)
				{
					direction.normalize();
					up.normalize();
					direction	= direction * (float)cos(f) + up * (float)sin(f);
					up			= up * (float)cos(f) - direction * (float)sin(f);
				}

				inline void RotateRight(float f)
				{
					Vector right;
					float dir_y = direction.y_();
					right = direction * up;
					right.normalize();
					direction.normalize();
					direction		= direction * (float)cos(f) + right * (float)sin(f);
					direction.y_()	= dir_y;
					// recalculate the up vector
					right = direction * Vector(0,1,0);
					up = right * direction;
				}

				enum Param {
					POSITION,
					DIRECTION,
					FOCUS,
				};

				inline void Set(Param param, Vector v)
				{
					switch (param){
						case POSITION:
							{
								eye = v;
								break;
							}
						case DIRECTION: 
							{
								direction = v;
								Vector right = direction * Vector(0,1,0);
								up = right * direction;
								break;
							}
						case FOCUS:
							{
								direction = v - eye;
								Vector right = direction * Vector(0,1,0);
								up = right * direction;
								break;
							}
					}
				}

				inline void Get(Param param, Vector &v) const
				{
					switch (param){
						case POSITION:	v = eye;		break;
						case DIRECTION: v = direction;	break;
					}
				}
		};

}//namespace DGL

#endif // ___DGL_CAMERA_H