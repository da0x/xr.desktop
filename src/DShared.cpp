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

#include <windows.h>
#include <cstdio>
#include <cstring>
#include <process.h>
#include <cstdlib>
#include <fstream>
#include "DShared.h"

/*
===============================================================
                String Functions
===============================================================
*/

/* String length */
size_t D_strlen(DCSTR s){
	int i=0;
	while(s[i++] && i< MAX_DSTRING);
	return (size_t)(i);
}

/* String copy */
void D_strcpy(DSTR dest, DCSTR source){
	int i=0;
	while(source[i] && i< MAX_DSTRING){
		dest[i]= source[i++];
	}
	dest[i]= 0;
}

/* String upper case */
void D_strupr(DSTR str){
	int i=0;
	while(str[i] && i< MAX_DSTRING){
		if(is_lower(str[i]))
			str[i]-=0x0020;
		i++;
	}
}

/* String lower case */
void D_strlwr(DSTR str){
	int i=0;
	while(str[i] && i< MAX_DSTRING){
		if(is_upper(str[i]))
			str[i]+=0x0020;
		i++;
	}
}
/* String compare */
int chcmp(char a, char b){
	return a-b;
}
int chcmpi(char a, char b){
	if(is_lower(a)) a-=0x0020;
	if(is_lower(b)) b-=0x0020;
	return a-b;
}
int D_strcmp(DSTR a, DSTR b){
	int i=0;
	int diff;
	while(a[i]||b[i]){
		if(diff= chcmp(a[i],b[i]) != 0)
			return diff;
		i++;
	}
	return 0;
}
int D_strcmpi(DCSTR a, DCSTR b){
	int i=0;
	int diff;
	while(a[i]||b[i]){
		if(diff= chcmpi(a[i],b[i]) != 0)
			return diff;
		i++;
	}
	return 0;
}

void D_ReplaceChar(char *s, const char sour, const char dest){
	char *temp;
	temp= s;
	while(*temp){
		if(*temp == sour)
			*temp = dest;
		temp++;
	}
}

