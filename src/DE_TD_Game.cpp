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

#include "DShared.h"
#include "DE_TD.h"

using namespace TD;
using namespace TD::game;

//////////////////////////////////
// entity
entity::entity(std::string _Classname)
	: shared::entity(_Classname)
	{
	TD::gameServer->entityServer.registerEntity( this );
	}

virtual entity::~entity()
	{
	TD::gameServer->entityServer.unregisterEntity( this );
	}

void entity::update()
	{
	this->position += this->velocity * global::msec;
	}

void entity::link()
	{
	this->linked = true;
	}

void entity::unlink()
	{
	this->linked = false;
	}

//////////////////////////////////
// frog
frog::frog()
	: entity("frog")
	{
	this->dead = false;
	this->position.Set(0,0,-constant::frogRadius);
	this->direction.Set(0,0,-1);
	this->leaping = false;
	}

void frog::update()
	{
	this->entity::update();
	if(this->leaping)
		{
		if( (global::levelTime - this->leapTime) >= constant::leapDuration )
			{ // stop the frog
			this->leaping = false;
			this->velocity.clear();
			}
		}
	// clip
	if( this->position.x_() > constant::maxX || this->position.x_() < constant::minX )
		{
		Clamp( constant::minX, constant::maxX, this->position.x_())
		this->velocity *= -1;
		}
	}

bool frog::is_leaping() const
	{
	return this->leaping;
	}

void frog::leap(Vector& _Direction)
	{// should fail if already leaping or dead...
	this->direction = _Direction;
	if(!this->leaping)
		{
		this->velocity = this->direction.normal() * global::leapSpeed;
		this->leapTime = global::levelTime;
		this->leaping = true;
		}
	}

//////////////////////////////////
// messile
messile::messile( const Vector& pos, const Vector& vel)
	: entity("messile")
	{
	this->position = pos;
	this->velocity = vel;					
	}

//////////////////////////////////
// messile launcher
void messile_launcher::init( Vector& _Position, Vector& _Direction, float _Speed)
	{
	this->position = _Position;
	this->direction = _Direction;
	this->speed = _Speed;
	}

void messile_launcher::launch_new()
	{
	this->messiles.push_back( new messile(this->position, this->direction.normal() * this->speed ) );
	}

void messile_launcher::update()
	{
	// first update the messiles
	list_type::iterator iter;
	for( iter = this->messiles.begin(); iter != this->messiles.end(); iter++)
		{
		messile *msl = *iter;

		msl->update();
		if( msl->position.x_() > constant::maxZ || msl->position.x_() < constant::minZ ||
			msl->position.y_() > constant::maxZ || msl->position.y_() < constant::minZ	)
			{// kill this one
			this->messiles.erase(--iter);
			delete msl;
			}
		}
	}

bool messile_launcher::touches_frog( const Vector& _FrogPos)
	{
	list_type::iterator iter;
	for( iter = this->messiles.begin(); iter != this->messiles.end(); iter++)
		{
		if( ( (*iter)->position - _FrogPos ).length() < ( global::messileRadius + constant::frogRadius ) )
			{
			return true;
			}
		}
	return false;
	}

void messile_launcher::destroy()
	{
	for( list_type::iterator iter = this->messiles.begin(); iter != this->messiles.end(); ++iter )
		{
		delete *iter;
		}
	this->messiles.clear();
	}

//////////////////////////////////
// messile launcher list

void messile_launcher_list::init()
	{
	speeds[0] = speeds[this->numOfMessiles -1 ] = global::minMessileSpeed;
	speeds[1] = speeds[this->numOfMessiles -2 ] = global::minMessileSpeed +	
		( global::maxMessileSpeed - global::minMessileSpeed ) / 2.0f;
	speeds[2] = speeds[this->numOfMessiles -3 ] = global::maxMessileSpeed;

	for( int i=0; i< numOfLanuchers; i++)
		{
		Vector pos( ( i % 2 )? constant::maxX : constant::minX, global::messileRadius + (i * (global::messileRadius * 2 )), 0);
		Vector dir( ( i % 2 )? -1 : 1 , 0 , 0);
		this->launchers[i].init(pos, dir, this->speeds[i]);
		}
	}

void messile_launcher_list::update()
	{
	for( int i=0; i< numOfLanuchers; i++)
		{
		this->launchers[i].update();
		global::levelTime  / global::launchFrequency;
		}
	}

void messile_launcher_list::destroy()
	{
	for( int i=0; i< numOfLanuchers; i++)
		{
		this->launchers[i].destroy();
		}
	}

bool messile_launcher_list::messile_touches_frog( const Vector& _FrogPos )
	{
	for( int i=0; i< numOfLanuchers; i++)
		{
		if( this->launchers[i].touches_frog( _FrogPos ) )
			{
			return true;
			}
		}
	return false;
	}