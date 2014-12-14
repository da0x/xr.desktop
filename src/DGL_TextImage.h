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

#pragma once
#ifndef ___DGL_TEXTIMAGE_H
#define ___DGL_TEXTIMAGE_H

#include "DGL_Texture.h"

namespace DGL {
/*
	class TextImage {

		Texture				image;
		char				textBuffer[1024];
		int					textLength;
		bool				random;

		int					width;

		struct Element {
			Color	color;
			char	charecter;
		};

		Element				*elements;
	public:

		TextImage( const char *JPGfile, const char *fmt, int width, bool random)
		{
			strcpy( this->textBuffer, fmt);
			this->textLength = strlen( this->textBuffer);
			this->random = random;
			this->width = width;

			this->image.Build(JPGfile);
			this->elements = new Element[this->image.Width() * this->image.Height()];

			int s = 0;
			for ( int j=0; j< this->image.Height(); j++)
			{
				for ( int i=0; i< this->image.Width(); i++)
				{
					this->elements[i + j * this->image.Height()].charecter =
						this->random
							?
							this->textBuffer[Random::Int() % this->textLength]
							:
							this->textBuffer[s++ % this->textLength];

					this->elements[i + j * this->image.Height()].
						color.Set(	this->image( i, j, this->image.Red),
									this->image( i, j, this->image.Green),
									this->image( i, j, this->image.Blue));
				}
			}
		}

		void Render()
		{
			glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			for(int j=0; j< this->image.Height(); j++)
			{
				for(int i=0; i< this->image.Width(); i++)
				{
					int index = i + j* this->image.Height();
					DGL::Font::Print3D(
						i*10,j*10,0,
						"%c",
						1,
						DGL::Font::STYLE_LEFT, 
						this->elements[index].color,
						0,
						this->elements[index].charecter);
				}
			}
			glPopAttrib();
		}


	};
	*/
}

#endif // ___DGL_TEXTIMAGE_H