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


#ifndef ___DSYS_VAR_H
#define ___DSYS_VAR_H

#include <string>

namespace DSys {

	//----------- Console Variables ---------//

	class Var {
		protected:
			std::string		name;			// var name
			std::string		val;			// value buffer
			int				ival;			// integer value
			float			fval;			// float value
			std::string		sval;			// string value
			int				flags;			// flags
			Var				*next;
			
			Var(); // protected default constructor
		public:
			
			Var( std::string name, std::string val);	// default flags
			Var( std::string name, std::string val, int flags); // custom flags
			virtual ~Var();
		
			void update();			// called when update needed	
			void set( std::string name, std::string val); // sets the variable but no update takes place
			void setval( std::string val);					// sets the value but no update takes place
			bool compare(std::string vname);				// compares with var name

			// int value
			inline int	intval()	{ return this->ival; }
			// float value
			inline float floatval()	{ return this->fval; }
			// string value
			inline std::string strval()	{ return this->val; }
			// name value
			inline std::string vname()	{ return this->name; }
			// flags
			inline int vflags()		{ return this->flags; }

			/*
			==============================================================
			List of the variables to get easy console access
			==============================================================
			*/
			protected:
				static Var		*variables;
				static Var		*ptr;

				static void ListRegisterVariable(Var *);
				static void ListUnRegisterVariable(Var *);
			public:
				static void ListUpdateAll( void );					// updates all variable
				static void ListDump(char *);					// dumps the variables into a file
				static bool ListSet(std::string name, std::string value);
				static bool ListGet(std::string name, char *value);
				static void ListEchoToConsole( void );
			


			// flags
			enum Flags
			{
				None,					// no flags
				Archive		= 0x0001,	// set to cause it to be saved to file on dump
				NoSet		= 0x0002,	// don't allow change from console at all,
										// but can be set from the command line
				Latch		= 0x0004,	// save changes until engine restart
				User		= 0x0008,	// user defined variable that can be deleted in run-time
			};
	};

	// boolean variable
	class Var_bool: public Var
	{
	public:
		Var_bool( std::string name, std::string val)
			: Var(name,val){ }	// default flags
		Var_bool( std::string name, std::string val, int flags)
			: Var(name,val,flags){ } // custom flags
		// operators
		inline bool operator == ( int i){ return i == this->ival;}
		inline bool operator != ( int i){ return i != this->ival;}
		inline bool operator !  () { return this->ival ? false : true; }
		inline operator bool	() { return this->ival ? true : false; }
	};

	// int variable
	class Var_int: public Var
	{
	public:
		Var_int( std::string name, std::string val)
			: Var(name,val){ }	// default flags
		Var_int( std::string name, std::string val, int flags)
			: Var(name,val,flags){ } // custom flags
		// operators
		inline bool operator == ( int i){ return i == this->ival;}
		inline bool operator != ( int i){ return i != this->ival;}
		inline bool operator !  () { return this->ival ? false : true; }
		inline operator int() { return this->ival; }
	};

	// float variable
	class Var_float: public Var
	{
	public:
		Var_float( std::string name, std::string val)	// default flags
			: Var(name,val){ }	// default flags
		Var_float( std::string name, std::string val, int flags)
			: Var(name,val,flags){ } // custom flags
		// operators
		inline bool operator == ( float f){ return f == this->fval;}
		inline bool operator != ( float f){ return f != this->fval;}
		inline bool operator == ( int i){ return i == this->ival;}
		inline bool operator != ( int i){ return i != this->ival;}
		inline bool operator !  () { return this->fval ? false : true; }
		inline operator float() { return this->fval; }
	};

	// string variable
	class Var_string: public Var
	{
	public:
		Var_string( std::string name, std::string val)	// default flags
			: Var(name,val){ }	// default flags
		Var_string( std::string name, std::string val, int flags)
			: Var(name,val,flags){ } // custom flags
		// operators
		inline bool operator !		() { return this->sval[0] ? false : true; }
		inline operator bool		() { return this->sval[0] ? true : false; }
		inline operator std::string () { return this->sval; }
		inline operator const char *() { return this->sval.c_str(); }
	};
}

#endif // ___DSYS_VAR_H