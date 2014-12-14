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

#ifndef ___DE_TD_GAME_H
#define ___DE_TD_GAME_H

#include "DE_TD.h"

namespace TD
	{
	namespace game
		{
		//////////////////////////////////////////////////////
		/// entity: Game Entity
		class entity: public shared::entity
			{			
			public: entity(std::string _Classname);
					virtual ~entity();

					void update();
					void link();
					void unlink();
			};		

		class frog : public entity 
			{
			public:	frog();
					void update();
					bool is_leaping() const;
					void leap(Vector& _Direction);

			protected: bool		dead;
					   bool		leaping;
					   float	leapTime;
			};

		class messile: public entity
			{
			public: messile( const Vector& pos, const Vector& vel) : entity("messile")
						{
						this->position = pos;
						this->velocity = vel;					
						}
			};

		class messile_launcher
			{
			public: void init( Vector& _Position, Vector& _Direction, float _Speed);
					void update();					
					void destroy();

					void launch_new();
					bool touches_frog( const Vector& _FrogPos);

			private: std::list<messile *>	messiles;
					 Vector					position;
					 Vector					direction;
					 float					speed;
			};

		class messile_launcher_list
			{
			public: void init();
					void update();
					void destroy();
					bool messile_touches_frog( const Vector& _FrogPos );

			private: static const int	numOfLaunchers = 6;
					 messile_launcher	launchers[numOfLaunchers];

					 float				speeds[numOfLaunchers];
			};

		// game client
		class client: public frog
			{
			public: void init( const Vector& _Position, const Vector& _Direction )
						{
						this->position = _Position;
						this->direction = _Direction;
						}
					
					enum client_command
						{
						leap_left,
						leap_right,
						leap_front,
						leap_back,
						};

					void send_command( client_command _Command )
						{
						switch( _Command )
							{
							case leap_left:		return this->leap( Vector(-1, 0, 0) );
							case leap_right:	return this->leap( Vector( 1, 0, 0) );
							case leap_front:	return this->leap( Vector( 0,-1, 0) );
							case leap_back:		return this->leap( Vector( 0, 1, 0) );
							}
						}
			};

		// game server
		class server
			{
			private:messile_launcher_list	*launchers;
					

//					HeighestScore			heighestScore;

					bool					gameOver;
					int						lives;
					int						level;
					float					addPrev;
					float					dieWait;
					bool					newRecordNotified;

			public:
				void Init()
					{
					Random::Randomize();
					
					}

				void StartNewRound()
					{
					global::levelTime = 0;

					}

				void Update()
					{
					// update game entities
					this->launchers->update();
					}

				void ClientThink( client* _Client )
					{
                    if( global::timeout <= global::levelTime )
						{
						gameServer->send_client_command( game_server::client_commmand::timeout );
						}

					if( this->launchers.messile_touches_frog( frog.get_position() ) )
						{
						_Client->send_command( client::client_commmand::frog_hit );
						}

					// check for new record
					if(this->heighestScore.CheckNewRecord(this->target.GetHitCounter()))
						{
						if(!newRecordNotified)
							{
							gameServer->send_command( game_server::client_commmand::new_record );
							}
						}
					}

				void Destroy()
					{
					}
			};
		}
	}


#endif // ___DE_TD_GAME_H