char *D_Msec2String(float time){
	int		sec;
	int		min;
	int		hrs;
	char	*s;

	time/=1000.0f;
	sec		= int(time) % 60;		time /= 60;
	min		= int(time) % 60;		time /= 60;
	hrs		= int(time) % 60;

	if(hrs)
		s = va("%2d:%2d:%2d", hrs, min, sec);
	else
		s = va("%2d:%2d", min, sec);
	D_ReplaceChar(s, ' ', '0');
	return s;
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char	*  va( char *fmt, ... ) {
	va_list			argptr;
	static char		string[2][32000];	// in case va is called by nested functions
	static int		index = 0;
	char			*buf;

	buf = string[index & 1];
	index++;

	va_start(argptr, fmt);
	vsprintf(buf, fmt,argptr);
	va_end	(argptr);

	return buf;
}

/*--------------------------------
         File Analysis
  --------------------------------*/

bool D_FileExist(DCSTR filename){
	FILE *fp;
	fp= fopen(filename,"r");
	if(fp){
		fclose(fp);
		return true;
	}
	return false;
}

long D_FileLength(FILE *fp){
	long len;
	fseek(fp, 0 , SEEK_END);
	len= ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return len;
}

bool
D_ChangeExt(
	DSTR		fname,
	DSTR		ext
){
	bool	status= false;
	int			i;
	int			j;

	i= D_strlen(fname)-1;
	j= i;
	//make sure an ext exists
	while(j){
		if(fname[j--]=='.'){
			status= true;
			i=j+2;
			break;
		}
	}

	if(!status){
		fname[i++]='.';
	}
	if(!ext){
		//remove the dot
		fname[i-1]=0;
		return true;
	}
	j=0;
	if(ext[j]=='.') // skip the dot
		j++;
	while(ext[j])
		fname[i++]=ext[j++];
	fname[i]=0;
	return true;
}

bool
D_fGetLine(
	FILE	*f,
	char	*string
	)
{
	do
	{
		if(feof(f))	{
			string[0] = (char) 0;
			return false;
		}
		fgets(string, 255, f);
	} while (
		(string[0] == '/') ||
		(string[0] == '\n')/*||
		(string[0] == '{') ||
		(string[0] == '}')*/);
	return true;
}



	long Random::holdrand = 1L;
	



/*--------------------------------------------------------*/
/*-------------------- namespace Daher -------------------*/
/*--------------------------------------------------------*/

namespace Daher {

	/*
	==============================
    
		DSys::Logger::Print Class definition
	
	==============================
	*/

	Logger::Logger(const char *lfname, bool tmp){
		this->temp = tmp;
		this->logfilename = new char [D_strlen(lfname)];
		D_strcpy(this->logfilename, lfname);

		log = fopen( logfilename , "wt");
		if(!log){
			ok= false;
		} else {
			ok= true;
		}
		Printf("---- LOG STARTUP ----\n");
	}
	
	void Logger::Printf( const char *fmt,...){
		if( ok ){
			va_list		argptr;
			DSTR		text;

			va_start( argptr, fmt);
			vsprintf( text, fmt, argptr);
			va_end	( argptr);

			fprintf( log, text);
			fflush( log);
		}
	}

	Logger::~Logger(){
		if( ok ){
			Printf("---- LOG SHUTDOWN ----");
			fclose( log);
			ok= false;
			// we don't need the log remove it
			if( this->temp)
				remove( logfilename);
		}
	}

	/*
	==========================================

		DMultithreading

	==========================================
	*/

	#define WIN32MULTITHREADING  defined( _WIN32 ) && defined( _MT )
	#if WIN32MULTITHREADING
	#	pragma message ("Win32 Multithreading Is Enabled")
	#else
	#	pragma message ("Win32 Multithreading Is Disabled")
	#endif

	//
	// DThreadProc
	//
	static
	DWORD
	WINAPI
	ThreadProc(
			Thread *pThis
	){
		return pThis->ThreadProc();
	}
	
	//
	// Thread::Thread()
	//
	Thread::Thread(){
		ID = 0;
		threadHandle = NULL;
		executing = false;
	}

	//
	// Thread::~Thread()
	//
	Thread::~Thread(){
		End();
	}

	//
	// Thread::Begin()
	//
	void Thread::Begin(){
	#if WIN32MULTITHREADING
		if( threadHandle )
			End();  // just to be safe.
		// Start the thread.
/*		threadHandle = CreateThread( NULL,
						0,
						(LPTHREAD_START_ROUTINE)ThreadProc,
						this,
						0,
						(LPDWORD)&ID );
*/		if( threadHandle == NULL ){
			throw Daher::Exception( "Thread::Begin() - Thread creation failed." );
		}
		executing = true;
	#endif
	}

	//
	// Thread::ThreadProc()
	//
	DWORD Thread::ThreadProc(){
		return 0;
	}

	//
	// Thread::IsExecuting()
	//
	inline bool Thread::IsExecuting() const {
		return executing;
	}
	
	//
	// Thread::End()
	//
	void Thread::End(){
	#if WIN32MULTITHREADING
		if( threadHandle != NULL ){
			executing = false;
			WaitForSingleObject( threadHandle, INFINITE );
			CloseHandle( threadHandle );
			threadHandle = NULL;
		}
	#endif
	}

	//
	// DMonitor::DMonitor()
	//
	Monitor::Monitor(){
	#if WIN32MULTITHREADING
		// This mutex will help the two threads share their toys.
		monitorHandle = CreateMutex( NULL, false, NULL );
		if( monitorHandle == NULL )
			throw Exception( "Monitor::Monitor() - Mutex creation failed." );
	#endif
	}

	//
	// DMonitor::~DMonitor()
	//
	Monitor::~Monitor() {
	#if WIN32MULTITHREADING
		if( monitorHandle != NULL ){
			CloseHandle( monitorHandle );
			monitorHandle = NULL;
		}
	#endif
	}

	//
	// Monitor::MutexOn()
	//
    inline void Monitor::MutexOn() const {
	#if WIN32MULTITHREADING
		WaitForSingleObject( monitorHandle, INFINITE );  // To be safe...
	#endif
	}
	
	//
	// Monitor::MutexOff()
	//
	inline void Monitor::MutexOff() const {
	#if WIN32MULTITHREADING
		ReleaseMutex( monitorHandle );  // To be safe...
	#endif
	}



	/*
	=================================

		CallStackTrace

	=================================
	*/

	#if (defined(_DEBUG) || defined(_DEBUG)) || !defined(NDEBUG)

	CallStackTrace* CallStackTrace::head = 0;
	CallStackTrace* CallStackTrace::tail = 0;
	
	void CallStackTrace::Dump()	{
		std::ofstream file;
		file.open("DCallStackTracer.log");
		Dump(file);
		file.close();
	}
	
	void CallStackTrace::Dump(std::ostream& out){
		out << "*** Begin Call Stack Dump *** \n";
		for(CallStackTrace* r = head;r!=0;r = r->next){
			out << r->funcName << "()[" << r->fileName << ":" << 
				r->lineNo << "]";
			if(r->next)
				out << "->";
			out << "\n";
		}
		out << "*** End Call Stack Dump *** " << std::endl;
	}
	
	void CallStackTrace::Enter(){
		if(head==0)
			head = this;
		if(tail!=0)
			tail->next = this;
		prev = tail;
		tail = this;
	}
	void CallStackTrace::Leave(){
		if(tail->prev)
			tail->prev->next = 0;
		tail = tail->prev;
		prev = next = 0;
	}

	#endif //(defined(_DEBUG) || defined(_DEBUG)) || !defined(NDEBUG)

} // namespace Daher
