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
#ifndef ___DCOLLISION_H
#define ___DCOLLISION_H

namespace DGL {
	
	namespace Collision {

		class Ray {
			public:
				Ray() { }
				Ray(const Ray& ray): Start(ray.Start), Direction(ray.Direction) { }
				Vector	Start;
				Vector	Direction;

				Vector PointOnRay(float f){
					return Start + Direction * f;
				}
		};

		class Plane {
			public:
				Vector	normal;
				Vector	Position;

				bool TestIntersection(const Ray& ray){
				}
		};

		class Cylinder {
			public:
				Ray		Axis;
				float	Radius;

				bool TestIntersection(const Ray& ray){
				}
		};

		class Object {
			public:
				Object() : next(NULL) { }

			private:
				void Link(){
					Register(this);
				}
				void Unlink(){
					Unregister(this);
				}
				virtual void Init() = 0;
				virtual void Render() = 0;
				virtual void Update(float msec) = 0;
				virtual bool CollisionDetect() = 0;
				virtual void DoCollision() = 0;


			private:
				Object *next;
				static Object *list;
				static void Register(Object *obj){
					Object *o = list;
					while(o)
						o = o->next;
					o = obj;
				}
				static void Unregister(Object *obj){
					Object *o = list,*p;
					while(o){
						p = o;
						o = o->next;
						if(obj == o){
							p = o->next;
							o->next = NULL;
							return;
						}
					}// if not then obj is already unregistered...
				}
				static void CheckCollision(Object *list){
					Object *o = list;
					while(o){
						if(list->CollisionDetect(o)){
							list->DoCollision(o);
							o->DoCollision(list);
						}
						o = o->next;
					}
				}
			public:
				static void InitAll(float msec){
					Object *o = list;
					while(o){
						o->Init();
						o = o->next;
					}
				}
				static void UpdateAll(float msec){
					Object *o = list;
					while(o){
						CheckCollision(o);
						o->Update();
						o = o->next;
					}
				}
				static void RenderAll(float msec){
					Object *o = list;
					while(o){
						o->Render();
						o = o->next;
					}
				}
		};

	}// namespace Collision

}// namespace DGL

#endif // ___DCOLLISION_H