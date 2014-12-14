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

#ifndef ___DEMO_REGISTERY_H
#define ___DEMO_REGISTERY_H

namespace Demo {

	class Registery {

		public:

			void Register(Demo::Scene *scene, const char *cameraScript)
			{
				if(cameraScript)
				{
					this->scenes[this->freeSlot].camera = new class Demo::Camera;
					this->scenes[this->freeSlot].camera->Init( cameraScript);
					this->scenes[this->freeSlot].hasCamera = true;
				}
				else
				{
					this->scenes[this->freeSlot].hasCamera = false;
				}

				this->scenes[this->freeSlot].scene = scene;
				this->scenes[this->freeSlot].scene->Init();
				this->freeSlot++;
				if(this->freeSlot >= this->_SceneMax)
					throw Daher::Exception("Demo::Registery::Register : no more slots");
				this->activeScene = 0;
			}

			void Update(float msec)
			{
				if(this->Done())	return;
				
				// update the scene
				this->scenes[this->activeScene].scene->Update(msec);

				if(this->scenes[this->activeScene].hasCamera)
				{
					this->scenes[this->activeScene].camera->Update(msec);
					if(this->scenes[this->activeScene].camera->Done())
					{
						this->EndCurrent();
						return this->Update(msec);
					}
				}
				else	// has no camera
				{
					if(this->scenes[this->activeScene].scene->Done())
					{
						this->EndCurrent();
						return this->Update(msec);
					}

				}
			}

			void Render()
			{
				if(this->Done())	return;

				if(this->scenes[this->activeScene].hasCamera)
					if(!DSys::sv_bDeveloper)
						this->scenes[this->activeScene].camera->Setup();
                
				this->scenes[this->activeScene].scene->Render();

				if(this->scenes[this->activeScene].hasCamera)
					this->scenes[this->activeScene].camera->Render();
			}

			void RestartCurrent()
			{
				if(this->Done())	return;

				this->scenes[this->activeScene].scene->Restart();
				if(this->scenes[this->activeScene].hasCamera)
					this->scenes[this->activeScene].camera->Restart();
			}

			void RestartAll()
			{
				if(this->Done())	return;

				for(int i=0; i< this->freeSlot; i++)
				{
					this->scenes[i].scene->Restart();
					if(this->scenes[i].hasCamera)
						this->scenes[i].camera->Restart();
				}
				this->activeScene = 0;
			}

			void EndCurrent()
			{
				this->activeScene++;
				if(this->activeScene == this->freeSlot)
				{
					// check for loop demo
					if(r_loopDemo)
						this->RestartAll();
					else
						this->Destroy();
				}
			}

			void Destroy()
			{
				while(--this->freeSlot >=0 )
				{
					if(this->scenes[this->freeSlot].hasCamera)
					{
						this->scenes[this->freeSlot].camera->Destroy();
						delete this->scenes[this->freeSlot].camera;
					}
					this->scenes[this->freeSlot].scene->Destroy();					
				}
				this->activeScene = 0;
				this->freeSlot = 0;
			}

			bool Done()
			{
				return this->activeScene >= this->freeSlot;
			}

			Registery() :
					freeSlot(0),
					activeScene(0)
			{
			}

		private:
			struct Scene_s
			{
				Demo::Scene		*scene;
				bool			hasCamera;
				Demo::Camera	*camera;
			};

			static const int	_SceneMax = 10;
			Scene_s			scenes[_SceneMax];
			int				freeSlot;
			int				activeScene;

	};
}

#endif // ___DEMO_REGISTERY_H