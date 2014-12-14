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

#ifndef ___HORROR_INTRO_H
#define ___HORROR_INTRO_H

namespace Horror {

	class IntroScene : public Demo::Scene {
		private:
			bool				done;
			float				time;
			DGL::Font			font;
			DGL::FadeScreen		fadeOut;

			static const int		_numOfFadingLogos = 3;
			Demo::FadingLogo	logos[_numOfFadingLogos];
			Demo::PopingLogo	daherLogo;

			float				text_y;

			DGL::Texture		reflectionMap;

		public:
			void Init()
			{
				this->font.Create("Font");
				this->font.SetStyle(this->font.STYLE_LEFT|this->font.STYLE_SHADOWS);
				this->time = 0;
				this->done = false;
				this->text_y = 0;
				int i =0;
				this->logos[i++].Init(200,113,7000, 2000, "textures/logoOGL.jpg");
				this->logos[i++].Init(200,113,7000, 2000, "textures/logoDE.jpg");
				this->logos[i++].Init(256,128,10000, 3000, "textures/logoDTO.jpg");
				this->daherLogo.Init(300,200,10000, 0, "textures/logoDIGIT.jpg");

				this->fadeOut.Init(DGL::Color(0.0f,0.0f,0.0f,0.0f),DGL::Color(0.0f,0.0f,0.0f,1.0f), 500);

				this->reflectionMap.Build("textures/refmap.tga");
			}

			void Update(float msec)
			{
				this->time += msec;
				if(this->time< 4000)
					return;

				int i;

				for( i =0; i< this->_numOfFadingLogos; i++)
					if(!this->logos[i].Done())
						break;

				if( i != this->_numOfFadingLogos)
				{
					this->logos[i].Update(msec);
					return;
				}

				this->daherLogo.Update(msec);

				// start scrolling text
				this->text_y += msec/50.0f;


				if(this->text_y >= 380)
					this->fadeOut.Update(msec);
			}

		private:
			inline void addText(int y, const char *fmt)
			{
				this->font.Print(20, y + this->text_y, fmt);
			}
		public:
			void Render()
			{
				const DGL::Color color(0.02734f,0.3828f,0.46875f,1.0f);// = DGL::Color::ColorGold()* 0.5f;
				glClearColor(color.r,color.g,color.b,color.a);
				glClear(GL_COLOR_BUFFER_BIT);
				glClearColor(0,0,0,1);
				DGL::Draw2DRect(0,0,DSys::sv_iScreenWidth,DSys::sv_iScreenHeight, DGL::Color::ColorBlack(), color);

				int i =0;
				while( this->logos[i].Done() && i< this->_numOfFadingLogos ){
					i++;
				}

				glPushAttrib(GL_ENABLE_BIT|GL_LIGHTING_BIT);
				glDisable(GL_LIGHTING);
				if(i < this->_numOfFadingLogos)
				{
					this->logos[i].Render();
					return;
				}
				
				if(!this->daherLogo.Done())
					this->daherLogo.Render();

                
				int y =0;
				this->font.SetSize(2.0f);
				this->addText( y, "www.DigitalThinker.org");

				y-= 30;
				this->font.SetSize(1.2f);
				this->addText( y, "Digital Thinkers");
				y-= 10;
				this->font.SetSize(0.8f);
				this->addText( y, "Dee       - Daher Alfawares  - 0x29a");
				y-= 10;
				this->addText( y, "Microbe   - Yohaan         - I'm a geant wild tardigrade");
				y-= 10;
				this->addText( y, "R*Knight  - Ramon Wong     - Darren Hayes Biggest Fan!!");
				y-= 10;
				this->addText( y, "Tomas     - Tomas Hamala   - Microsoft Jesus");
				y-= 10;
				this->addText( y, "Vlado     - Vlado Jokic    - Smiling Politely");
				y-= 10;
				this->addText( y, "Gautam    - Gautamn Narain");
				y-= 10;
				this->addText( y, "Wichetael - Merko          - Monkey..");
				y-= 10;
				this->addText( y, "Howdy     - Mike Robins    - Getting ready for Lotoja");
				y-= 10;
				this->addText( y, "Doxxan    - Unknown        - Mutant-killer-ninja-noodle");
				y-= 20;
				this->font.SetSize(1.2f);
				this->addText( y, "Programming");
				y-= 10;
				this->font.SetSize(0.8f);
				this->addText( y, "Dee - 0x29a");
				y-= 20;
				this->font.SetSize(1.2f);
				this->addText( y, "Special thanks");
				y-= 10;
				this->font.SetSize(0.8f);
				this->addText( y, "Vlado");

				this->font.SetColor(DGL::Color::ColorGold());
				y-= 100;
				this->addText( y, "July 6th, 2006. Copyright (c) 2003 - 2006 DigitalThinker.org");
				y-= 15;
				this->font.SetColor(DGL::Color::ColorWhite());  this->addText( y, "Demo writen by ");
				this->font.SetColor(DGL::Color::ColorYellow()); this->addText( y, "               D");
				this->font.SetColor(DGL::Color::ColorGreen());  this->addText( y, "                a");
				this->font.SetColor(DGL::Color::ColorWhite());  this->addText( y, "                 H");
				this->font.SetColor(DGL::Color::ColorGreen());  this->addText( y, "                  e");
				this->font.SetColor(DGL::Color::ColorYellow()); this->addText( y, "                   R");
				this->font.SetColor(DGL::Color::ColorWhite());


				if(!this->fadeOut.Render())
					this->done = true;
				glPopAttrib();
			}

			void Destroy()
			{
				this->fadeOut.Destroy();
				this->reflectionMap.Delete();
			}

			bool Done()
			{
				return this->done;
			}

			void Restart()
			{
				this->time = 0;
				this->done = false;
				for(int i=0; i< this->_numOfFadingLogos; i++)
					this->logos[i].Restart();
				this->daherLogo.Restart();
				this->fadeOut.Restart();
				this->text_y = 0;
			}
	};

}// namespace Horror

#endif // ___HORROR_INTRO_H