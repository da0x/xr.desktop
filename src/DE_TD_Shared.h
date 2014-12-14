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

#ifndef ___DE_TD_SHARED_H
#define ___DE_TD_SHARED_H

namespace TD
	{
	namespace shared
		{
		class entity
			{
			public: entity( std::string _Classname);
					entity( const entity& _Copy);
					entity& operator = ( const entity& _Right);

			protected: std::string		classname;

					   float			radius;
					   Vector			position;
					   Vector			direction;
					   Vector			velocity;

					   bool				linked;
			};

		namespace event
			{
			
			class basic_event
				{
				public: basic_event( std::string _Classname);
						virtual bool check() = 0;
						virtual void fire() = 0;

				private: std::string		classname;
				};

			class time: public basic_event
				{
				public: time( std::string _Classname, float _TimeTrigger);
						virtual bool check();
						virtual void fire();

				protected: float timeTrigger;
						   bool fired;
				};
											
			class keydown: public basic_event
				{
				public: keydown( std::string _Classname, unsigned char  _Key);
						virtual bool check();

				protected: unsigned char key;
				};

			class keychar: public basic_event
				{
				public: keychar( std::string _Classname, unsigned char  _Key);
						virtual bool check();

				protected: unsigned char key;
				};
			}// event
		}// shared
	}// TD


#endif // ___DE_TD_H