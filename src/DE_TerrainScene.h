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

#ifndef ___HORROR_TERRAIN_H
#define ___HORROR_TERRAIN_H

namespace Horror {

	class TerrainScene : public Demo::Scene {

		private:

			DGL::Terrain			terrain;
			DGL::Frustum			underwaterFrustum;
			DGL::Fog				underwaterFog;
			DGL::UnderwaterPerspective
									underwaterPerspective;

			bool					cameraIsUnderwater;
			DGL::SkyBox_static		skbox;
			DGL::Water				water;

			DGL::ModelMilkshape		modelCity;
			DGL::Light				light;

		public:

			TerrainScene() : light(GL_LIGHT0)
			{
			}

			void Init()
			{
				this->terrain.LoadTerrain("DTerrain");
				this->underwaterFog.SetColor(0.50f, 0.60f, 1.00f, 1.00f);
				this->underwaterFog.SetStartEnd(0, 100);
				this->underwaterFog.Init();
				this->skbox.InitSkybox("Clouds", 600,600,300);
				this->water.Init("water",600,600);
				this->modelCity.LoadModelData("textures/fence.ms3d");
				this->modelCity.SetPosition( Vector(0, 10, 0 ));
			}

			void Update(float msec)
			{
				this->water.Update(msec);
				this->underwaterPerspective.Update(msec);

				//HACK: check if the camera is underwater
				float matrix[4][4];
				glGetFloatv(GL_PROJECTION_MATRIX, (float *)matrix);
				this->cameraIsUnderwater = false; //matrix[3][1] < this->water.GetWaterHeight();

//				this->modelCity.SetAngles( this->modelCity.GetAngles() + Vector( 0, msec/ 100.0f, 0));
				this->modelCity.Update(msec);
			}

		private:
			void drawObjects()
			{
				// Render Skybox
				this->skbox.Render();

				// Render Terrain
//				this->terrain.Render();

				// Render the model
				this->modelCity.Render();
				this->modelCity.RenderModelCoords();
			}
		public:

			void Render()
			{
				glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				this->drawObjects();
				return;

				if(this->cameraIsUnderwater){
					this->underwaterPerspective.ModifyPerspective(DSys::Window::Width(),DSys::Window::Height(),45,.5f,1000);
					this->underwaterFog.Apply();
				}

				if(!this->cameraIsUnderwater) {
					if( true )
					{ // reflections is enabled
										
						// enable a clipping plane to clip things out of water
						double plane_eqn[4] = { 0, -1, 0, this->water.GetWaterHeight()}; 
						glClipPlane( GL_CLIP_PLANE0, plane_eqn ); 
						glEnable( GL_CLIP_PLANE0 ); 
						
						// vertical-flip everything
						glTranslatef(0, 2* this->water.GetWaterHeight(), 0);
						glScalef(1, -1, 1);
						
						// reverse culling order
						glCullFace(GL_FRONT);

						// get current frustum
						DGL::Model::_FrustumUpdate();

						// draw the scene
						this->drawObjects();

						// restore correct culling
						glCullFace(GL_BACK);

						// re-flip vertically
						glScalef(1, -1, 1);
						glTranslatef(0, -2* this->water.GetWaterHeight(), 0);
						
						// disable extra clipping-plane
						glDisable( GL_CLIP_PLANE0 );

						this->water.Render();
					}
				}

				// get current frustum
				DGL::Model::_FrustumUpdate();
				
				// draw scene normally
				this->drawObjects();
				
				if(this->cameraIsUnderwater){
					glCullFace(GL_FRONT);
					this->water.Render();
					glCullFace(GL_BACK);

					this->underwaterPerspective.ResetPerspective(DSys::Window::Width(), DSys::Window::Height(), 45, 0.5f, 100);
					this->underwaterFog.Disable();

				} else if(! true )
					// no need for this pass if there is a water reflection
					this->water.Render();

			}

			void Destroy()
			{
				this->terrain.Destroy();
				this->water.Destroy();
				this->skbox.Destroy();

				this->modelCity.Destroy();
			}

			void Restart()
			{
			}

	};

}// namespace Horror

#endif // ___HORROR_TERRAIN_H