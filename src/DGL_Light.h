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

#ifndef ___DGL_LIGHT_H
#define ___DGL_LIGHT_H

/*
TODO List:
- ...
*/

namespace DGL {

	class Light {
		public:
			void Setup()
			{
				// light color
				glLightfv(this->slot, GL_AMBIENT, this->ambient);
				glLightfv(this->slot, GL_DIFFUSE, this->diffuse);
				glLightfv(this->slot, GL_SPECULAR, this->specular);

				// position
				glLightfv(this->slot, GL_POSITION, this->pPosition);

				// spot
				if(this->bSpot)
				{
					glLightfv(this->slot, GL_SPOT_DIRECTION, this->vSpotDirection);
					glLightf(this->slot, GL_SPOT_CUTOFF, this->fSpotCutoff);
					glLightf(this->slot, GL_SPOT_EXPONENT, this->fSpotExponent);
				}

				// attenuation
				glLightf(this->slot, GL_CONSTANT_ATTENUATION, this->fConstAttenuation);
				glLightf(this->slot, GL_LINEAR_ATTENUATION, this->fLinearAttenuation);
				glLightf(this->slot, GL_QUADRATIC_ATTENUATION, this->fQuadraticAttenuation);
			}
			void Enable()
			{
				glEnable(this->slot);
			}
			void Disable()
			{
				glDisable(this->slot);
			}

			enum ColorParam {
				AMBIENT,
				DIFFUSE,
				AMBIENT_AND_DIFFUSE,
				SPECULAR
			};

			void SetColor(ColorParam cp, const Color& c)
			{
				switch (cp)
				{
					case AMBIENT:
						this->ambient = c;
						break;
					case DIFFUSE:
						this->diffuse = c;
						break;
					case AMBIENT_AND_DIFFUSE:
						this->ambient = c;
						this->diffuse = c;
					case SPECULAR:
						this->specular = c;
						break;
				}
			}

			void SetAttenuation(float constAtten,float linearAtten, float quadraticAtten)
			{
				this->fConstAttenuation = constAtten;
				this->fLinearAttenuation = linearAtten;
				this->fQuadraticAttenuation = quadraticAtten;
			}

			void SetSpot(Vector direction, float cutoff, float exponent)
			{
				this->bSpot = true;
				this->vSpotDirection = direction;
				this->fSpotCutoff = cutoff;
				this->fSpotExponent = exponent;
			}

			void SetPosition(Vector position)
			{
				this->vPosition = position;
			}

			void SetSlot( GLenum slotID)
			{
				this->slot = slotID;
			}

		protected:
			GLenum		slot;
			Color		ambient;
			Color		diffuse;
			Color		specular;

			union {
				struct {
					Vector	vPosition;
					float	fIsNotDirectional;
				};
				float	pPosition[4];
			};

			bool		bSpot;
			Vector		vSpotDirection;
			float		fSpotExponent;
			float		fSpotCutoff;

			float		fConstAttenuation;
			float		fLinearAttenuation;
			float		fQuadraticAttenuation;

		public:
			Light(GLenum slotID = 0) :
						slot(slotID),
						ambient(0.0f,0.0f,0.0f,1.0f),
						diffuse(1.0f,1.0f,1.0f,1.0f),
						specular(1.0f,1.0f,1.0f,1.0f),
						vPosition(0.0f,0.0f,1.0f),
						fIsNotDirectional(1.0f),

						bSpot(false),
						vSpotDirection(0.0f,0.0f,-1.0f),
						fSpotExponent(0.0f),
						fSpotCutoff(180.0f),

						fConstAttenuation(1.0f),
						fLinearAttenuation(0.0f),
						fQuadraticAttenuation(0.0f)
			{
			}

			

			////////////////////////////
			// Light models

			enum ModelParam {
				MODEL_LOCAL_VIEWER,
				MODEL_AMBIENT,
			};

			static void _ModelSet(ModelParam param, bool b)
			{
				switch (param)
				{
					case MODEL_LOCAL_VIEWER:
						glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, (GLboolean) b);
						break;
				}
			}
			static void _ModelSet(ModelParam param, const Color& c)
			{
				switch (param)
				{
					case MODEL_AMBIENT:
						glLightModelfv(GL_LIGHT_MODEL_AMBIENT, c);
						break;
				}
			}
	};

}//namespace DGL


#endif // ___DGL_LIGHT_H