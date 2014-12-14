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


#ifndef ___D_STARTUPDIALOG_H
#define ___D_STARTUPDIALOG_H

namespace DSys {

	class ConfigDialog
	{
			static char	*controlsString;

			static BOOL CALLBACK ConfigDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
		public:
			static bool Show();
	};

}// end of namespace


#endif // ___D_STARTUPDIALOG_H