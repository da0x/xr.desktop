/*//C++

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

#ifndef ___DGL_FADESCREEN_H
#define ___DGL_FADESCREEN_H

namespace DGL {

	//////////////////////////
	// FadeScreen
	class FadeScreen {
		public:
			FadeScreen():	sc(Color::ColorBlack()),
							ec(Color::ColorBlack()),
							tt(0),
							t(0)
			{
			}

			void Init(Color sc, Color ec, float tt)
			{
				this->sc = sc;
				this->ec = ec;
				this->tt = tt;
				this->t  = 0;
			}

			void Update(float msec)
			{
				t += msec;
			}

			bool Render()
			{
			Draw2DRect(0,0,DSys::sv_iScreenWidth,DSys::sv_iScreenHeight,this->CurrentColor());
				if(!tt || t>tt)
					return false;
				return true;
			}

			void Destroy()
			{
				tt = 0;
			}

			void Restart()
			{
				t = 0;
			}

			Color CurrentColor()
			{
				float ratio = (t/tt);
				Clamp<float>(0,1,ratio);
				
				// combine the two colors
				return sc.CombineWith(ec ,ratio);
			}

		protected:
			Color		sc;		// start color
			Color		ec;		// end color
			float		tt;		// total time
			float		t;		// current time

	};
}

#endif // ___DGL_FADESCREEN_H