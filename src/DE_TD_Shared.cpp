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
using namespace TD::shared;

entity::entity(std::string _Classname)
	{
	this->classname = _Classname;
	this->linked	= false;
	}

entity::entity( const entity& _Copy)
	{
	this->radius    = _Copy.radius;
	this->velocity  = _Copy.velocity;
	this->position  = _Copy.position;
	this->direction = _Copy.direction;

	this->classname = _Copy.classname;
	this->linked    = _Copy.linked;
	}

entity& entity::operator = ( const entity& _Right)
	{
	this->radius    = _Right.radius;
	this->velocity  = _Right.velocity;
	this->position  = _Right.position;
	this->direction = _Right.direction;

	this->classname = _Right.classname;
	this->linked    = _Right.linked;
	return *this;
	}

/////////////////////////////////////////////
// Events
using namespace event;

// basic event
basic_event::basic_event( std::string _Classname)
	: classname(_Classname)
	{
	}

// time event
time::time( std::string _Classname, float _TimeTrigger)
	: basic_event(_Classname), timeTrigger(_TimeTrigger)
	{
	fired = false;
	}

bool time::check()
	{
	return this->fired ? true : this->timeTrigger <= global::levelTime;
	}

void time::fire()
	{
	this->fired = true;
	}

// key down
keydown::keydown( std::string _Classname, unsigned char  _Key)
	: basic_event(_Classname), key(_Key)
	{
	}

bool keydown::check()
	{
	return DSys::Input::KeyDown( key );
	}

// key char
keychar::keychar( std::string _Classname, unsigned char  _Key)
	: basic_event(_Classname), key(_Key)
	{
	}

bool keychar::check()
	{
	return DSys::Input::KeyDown( key );
	}
