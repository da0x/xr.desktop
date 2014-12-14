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

#ifndef ___DE_TD_H
#define ___DE_TD_H

#include <list>
#include "DSys.h"

#include "de_td_shared.h"
#include "de_td_game.h"
#include "de_td_client.h"

namespace TD
	{
	namespace constant
		{
		const static int minX = -10;
		const static int maxX = 10;
		const static int minZ = 0;
		const static int maxZ = 450;
		
		const static int leapDuration = 300; // msec
		const static int frogRadius   = 5;
		}

	namespace global
		{ // changes according to level...
		float			msec;
		float			levelTime;
		float			leapSpeed;
		float			launchFrequency;			// messiles per second...
		float			messileRadius;
		float			maxMessileSpeed;
		float			minMessileSpeed;

		struct game_state
			{
			bool			gameOver;
			int				lives;
			int				level;
			float			addPrev;
			float			dieWait;
			bool			newRecordNotified;
			};
		}
	
	class main_server
		{

		public: void init()
					{
					}
				void update(float msec)
					{
					global::msec	= msec;
					global::levelTime += msec;
					}
				void render()
					{
					}
				void destroy()
					{
					}
				void start_new_game()
					{
					}

				

				class entity_server
					{
					typedef class shared::entity  entity;

					public: void registerEntity( entity* _Entity)
								{
								this->entityList.push_back( _Entity );
								}
							void unregisterEntity( entity* _Entity )
								{
								this->entityList.remove_if( std::bind2nd( std::equal_to<entity*>(), _Entity ) );
								}

							void update()
								{
								}

					private: std::list<entity*>		entityList;

					friend class game_server;
					};

				class event_server
					{
					typedef shared::event::basic_event event;
					public: void registerEvent( event* _Event)
								{
								this->eventList.push_back( _Event );
								}
							void unregisterEvent( event* _Event)
								{
								this->eventList.remove_if( std::bind2nd( std::equal_to(), _Event ) );
								}

							void update()
								{
								}

					private: std::list<event*>  eventList;

					friend class game_server;
					};

				enum server_commmand
					{
					game_over,		// game over msg
					};

				void send_command( int );

				

				class heighest_score {
					private:
						int		numOfGames;
						int		scoreRecord;
					public:
						heighest_score() : numOfGames(0), scoreRecord(0) { }
						void Init()
							{
							FILE *f = fopen("game.dat","rb");
							if(!f)	return;
							fread(this,sizeof(*this),1,f);
							fclose(f);
							}
						void new_game_started()
							{
							numOfGames++;
							}
						bool check_new_record(int _Hits){
							if( _Hits > this->scoreRecord )
								{
								scoreRecord = _Hits;
								if( numOfGames > 1 )	// don't notify new score in the first game.
									return true;
								}
							return false;
							}
						void destroy()
							{
							FILE *f = fopen("game.dat","wb");
							if(!f)
								{
								DSys::Logger::Error("Cannot save game data");
								return;
								}
							fwrite(this,sizeof(*this),1,f);
							fclose(f);
							}
						int get_num_of_games()	{ return numOfGames;	}
						int get_score_record()	{ return scoreRecord;	}
				};

                
		public: class entity_server  entityServer;
		public: class event_server  eventServer;

		private: class game::client  gameClient;
		private: class game::server  gameServer;

		} *mainServer;
	}


#endif // ___DE_TD_H