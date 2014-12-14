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

#ifndef ___MCU_SNOW_H
#define ___MCU_SNOW_H

namespace MCU {

	class SnowScene : public Demo::Scene {

		private:

			DGL::Fog				fog;

//			DGL::SkyBox_static		skbox;

			DGL::ModelMilkshape		modelStation;
			DGL::Light				light;

		public:

			SnowScene() : light(GL_LIGHT0)
			{
			}

			void Init()
			{
//				this->fog.SetColor(0.70f, 0.70f, 0.70f, 1.00f);
//				this->fog.SetStartEnd(0, 100);
//				this->fog.Init();
//				this->skbox.InitSkybox("Clouds", 600,600,300);
				this->modelStation.LoadModelData("textures/fence.ms3d");
				this->modelStation.SetPosition( Vector(0, 0, 0 ));
			}

			void Update(float msec)
			{
				this->modelStation.Update(msec);
			}

	
			void Render()
			{
				glClearColor(1,1,1,1);
				glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

//				this->fog.Apply();
				this->modelStation.Render();
			}

			void Destroy()
			{
//				this->skbox.Destroy();

				this->modelStation.Destroy();
			}

			void Restart()
			{
			}

	};

}// namespace MCU

#endif // ___MCU_SNOW_H