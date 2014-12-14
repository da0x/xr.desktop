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
#include "DSys.h"
#include "DSys_ConfigDialog.h"
#include "resource.h"

namespace DSys {

	BOOL CALLBACK ConfigDialog::ConfigDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{ 
		switch (message)
		{ 
			case WM_COMMAND: 
				switch (LOWORD(wParam))
				{
					case IDOK:
						if(!IsDlgButtonChecked(hwndDlg, IDC_FULLSCREEN))
							sv_bFullscreen.setval("0");
						else
							sv_bFullscreen.setval("1");

						if(!IsDlgButtonChecked(hwndDlg, IDC_TIMEDEMO))
							sv_bTimeDemo.setval("0");
						else
							sv_bTimeDemo.setval("1");

						if(!IsDlgButtonChecked(hwndDlg, IDC_DEVELOPER))
							sv_bDeveloper.setval("0");
						else
							sv_bDeveloper.setval("1");

						if(!IsDlgButtonChecked(hwndDlg, IDC_DRAW_FPS))
							r_drawFPS.setval("0");
						else
							r_drawFPS.setval("1");

						if(!IsDlgButtonChecked(hwndDlg, IDC_SFX))
							DSys::SendConsoleCommand("set s_soundEnable 0");
						else
							DSys::SendConsoleCommand("set s_soundEnable 1");
/*
						if(IsDlgButtonChecked(hwndDlg, IDC_640_480_16))
						{
							sv_iColorBits.setval	( "16" );
							sv_iScreenWidth.setval	( "640" );
							sv_iScreenHeight.setval	( "480" );
						}
						else if(IsDlgButtonChecked(hwndDlg,IDC_800_600_16))
						{	
							sv_iColorBits.setval	( "16" );
							sv_iScreenWidth.setval	( "800" );
							sv_iScreenHeight.setval	( "600" );
						}
						else if(IsDlgButtonChecked(hwndDlg,IDC_1024_768_16))
						{
								sv_iColorBits.setval	( "16" );
								sv_iScreenWidth.setval	( "1024" );
								sv_iScreenHeight.setval	( "768" );
						}
						else*/ if(IsDlgButtonChecked(hwndDlg,IDC_640_480_32))
						{
							sv_iColorBits.setval	( "32" );
							sv_iScreenWidth.setval	( "640" );
							sv_iScreenHeight.setval	( "480" );
						}
						else if(IsDlgButtonChecked(hwndDlg,IDC_800_600_32))
						{
							sv_iColorBits.setval	( "32" );
							sv_iScreenWidth.setval	( "800" );							sv_iScreenHeight.setval	( "600" );
						}
						else if(IsDlgButtonChecked(hwndDlg,IDC_1024_768_32))
						{
							sv_iColorBits.setval	( "32" );
							sv_iScreenWidth.setval	( "1024" );
							sv_iScreenHeight.setval	( "768" );
						} // else keep the incode initializations

						DSys::Var::ListUpdateAll();

						EndDialog(hwndDlg, IDOK); 
						return true;

					case IDCANCEL:
						EndDialog(hwndDlg, IDCANCEL);
						PostQuitMessage(0);
						DSys::Logger::Print("Canceled! ... Quiting");
						return true;

					case IDC_CONTROLS:
						MessageBox(hwndDlg,
							"=================================\n"
							"Game Controls \n"
							"=================================\n"
							"Mouse Move\t: Control ball launcher.\n"
				//			"W\t\t: Move farward.\n"
				//			"S\t\t: Move backward.\n"
				//			"A\t\t: Strafe left.\n"
				//			"D\t\t: Strafe right.\n"
				//			"Space\t\t: Move up.\n"
				//			"C\t\t: Move down.\n"
				//			"~\t\t: Toggle console.\n"
							"Mouse Left Click\t: Launch Ball\n"
							"ESC\t\t: Toggle Menu.\n"
							"Insert\t\t: Toggle Console.\n"
							"F10\t\t: Shutdown Application.\n"
							"F12\t\t: Screenshot.\n"
				//			"=================================\n"
				//			"Misc keys:\n"
				//			"=================================\n"
				//			"Alt + S\t\t: Save Camera Node.\n"
				//			"Alt + U\t\t: Toggle Camera Automove.\n"
				//			"Arows:\t\t: Move Particle Engine.\n"
				//			"R\t\t: Select Water Patricle Engine.\n"
				//			"F\t\t: Select Fire  Patricle Engine.\n"
							,

							"Demo Controls", 0);
						break;
				}
				switch (HIWORD(wParam))
				{
					case BN_CLICKED:
						CheckDlgButton(hwndDlg, lParam, BST_CHECKED);
						break;
				}
				break;

			case WM_INITDIALOG:
				DSys::Logger::Print("Initializing Dialogbox");
				SetDlgItemText(hwndDlg, IDC_ENGINE_TITLE, DSys::Version );
				CheckRadioButton(hwndDlg, IDC_640_480_32, IDC_1024_768_32, IDC_1024_768_32);
				CheckDlgButton	(hwndDlg, IDC_FULLSCREEN,	BST_CHECKED);
				CheckDlgButton	(hwndDlg, IDC_TIMEDEMO,		BST_UNCHECKED);
				CheckDlgButton	(hwndDlg, IDC_GLEXTENSIONS, BST_CHECKED);
				CheckDlgButton	(hwndDlg, IDC_DRAW_FPS,		BST_UNCHECKED);
				CheckDlgButton	(hwndDlg, IDC_SFX,			BST_CHECKED);
		} 
		return false;
	}

	bool ConfigDialog::Show()
	{
		int result = DialogBox(
							NULL,
							MAKEINTRESOURCE(IDD_STARTUP),
							HWND_DESKTOP,
							(DLGPROC)ConfigDialogProc);
		
		if( result == IDCANCEL )
			return false;

		DSys::SendConsoleCommand("writeconfig user/user.cfg");
		return true;
	}


};