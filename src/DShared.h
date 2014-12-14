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

#ifndef ___D_SHARED_H
#define ___D_SHARED_H

#ifndef __cplusplus
#error Must use C++ for Daher Shared Definitions.
#endif

#pragma warning(disable : 4244)		// possible loss of data
#pragma warning(disable : 4267)		// '=' : conversion from 'size_t' to 'int', possible loss of data
#pragma warning(disable : 4800)		// 'int' : forcing value to bool 'true' or 'false' (performance warning)



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/* Declare Most Needed Headers */

#include <iostream>
#include <stdio.h>		// Log class
#include <math.h>		// Mathlib
#include <cstring>		
#include <ctime>		// Rand class
#include <cstdlib>
#include <assert.h>
//#include <exception>

#include "DMath.h"
using namespace DMath;


#define MAX_DSTRING		255
typedef char			DSTR[MAX_DSTRING];
typedef const char		DCSTR[MAX_DSTRING];


/*
======================================
  STRINGS
======================================
*/

#define is_lower(a)	((a)>= 0x0061 && (a)<= 0x007A)
#define is_upper(a)	((a)>= 0x0041 && (a)<= 0x005A)
#define is_alpha(a) (is_lower(a) || is_upper(a))
#define is_number(a) ((a)>= '0' && (a) <= '9' )

  size_t	D_strlen( DCSTR s);
  void		D_strcpy( DSTR dest, DCSTR source);
  void		D_strupr( DSTR str);
  void		D_strlwr( DSTR str);
  int		D_strcmp( DSTR,DSTR);
  int		D_strcmpi( DCSTR,DCSTR);
  char		*  va( char *fmt, ... );
  void		D_ReplaceChar( char *s, const char sour, char dest);
  char		*D_Msec2String( float time);

/*-------------------------------------------------------
           File Analysis
 -------------------------------------------------------*/
bool	D_FileExist(DCSTR fname);
long	D_FileLength(FILE* f);
bool	D_ChangeExt(DSTR fname, DSTR ext= NULL);
bool	D_fGetLine(FILE *f,char *oneline);


/*------------------------------------------------*/
/*---------------- Namespace Daher ---------------*/
/*------------------------------------------------*/


namespace Daher {


	/*
	==========================================

		Logger Class

	==========================================
	*/

	class Logger { 
		private:
			FILE			*log;
			bool			ok;
			bool			temp;
			char			*logfilename;
		public:
			Logger(const char *, bool = false);
			void  Printf(const char *fmt,...);
			inline Logger operator << ( const char *fmt)	{ Printf("%s", fmt);		return *this;}
			inline Logger operator << ( char *fmt)			{ Printf("%s", fmt);		return *this;}
			inline Logger operator << ( bool b)				{ if(b)Printf("(bool)true");else Printf("(bool)false");return *this;}
			inline Logger operator << ( char c)				{ Printf("(char)%c\"%i\"",c, (int)c);		return *this;}
			inline Logger operator << ( int i)				{ Printf("(int)%i",i);		return *this;}
			inline Logger operator << ( float f)			{ Printf("(float)%f",f);	return *this;}
			inline Logger operator << ( double d)			{ Printf("(double)%d",d);	return *this;}
			inline Logger operator << ( Vector v)	{ Printf("(vector)(%f, %f, %f)",v.x_(),v.y_(),v.z_());	return *this;}
            ~Logger();
	};

	/*
	==========================================

		Exception

	==========================================
	*/

	class Exception {
		public:
			Exception() throw() : exceptionMsg("unknown") {  }
			Exception(const char * const & fmt) throw() : exceptionMsg(fmt) {  }
			Exception(Exception& ex) throw() : exceptionMsg(ex.exceptionMsg) {  }
			Exception& operator= (const Exception& ex) throw() {
				this->exceptionMsg = ex.exceptionMsg;
				return *this;
			}
			virtual ~Exception() throw() {  };
			virtual const char *What() const throw() {
				return exceptionMsg ? exceptionMsg : "null";
			}
			virtual void Dorais()/* throw(Exception)*/{
				throw *this;
			}
		private:
			const char	*exceptionMsg;
	};

