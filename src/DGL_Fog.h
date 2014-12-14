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

#ifndef ___DFOG_H
#define ___DFOG_H

/*
TODO List:
- ...
*/


namespace DGL {
	class Fog{
		private:
			float		FogDensity;					// density
			bool		bFog;						// for status
			float		fogColor[4];				// fog color
			float		fogStart;					// fog start
			float		fogEnd;						// fot end
		public:
			Fog	(){
				FogDensity = 0.2f;
				bFog = true;
				fogColor[0]=fogColor[1]=fogColor[2]=fogColor[3]= 1.0f;
				fogStart = 0.0f;
				fogEnd = 20.0f;
			}
			void SetDensity(float d){
				if(FogDensity > 1) FogDensity = 1;
				if(FogDensity < 0) FogDensity = 0;
				glFogf(GL_FOG_DENSITY, FogDensity);
			}
			void SetStartEnd(float start, float end){
				fogStart = start;
				fogEnd = end;
			}
			void Toogle(){
				bFog = !bFog;
				if(bFog)
					glEnable(GL_FOG);
				else
					glDisable(GL_FOG);
			}
			void SetColor(float r, float g, float b, float a){
				fogColor[0]= r;
				fogColor[1]= g;
				fogColor[2]= b;
				fogColor[3]= a;
			}
			void Init(){
				// set the fog mode
				glFogi(GL_FOG_MODE, GL_LINEAR);
				// set the fog color and clear color
				glFogfv(GL_FOG_COLOR, fogColor);
				glClearColor(fogColor[0],fogColor[1],fogColor[2],fogColor[3]);
				// set the density
				glFogf(GL_FOG_DENSITY, FogDensity);
				// how acurate the calculated fog is
				glHint(GL_FOG_HINT, GL_NICEST);
				// set start and end
				glFogf(GL_FOG_START, fogStart);
				glFogf(GL_FOG_END, fogEnd);
			}
			void Apply(){
				glEnable(GL_FOG);
			}
			void Disable(){
				glDisable(GL_FOG);
			}
	};

}//namespace DGL
#endif // ___DFOG_H