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

#pragma once
#ifndef ___DBALL_H
#define ___DBALL_H

namespace DGL {


		class Ball {
			public:
				Ball() : next(NULL) { }

			protected:
				float			radius;
				Vector	position;
				Vector	velocity;
				Vector	angles;
				Vector	angle_velocity;
				float			mass;

				void accelerate(
							float sec)
				{
					const int 		gravity = -120;
					const Vector	accel(0,gravity,0);
					// first decelerate the y value
					velocity[1] += gravity * sec;						// v= v0 + a*t
					position += velocity * sec +  Vector(0,gravity,0) * sec * sec * 0.5f; // dx= v0*t+0.5*a*t*t                    
				}

				bool collisionDetect(
							Ball *o)
				{
					if( o == this ) return false; // of course :)

					// no advanced algo or anything...
					// simply I will check the the balles does not intersect
					if( ( this->position - o->position ).length() <= this->radius + o->radius )
						return true;
					return false;
				}
				void doCollision(
							Ball *o)
				{
					// collision has already been detected with the object o. so we move this back.
					this->position = o->position+ (this->position - o->position).normal() * (this->radius+o->radius);

					// only velocity along the distance between the balls
					// will be affected. the tangential velocity wont... the rate is (m/m+m1)
					Vector thisNormal;
					Vector objNormal;

					thisNormal = (this->position - o->position).normal();
					objNormal = (o->position - this->position).normal();

					float thisFactor = this->mass / (o->mass + this->mass);
					float objFactor = 1-thisFactor;

					float totalVelocity = this->velocity.length() + o->velocity.length();

					thisNormal *= thisFactor * totalVelocity;
					objNormal *= objFactor * totalVelocity;

					this->velocity += thisNormal;
					o->velocity += objNormal;
				}

				
				virtual void render() = 0;
				virtual void update(float msec) = 0;
				virtual void onCollision() = 0;
			public:
				void Link(){
					_register(this);
				}
				void Unlink(){
					_unregister(this);
				}
				void Launch(float			radius,
							Vector	position,
							Vector	velocity,
							Vector	angles,
							Vector	angle_velocity,
							float			mass)
				{
					this->radius = radius;
					this->position = position;
					this->velocity = velocity;
					this->angles = angles;
					this->angle_velocity = angle_velocity;
					this->mass = mass;
					this->Link();
				}
				


				
			private:
				Ball		*next;
				static Ball *list;
				static void _register(Ball *obj){
					Ball *o = list;

					if(!list){
						list = obj;
						return;
					}

					while(o->next){
						if(o->next == obj) return;
						o = o->next;
					}

					o->next = obj;
				}
				static void _unregister(Ball *obj){
					Ball *o = list;
					if(!list) return;
					if(list == obj){
						if(list->next)
							list = list->next;
						else
							list = NULL;
						obj->next = NULL;
						return;
					}

					while(o->next){
						if(obj == o->next){
							o = obj->next;
							obj->next = NULL;
							return;
						}
                        o = o->next;
					}
				}
				static void checkCollision(Ball *list){
					Ball *o = list;
					while(o){
						if(list->collisionDetect(o)){
							list->doCollision(o);
							list->onCollision();
							o->onCollision();
						}
						o = o->next;
					}
				}
			public:
				static void UpdateAll(float msec){
					Ball *o = list;
					while(o){
						checkCollision(o);
						o->update(msec);
						o = o->next;
					}
				}
				static void RenderAll(){
					Ball *o = list;
					while(o){
						o->render();
						o = o->next;
					}
				}
		};

}// namespace DGL


#endif // ___DBALL_H