	class BadException : public Exception {
		public:
			BadException(const char *_msg = "Bad Exception")
				throw() : Exception(_msg) { }
	};

	/*
	==========================================
		Thread / Monitor
	==========================================
	*/
	
	#ifdef _WINDOWS_
	class Thread {
		public:
			Thread();
			virtual ~Thread();
            void Begin();
			void End();
			inline bool IsExecuting() const;
			virtual DWORD ThreadProc();
		protected:
			HANDLE  threadHandle;
			DWORD   ID;
		private:
			bool    executing;
	};

	class Monitor {
		public:
			Monitor();
			virtual ~Monitor();
			inline void MutexOn() const;
			inline void MutexOff() const;
		private:
			HANDLE monitorHandle;
	};
	#endif


	/*
	===========================
		CallStackTrace
	===========================
	*/
	//
	// To enable the stack tracing, at the top of the function (preferrably the 
	// first line of the function), put
	// 	
	// 	DMacro_TraceEnter(functionName)
	// 
	// where functionName is (obviously) the name of the function being traced.
	// Then, at the end of the function, put
	//
	//		DMacro_TraceLeave()
	//
	//	The only function you shouldn't have to put that in is the main() function.
	// Note that no functionName is required.
	//
	// If an error occurs, just call CallStackTrace::Dump() to dump the current call 
	// stack ( with file names and line numbers) to stdout. You can optionally pass a 
	// std::ostream to CallStackTrace::Dump() to dump the stack trace to a file or 
	// other ostream derived object.
	//
	// Catching unhandled exceptions:
	// =================================
	// 
	// To report the call stack when an unhandled exception is thrown, structure your
	// main() function like the following:
	//
	//		main()
	//		{
	//			try {
	//				// run program here
	//			} catch(...) {
	//				cout << "Unhandled Exception!" << endl;
	//				CallStackTrace::Dump();
	//			}
	//		}
	//
	// Handling "Expected" exceptions
	// =================================
	//
	// "But wait!" you say. "What if I handle an exception in a catch() block?
	// Well, in that case, just make the first line of your catch block the
	// DMacro_TraceUnwind() macro (no function name needed, as long as it has a 
	// DMacro_TraceEnter(functionName) at the top of the function).
	// That will unwind the Call Stack Trace, and allow normal functioning
	// from that point forward.

	#if (defined(_DEBUG) || defined(_DEBUG)) || !defined(NDEBUG)
	
	class CallStackTrace {
		private:
			const char*				funcName;
			const char*				fileName;
			int						lineNo;
			CallStackTrace*			prev;
			CallStackTrace*			next;
			static CallStackTrace*	head;
			static CallStackTrace*	tail;
		public:
			CallStackTrace(const char* n,const char* f,int l) : funcName(n), fileName(f), lineNo(l) {}
			void		Enter();
			void		Leave();
			void		Unwind() { next = 0; tail = this;	}

			static void	Dump();
			static void	Dump(std::ostream& out);
	};
	
	class CallStackTracer {
		private:
			CallStackTrace&		trace;
		public:
			CallStackTracer(CallStackTrace& t) : trace(t) { trace.Enter(); }
			~CallStackTracer() { trace.Leave(); }
	};

	// When starting a function
	#define DMacro_TraceEnter(fname) \
				static Daher::CallStackTrace c_CallStackTrace(#fname, __FILE__, __LINE__); \
				c_CallStackTrace.Enter();
	// Before leaving the function
	#define DMacro_TraceLeave() \
				c_CallStackTrace.Leave();
	// After recovering from a thrown exception
	#define DMacro_TraceUnwind() \
				c_CallStackTrace.Unwind();

	// Or use the automatic macro:
	#define DMacro_TRACE_AUTO(fname)	\
				static Daher::CallStackTrace c_CallStackTrace(#fname, __FILE__, __LINE__); \
				Daher::CallStackTracer(&c_CallStackTrace);
	
	// UT2003-style guard/unguard macros

	#else

	#define DMacro_TraceEnter(fname)
	#define DMacro_TraceLeave()
	#define DMacro_TraceUnwind()
	#define DMacro_TRACE_AUTO(fname)

	#endif // (_DEBUG || _DEBUG) || !NDEBUG


} // end namespace Daher

#endif // ___D_SHARED_H