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

#include "DShared.H"
#include "DSys.H"
#include "DGL.h"

namespace DSys
{
	void IntroScene()
	{
#ifndef _DEBUG
		// Texture
		glEnable( GL_TEXTURE_2D);
		// shade model
		glShadeModel(GL_SMOOTH);		
		// clear color
		glClearColor(0,0,0,1);

		float endTime = 5000; // msec
		bool ending = false;

		DGL::Texture logo;
		logo.Build("textures/engine_logo.jpg");

		DGL::FadeScreen fade;
		fade.Init( DGL::Color(0.0f,0.0f,0.0f,1.0f), DGL::Color(0.0f,0.0f,0.0f,0.0f), 3000 );
	
		const int fadeOutTime = 1000;
		float totalTime = 0;
		float tprev = Timer::Time();
		float time;
		float frameTime;

		bool done =false;
		while( !done && SystemThink() )
		{
			const int fadeOutTime = 1000;

			time = Timer::Time();
			frameTime = time - tprev;
			totalTime += frameTime;
			tprev = time;

			fade.Update(frameTime);
			
			if( !ending )
			{// check if we are ending
				DSys::Input::Update();
				if( DSys::Input::MouseDown(0) || totalTime >= endTime)
				{// on mouse down and when time it up
					totalTime = endTime;
					ending = true;
					// then launch a fade out object
					// make sure to start from the current color of the fade,
					// so that if the mouse was clicked early, then the
					// fade will not flicker...
					fade.Init(
						fade.CurrentColor(),
						DGL::Color(0.0f,0.0f,0.0f,1.0f), fadeOutTime );
				}
			}
			else
			{// if we are and fadeout time reached then we're done
				if(totalTime >= endTime + fadeOutTime )
					done = true;
			}

			// render
			glClear( GL_COLOR_BUFFER_BIT );
			
			const int width		= 128;
			const int height	= 32;		

			DGL::Draw2DRect(
					320-width/2,
					240-height/2,
					width,
					height,
					logo);

			fade.Render();

		}// while
	
		logo.Delete();
		// Texture
		glDisable( GL_TEXTURE_2D);
#endif // _DEBUG
	}
}//namespace DSys