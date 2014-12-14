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

#include "core_shell_contextmenu.hpp"
#include "core_string_cast.hpp"

#include <string>
#include <vector>
#include <utility>
#include <iostream>

#define UNICODE
#include <shobjidl.h>
#include <shlobj.h>
#include <malloc.h>
#include <windows.h>

#pragma warning ( disable: 4311 )	// pointer truncation
#pragma warning ( disable: 4312 )	// conversion from 'A' to 'B' of greater size

namespace xr { namespace core { namespace shell
	{
	namespace global
		{
		static const int	MinId = 1;
		static const int	MaxId = 10000;
		IContextMenu2		*IContext2 = 0;
		IContextMenu3		*IContext3 = 0;

		std::size_t			ItemCount = 0;
		IShellFolder		* ShellFolder;
		LPITEMIDLIST		* PidlArray;

		
		WNDPROC				OldWndProc = 0;
		}

	namespace detail
		{
			// this functions determines which version of IContextMenu is avaibale for those objects
			// and returns the heighest interface supported.
		bool query_interface(void ** ppContextMenu, int & iMenuType)
			{
			*ppContextMenu = 0;
			LPCONTEXTMENU icm1 = 0;
			
			// first we retrieve the normal IContextMenu interface (every object should have it)
			global::ShellFolder->GetUIObjectOf( 0, (UINT)global::ItemCount, (LPCITEMIDLIST *) global::PidlArray, IID_IContextMenu, 0, (void**) &icm1);

			if (icm1)
			{	// since we got an IContextMenu interface we can now obtain the higher version interfaces via that
				if (icm1->QueryInterface (IID_IContextMenu3, ppContextMenu) == NOERROR)
					iMenuType = 3;
				else if (icm1->QueryInterface (IID_IContextMenu2, ppContextMenu) == NOERROR)
					iMenuType = 2;

				if (*ppContextMenu) 
					icm1->Release(); // we can now release version 1 interface, cause we got a higher one
				else 
				{	
					iMenuType = 1;
					*ppContextMenu = icm1;	// since no higher versions were found
				}							// redirect ppContextMenu to version 1 interface
			}
			else
				return (FALSE);	// something went wrong
			
			return (TRUE); // success
			}

		LRESULT CALLBACK window_proc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
			{
			switch (message)
				{ 
				case WM_MENUCHAR:	// only supported by IContextMenu3

					if( global::IContext3 )
						{
						LRESULT lResult = 0;
						global::IContext3->HandleMenuMsg2( message, wParam, lParam, &lResult );
						return( lResult );
						}
					break;

				case WM_DRAWITEM:
				case WM_MEASUREITEM:
					if( wParam ) 
						break; // if wParam != 0 then the message is not menu-related

				  
				case WM_INITMENUPOPUP:
					if( global::IContext2 )
						global::IContext2->HandleMenuMsg( message, wParam, lParam );
					else	// version 3
						global::IContext3->HandleMenuMsg( message, wParam, lParam );
					
						// inform caller that we handled WM_INITPOPUPMENU by ourself
					return message == WM_INITMENUPOPUP ? false : true;

				default:
					break;
				}

				// call original WndProc of window to prevent undefined bevhaviour of window
			return ::CallWindowProc( global::OldWndProc, hWnd, message, wParam, lParam);
			}

		void invoke_command( LPCONTEXTMENU pContextMenu, UINT idCommand )
			{
			CMINVOKECOMMANDINFO cmi = {0};
			cmi.cbSize = sizeof (CMINVOKECOMMANDINFO);
			cmi.lpVerb = (LPSTR) MAKEINTRESOURCE (idCommand);
			cmi.nShow = SW_SHOWNORMAL;

			pContextMenu->InvokeCommand (&cmi);
			}

		UINT GetPIDLSize (LPCITEMIDLIST pidl)
			{  
			if (!pidl) 
				return 0;
			int nSize = 0;
			LPITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;
			while (pidlTemp->mkid.cb)
				{
				nSize += pidlTemp->mkid.cb;
				pidlTemp = (LPITEMIDLIST) (((LPBYTE) pidlTemp) + pidlTemp->mkid.cb);
				}
			return nSize;
			}

		LPITEMIDLIST CopyPIDL( LPCITEMIDLIST pidl, int cb = -1 )
			{
			if (cb == -1)
				cb = detail::GetPIDLSize (pidl); // Calculate size of list.

			LPITEMIDLIST pidlRet = (LPITEMIDLIST) calloc (cb + sizeof (USHORT), sizeof (BYTE));
			if (pidlRet)
				CopyMemory(pidlRet, pidl, cb);

			return (pidlRet);
			}

		void FreePIDLArray(LPITEMIDLIST *pidlArray)
			{
			if (!pidlArray)
				return;

			std::size_t iSize = _msize (pidlArray) / sizeof (LPITEMIDLIST);

			for( std::size_t i = 0; i < iSize; i++ )
				free (pidlArray[i]);
			free (pidlArray);
			}

