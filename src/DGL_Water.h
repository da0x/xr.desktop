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

#ifndef ___DWATER_H
#define ___DWATER_H

/*
TODO List:
- ...
*/


namespace DGL {
	class Water{
	protected:
		float			WaterHeight;		// Water Height
		float			WaterTransperency;	// WaterTransperency (0 -> 1)
		float			WaterStepLen;		// Water Step length
		Texture			WaterTexture;
		float			anim;
		Frustum			frustum;

		float			width, length;

	public:

		void Init(char *name, float width,float length){
			this->WaterTexture.Build(va("textures/liquids/%s.jpg", name));
			this->width = width;
			this->length = length;
		}

		void Update(float msec){
			this->anim += msec*0.00003f;
		}

		void Render(){
			DMacro_TraceEnter(Water::Render);
	//		int i,j;
			glPushMatrix();
			glPushAttrib(GL_ENABLE_BIT|GL_LIGHTING_BIT);

			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);

			glBindTexture(GL_TEXTURE_2D, WaterTexture);
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
	//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			static Vertex waterpos( - this->width/2.0f,0, - this->length / 2.0f);
			waterpos.Translate();	

			glColor4f(1,1,1,WaterTransperency);
			this->frustum.Extract();

			glBegin(GL_QUADS);
	/**/
			static int i,j;
			for ( i = 0; i< this->width; i+=WaterStepLen){
				for(j =0; j< this->length; j+=WaterStepLen){
					Vector quad[4];
					quad[0].Set( i,				WaterHeight, j);
					quad[1].Set( i+WaterStepLen,WaterHeight, j);
					quad[2].Set( i+WaterStepLen,WaterHeight, j+WaterStepLen);
					quad[3].Set( i,				WaterHeight, j+WaterStepLen);			

					Vector diag = (quad[0] - quad[2])/2.0f;
					if(!frustum.SphereInFrustum(quad[0] - diag, diag.length() ) )
						continue;

						glTexCoord2f(0.0f + anim,0.0f + anim);
						glVertex3fv(quad[0]);

						glTexCoord2f(1.0f + anim,0.0f + anim);
						glVertex3fv(quad[1]);

						glTexCoord2f(1.0f + anim,1.0f + anim);
						glVertex3fv(quad[2]);

						glTexCoord2f(0.0f + anim,1.0f + anim);
						glVertex3fv(quad[3]);
				}
			}
			glEnd();

			glPopAttrib();
			glPopMatrix();
			DMacro_TraceLeave();
		}
		__forceinline float GetWaterHeight(){
			return WaterHeight;
		}
		void IncreasWaterTransperency(int dir){
			if(dir>0)
				WaterTransperency += 0.01f;
			else
				WaterTransperency -= 0.01f;
			Clamp<float> (0.0f,1.0f,WaterTransperency);
		}
		float GetWaterTransperency(){return WaterTransperency;}


		Water(){
			initvars();
		}
		void initvars(){
			WaterHeight			= 9.0f;
			WaterTransperency	= 0.4f;
			WaterStepLen		= 20;
		}

		void Destroy()
		{
			this->WaterTexture.Delete();
		}
	};
}//namespace DGL


#endif // ___DWATER_H