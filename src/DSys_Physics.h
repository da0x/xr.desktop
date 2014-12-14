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

namespace DSys { namespace Physics
	{

	namespace dynamic
		{
		class item
			{
			public:
				struct type { enum type_e
					{
					box,
					sphere
					}; };

				item(): actor(0),jointL(0),jointR(0),dragOn(false)
					{}
				void init(Vector Location, Vector Dimensions);
				void init( Vector Location, float Radius );

				void matrix( float* Mat ) const; //4x4 matrix
				void drag( Vector Location );
				void drag_off();
				void resize( Vector Dimensions );
				Vector position() const;
//				Vector pose() const;
				bool hung() const;
				Vector hung_vector() const;
				float speed() const;
				void identity();

				void hang( Vector Location );

				int actor_type() const { return this->actorType; }
			public:
				void*	actor;
				int		actorType;
				void*	jointL;
				void*	jointR;
				Vector	dragLocal;
				Vector	dragInitial;
				bool	dragOn;

				Vector	hangVector;
			};

		class ball : public item
			{
			public: 
			};
		}

	namespace kinetic
		{
		class item
			{
			public:
				void init(Vector Location, Vector Dimensions);
				void matrix( float* Mat ) const; //4x4 matrix
				void drag( Vector Location );
				void drag_off();
			public:
				void*	actor;
				Vector	dragLocal;
				bool	dragOn;
			};
		}

	void Init();
	void CreatePlane( Vector Normal, float D );
	void Update( float Msec );
	void Render();
	void Shutdown();

	} /* Physics */ } /* DSys */