		void show( HWND _ParentWindowHandle, std::pair<int,int> _Point )
			{ // all goes here.
			
				// to know which version of IContextMenu is supported.
			int iMenuType = 0;

				// common pointer to IContextMenu and higher version interface.
			LPCONTEXTMENU pContextMenu;

			if( !detail::query_interface( (void**) &pContextMenu, iMenuType ) )    
				return;    // something went wrong


				// create a new menu.
			HMENU MenuHandle = CreatePopupMenu();

				// lets fill the our popupmenu.
			pContextMenu->QueryContextMenu( MenuHandle, 0, global::MinId, global::MaxId, CMF_EXPLORE );

				// subclass window to handle menurelated messages in CShellContextMenu 
			if (iMenuType > 1)    // only version 2 and 3 supports menu messages
				{
//				global::OldWndProc = (WNDPROC) SetWindowLongPtr( _ParentWindowHandle, GWLP_WNDPROC, (LONG)detail::window_proc );
				if (iMenuType == 2)
					global::IContext2 = (LPCONTEXTMENU2) pContextMenu;
				else	// version 3
					global::IContext3 = (LPCONTEXTMENU3) pContextMenu;
				}

				// track the menu.
			UINT idCommand = TrackPopupMenu( MenuHandle, TPM_RETURNCMD | TPM_LEFTALIGN, _Point.first, _Point.second, 0, _ParentWindowHandle, 0 );
		    
				// unsubclass
//			if (global::OldWndProc)
//				SetWindowLongPtr( _ParentWindowHandle, GWLP_WNDPROC, (DWORD)global::OldWndProc );

				// see if returned idCommand belongs to shell menu entries
			if( idCommand >= global::MinId && idCommand <= global::MaxId )
				{
					//executes related command
				detail::invoke_command( pContextMenu, idCommand - global::MinId );    
				idCommand = 0;
				}

			pContextMenu->Release();
			global::IContext2 = 0;
			global::IContext3 = 0;
			}
		}


	void contextmenu( std::vector<std::string> _ItemList, void* _ParentWindowHandle, std::pair<int,int> _Point )
		{
		
			// get IShellFolder interface of Desktop (root of shell namespace)
		IShellFolder * DesktopShellFolder = 0;
		SHGetDesktopFolder(&DesktopShellFolder);	// needed to obtain full qualified pidl

			// ParseDisplayName creates a PIDL from a file system path relative to the IShellFolder interface
			// but since we use the Desktop as our interface and the Desktop is the namespace root
			// that means that it's a fully qualified PIDL, which is what we need
		LPITEMIDLIST pidl = 0;

		DesktopShellFolder->ParseDisplayName( 0, 0, (LPOLESTR)core::string_cast<std::wstring>(_ItemList[0]).c_str(), 0, &pidl, 0);


			// now we need the parent IShellFolder interface of pidl, and the relative PIDL to that interface
		LPITEMIDLIST pidlItem = 0;	// relative pidl
		SHBindToParent/*Ex*/(pidl, IID_IShellFolder, (void **) &global::ShellFolder, 0);

			// now we have the IShellFolder interface to the parent folder specified in the first element in strArray
			// since we assume that all objects are in the same folder (as it's stated in the MSDN)
			// we now have the IShellFolder interface to every objects parent folder
		
		IShellFolder * psfFolder = 0;
		global::ItemCount = _ItemList.size();

		global::PidlArray = (LPITEMIDLIST *) malloc( sizeof (LPITEMIDLIST) );
		for( std::size_t i = 0; i < _ItemList.size(); i++ )
			{
			pidlItem = 0;
			pidl = 0;

			DesktopShellFolder->ParseDisplayName( 0, 0, (LPOLESTR)core::string_cast<std::wstring>(_ItemList[i]).c_str(), 0, &pidl, 0);

			global::PidlArray = (LPITEMIDLIST *) realloc( global::PidlArray, (i + 1) * sizeof (LPITEMIDLIST) );

				// get relative pidl via SHBindToParent
			HRESULT Result = SHBindToParent/*Ex*/ (pidl, IID_IShellFolder, (void **) &psfFolder, (LPCITEMIDLIST *) &pidlItem);
			if( SUCCEEDED(Result) )
				{
				global::PidlArray[i] = detail::CopyPIDL (pidlItem);	// copy relative pidl to pidlArray
				psfFolder->Release();
				}
			}

		DesktopShellFolder->Release();

		detail::show( (HWND)(*(&_ParentWindowHandle)), _Point );

		global::ShellFolder->Release ();
		global::ShellFolder = 0;
		detail::FreePIDLArray( global::PidlArray );
		global::PidlArray = 0;
		}
	
	void contextmenu( std::string _Item, void* _ParentWindowHandle, std::pair<int,int> _Point )
		{
		if( _Item.empty() )
			return;

		std::vector<std::string> List;
		List.push_back( _Item );

		contextmenu( List, _ParentWindowHandle, _Point );
		}

	} /* shell */ } /* core */ } /* xr */

