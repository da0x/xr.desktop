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

#ifdef WIN32
#include "core_shell.hpp"

#include <cstring>
#include <cstdio>
#include <cstdlib>
//#include <boost/tokenizer.hpp>

//#include <QtGui/QApplication>
//#include <QtGui/QMessageBox>
//#include <QtGui/QWidget>

#define UNICODE
#include <windows.h>
#include <process.h>
#include <shlobj.h>

#include "core_string_cast.hpp"
//#include "core_env.hpp"


namespace xr { namespace core { namespace shell
	{

	bool proc( std::string _Command, bool _Wait )
		{
		STARTUPINFO StartupInfo = {0};
		PROCESS_INFORMATION ProcessInfo;
		wchar_t Cmd[1024];
		std::wcsncpy( Cmd, core::string_cast<std::wstring>( _Command ).c_str(), 1024 );
		BOOL Result = CreateProcess( NULL, Cmd, 0, 0, TRUE, NORMAL_PRIORITY_CLASS, 0, 0, &StartupInfo, &ProcessInfo );

		if( !Result && _Wait )
			{
			DWORD WaitResult = WAIT_TIMEOUT;
			while( WaitResult == WAIT_TIMEOUT )
				{
				WaitResult = WaitForSingleObject( ProcessInfo.hProcess, 100 );
				}
			}

		CloseHandle( ProcessInfo.hProcess );
		CloseHandle( ProcessInfo.hThread );

		return Result == 1 ? true : false;
		}
		/*
	bool proc_as_admin( std::string _Command, std::string _Parameters )
		{
		std::string FullCommand = core::env::application::base() + "\\" + _Command;
		std::wstring Command = core::string_cast<std::wstring>( FullCommand );
		std::wstring Parameters = core::string_cast<std::wstring>( _Parameters );
		
		// Launch itself as administrator
		SHELLEXECUTEINFO sei = {0};

		sei.cbSize = sizeof(SHELLEXECUTEINFOA);
		sei.fMask = 0;
		sei.hwnd = NULL;
		sei.lpVerb = L"runas";
		sei.lpFile = Command.c_str();
		sei.lpParameters = Parameters.c_str();
		sei.nShow = SW_NORMAL;

		if( !ShellExecuteEx(&sei) )
			{
			return false;
            }

		return true;
		}
		*/

	void open( std::string _Command )
		{
		int Result = reinterpret_cast<int>( ShellExecute( HWND_DESKTOP, L"open", core::string_cast<std::wstring>( _Command ).c_str(), NULL, NULL, SW_SHOWNORMAL ) );
		if( Result == SE_ERR_ASSOCINCOMPLETE )
			{ // if association fails. display open with dialog.

				// Windows Vista is Minimum.
//			OPENASINFO Info = { core::string_cast<std::wstring>( _Command ).c_str(), 0 };
//			SHOpenWithDialog( 0, &Info );
			}
		}

	void copy( std::string _Src, std::string _Dst )
		{
		SHFILEOPSTRUCT FileOp = {0};

		std::wstring Src = core::string_cast<std::wstring>( _Src ) + L'\0';
		std::wstring Dst = core::string_cast<std::wstring>( _Dst ) + L'\0';

		FileOp.lpszProgressTitle	= L"Installing Instant Replicator Portable";
		FileOp.pFrom				= Src.c_str();
		FileOp.pTo					= Dst.c_str();
		FileOp.wFunc				= FO_COPY;
		FileOp.hwnd					= GetForegroundWindow();

		SHFileOperation( &FileOp );
		}

	} /* execute */ } /* execute */ } /* execute */

#endif // WIN32