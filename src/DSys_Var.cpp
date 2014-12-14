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

#include <cstdlib>
#include <cmath>
#include <string>
#include "DSys_Var.h"
#include "DSys_File.h"
#include "DSys_Logger.h"

/*
TODO List:
- ...
*/

namespace DSys {
	/*
	===================================

	Var

	===================================
	*/

	Var::Var(){
//		this->name		= "";
//		this->val		= "";
		this->ival		= 0;
		this->fval		= 0;
//		this->sval		= 0;
		this->next		= 0;
		this->flags		= 0;
	}

	Var::Var( std::string name, std::string val)
	{
		Var();
		this->name = name;
		this->val = val;
		this->flags = Archive;

		ListRegisterVariable(this);
	}

	Var::Var( std::string name, std::string val, int flags)
	{
		Var();
		this->name	= name;
		this->val	= val;
		this->flags = flags;

		ListRegisterVariable(this);
	}

	Var::~Var()
	{
		Var::ListUnRegisterVariable(this);
	}

	// called when an update is needed or when *_restart command is called if a latched var
	void Var::update()
	{
		this->ival = atoi(this->val.c_str());
		this->fval = atof(this->val.c_str());
		this->sval = this->val;
	}

	// sets the variable but no update takes place
	void Var::set( std::string name, std::string val)
	{
		this->name = name;
		this->val = val;
	}

	// sets the variable but no update takes place
	void Var::setval( std::string val)
	{
		this->val = val;
	}

	// compares with the variable's name
	bool Var::compare( std::string vname)
	{
		//return this->name == vname;
		return _strcmpi( this->name.c_str(), vname.c_str());
	}

	/*----------------
	Static functions
	----------------*/

	Var		*Var::variables = NULL;
	Var		*Var::ptr;

	// sets a variable's value in the list
	bool Var::ListSet( std::string name, std::string value)
	{
		if(!Var::variables)
			return false; // list is empty

		Var::ptr = Var::variables;
		while(Var::ptr)
		{			
			if(!Var::ptr->compare( name))
			{
				if(Var::ptr->vflags() & NoSet){
					Logger::Error("Access Denied to '%s' (write protected)", Var::ptr->vname());
					return true; // operation was correctly executed
				}
				Var::ptr->setval(value);
				if(!(Var::ptr->vflags() & Latch))
					Var::ptr->update();
				else
					Logger::Print("%s will be changed upon restarting...", Var::ptr->vname());

				return true;
			}
			Var::ptr= Var::ptr->next;
		}
		return false;
	}

	// gets a variable's value from the list
	bool Var::ListGet(std::string name, char *value)
	{
		if(!Var::variables)
			return false; // empty list

		Var::ptr = Var::variables;
		while(Var::ptr)
		{
			if(!Var::ptr->compare( name) )
			{
				strcpy( value, Var::ptr->sval.c_str());
				return true;
			}

			Var::ptr = Var::ptr->next;
		}
		return false;
	}

	// updates all objects
	void Var::ListUpdateAll()
	{
		if(!Var::variables)
			return;

		Var::ptr =Var::variables;
		while(Var::ptr)
		{
			Var::ptr->update();
			Var::ptr = Var::ptr->next;
		}
	}

	// echos list of Var::variables into the console
	void Var::ListEchoToConsole()
	{
		if(!Var::variables)
			return;

		Logger::Print("Searching Variable List ...");
		Logger::Print("a = Archive variable.");
		Logger::Print("n = Noset variable.");
		Logger::Print("l = Latch variable.");
		Logger::Print("u = User variable.");
		Logger::Print("--------------------");
		Logger::Print(" a n l u : cvarname");
		Var::ptr = Var::variables;
		while(Var::ptr)
		{
			Logger::Print(" %i %i %i %i : %s",
				Var::ptr->vflags() & (1 << 0),
				Var::ptr->vflags() & (1 << 1),
				Var::ptr->vflags() & (1 << 2),
				Var::ptr->vflags() & (1 << 3),
				Var::ptr->vname().c_str());
			Var::ptr = Var::ptr->next;
		}
	}

	// registers a variable
	void Var::ListRegisterVariable(Var *var)
	{
		Logger::Print("Registering Variable: '%s'",var->vname().c_str());

		if(!Var::variables)
		{
			Var::variables = var;
			return;
		}
		
		Var::ptr = Var::variables;
		while(Var::ptr->next)
		{			
			if(Var::ptr == var)
				return; // already registered

			Var::ptr = Var::ptr->next;
		}
		Var::ptr->next = var;
	}

	// unregisters a variable
	void Var::ListUnRegisterVariable(Var *var)
	{
		if(!Var::variables)
			return;

		if( var == Var::variables )
		{
			Var::variables = var->next;
			return;
		}

		Var::ptr = Var::variables;
		while(Var::ptr)
		{
			if(Var::ptr->next == var)
			{
				Var::ptr = var->next;
				return;
			}
			Var::ptr = Var::ptr->next;
		}
	}

	// dumps list into a file
	void Var::ListDump(char *fname)
	{
		file_t file;
		file = fs_open(fname, "wt");
		if(!file)
		{
			Logger::Error("Connot open for writing: file '%s'",fname);
			return;
		}
		fs_writeln("// Generated by Daher Engine.\n\n", file);
		Var::ptr = Var::variables;
		while(Var::ptr)
		{
			if(Var::ptr->vflags() & Archive)
				fs_writeln("set %s %s", file, Var::ptr->vname().c_str(), Var::ptr->strval().c_str());
			Var::ptr= Var::ptr->next;
		}
		fs_close(file);
	}
}// namespace DSys