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

#ifndef ___DEMO_EFFECT
#define ___DEMO_EFFECT

namespace Demo {


	////////////////////////
	// Base Effect class
	class Effect {
	protected:
		float		time;
	public:
		virtual void Restart()
		{
			time = 0;
		}
		virtual void Destroy()
		{
		}
	};



    ////////////////////////
	// FadingLogo
	class FadingLogo : public Effect {
		protected:
			float			totalTime;
			float			delayTime;
			float			alpha;
			DGL::Texture	texture;
			int				width, height;

		public:
			void Init(int w, int h, float totalTime,float delayTime, char*texturePath)
			{
				this->time = 0;
				this->texture.Build(texturePath);
				this->totalTime = totalTime;
				this->delayTime = delayTime;
				this->alpha = 0;
				this->width = w;
				this->height = h;
			}
			inline bool Done()
			{
				return this->time >= this->totalTime+this->delayTime;
			}
			void Update(float msec)
			{
				this->time += msec;

				// check for fading in
				if(this->time <= this->totalTime / 6.0f )
				{
					this->alpha = this->time/(this->totalTime/6.0f);
				}
				else if(this->time <= 4 * this->totalTime /6.0f)
				{
					this->alpha = 1.0f;
				}
				else
				{
					this->alpha = (this->totalTime - this->time) / (2 * this->totalTime / 6.0f);
					if(this->alpha < 0)
						this->alpha = 0.0f;
				}
			}
			void Render()
			{
				DGL::Draw2DRect(
						320 - this->width/2,
						300 - this->height/2,
						this->width,
						this->height,
						this->texture,
						this->alpha);
			}
			void Destroy()
			{
				this->texture.Delete();
			}
	};

	//////////////////////////
	// Poping logo
	class PopingLogo : public FadingLogo {
		public:
			void Render()
			{
				if(alpha)
				{
					DGL::Draw2DRect(
							320 - this->width*this->alpha/2,
							300 - this->height*this->alpha/2,
							this->width*this->alpha,
							this->height*this->alpha,
							this->texture);
				}
			}
	};

}// namespace Horror

#endif // ___DEMO_EFFECT