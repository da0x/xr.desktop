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

#ifndef ___DE_GETMRFRENCH_H
#define ___DE_GETMRFRENCH_H

namespace GetMrFrench {
	using namespace DGL;
	const static int		_Width			= 50;
	const static int 		_Height			= 35;
	const static int		_CameraZ		= 50;

	DSys::Var_float		_GravityAccel	("bg_gravityAccel",		"9.802");
	DSys::Var_float		_FrictionFactor	("bg_FrictionFactor",	"0.924");
	DSys::Var_float		_RadialEffect	("bg_RadialEffect",		"20.0");
	DSys::Var_float		_ExpVelocity	("bg_ExpVelocity",		"10.75");
	DSys::Var_float		_ExpSpread		("bg_ExpSpread",		"1200");
	DSys::Var_int		_liquidExpStyle	("bg_ExpStyle",			"0");
	DSys::Var_bool		_localCamera	("g_localCamera",		"1");
	
	const static float	_RotationMax	= 0.180f; // degrees/msec

	class Fragment {
	public:
		Vector		home;
		Vector		position;
		Vector		velocity;
		Vector		expVelocity;
		float		timeLeft;

		struct TexCoord
		{
			float s, t;
		};
		TexCoord	texCoords[4];
		float		rotSpeed;
		float		angle;

		void Init( Vector home )
		{
			this->home = home;
			this->position = home;
			this->velocity.clear();
			this->rotSpeed = Random::Float() * _RotationMax;
			this->expVelocity.clear();
			this->timeLeft = 0;
		}

		void Update(float msec)
		{
			const float sec = msec/1000.0f;

			Vector homeDir = this->home - this->position;

			if( homeDir.length() < 0.1f && this->velocity.length() < 0.1f )
			{
				this->position= this->home;
			}
			else
			{
				Vector acceleration = homeDir.normal()* _GravityAccel;
//				float friction = this->velocity.length() * _FrictionFactor; // inverse

				// v = v0 + a dt
				this->velocity += acceleration * sec;
				this->velocity *= _FrictionFactor;

				// v = dx / dt => dx = v * dt
				this->position += this->velocity * sec;

				// now update the angle
				this->angle += (msec * this->rotSpeed)/1000.0f;
			}

			// check for extra velocity ( due to explosions )
			if( this->timeLeft > 0 )
			{
				this->timeLeft -= msec;
				if( this->timeLeft <= 0 )
					this->AddVelocity( this->expVelocity);
			}
		}

		void AddVelocity( const Vector& velocity )
		{
			this->velocity += velocity;
		}

		void SetExpVelocity( const Vector& velocity, float time )
		{
			this->expVelocity = velocity;
			this->timeLeft = time;
		}
	};

	class Background {
		

		Fragment		fragments[_Width][_Height];
		DGL::Texture	texture;
	public:

		void Init()
		{
			Random::Randomize();
			this->texture.Build("textures/background.jpg");
			for( int j=0; j< _Height; j++)
			{
				for( int i=0; i< _Width; i++)
				{
					Fragment* frag = &this->fragments[i][j];
					frag->Init( Vector(i-_Width/2,j-_Height/2,0) );
					frag->texCoords[0].s =(i  )/ static_cast<float>(_Width);
					frag->texCoords[0].t =(j  )/ static_cast<float>(_Height);
					frag->texCoords[1].s =(i+1)/ static_cast<float>(_Width);
					frag->texCoords[1].t =(j  )/ static_cast<float>(_Height);
					frag->texCoords[2].s =(i+1)/ static_cast<float>(_Width);
					frag->texCoords[2].t =(j+1)/ static_cast<float>(_Height);
					frag->texCoords[3].s =(i  )/ static_cast<float>(_Width);
					frag->texCoords[3].t =(j+1)/ static_cast<float>(_Height);
				}
			}
		}

		void Update(float msec)
		{
			for( int j=0; j< _Height; j++)
			{
				for( int i=0; i< _Width; i++)
				{
					this->fragments[i][j].Update(msec);
				}
			}
		}

		void Explode( Vector expPos)
		{
			Fragment* frag;
			Vector direction;
			float distance;
		
			for( int j=0; j< _Height; j++)
			{
				for( int i=0; i< _Width; i++)
				{
					frag = &this->fragments[i][j];
					direction = frag->position - expPos;
					distance = direction.length();

					if(_liquidExpStyle)
					{
						direction.clear();
						direction.z_() = _ExpVelocity;// * damageFactor;
					}
					else
					{
						direction.normalize();
						direction.x_() *= _ExpVelocity;
						direction.y_() *= _ExpVelocity;// * damageFactor;
						direction.z_() *= _ExpVelocity;// * damageFactor;
					}

						fragments[i][j].SetExpVelocity( direction, (distance/_RadialEffect)*_ExpSpread );
				//	}
				}
			}
		}
		void Render()
		{
			Fragment* frag;

			glBindTexture( GL_TEXTURE_2D, this->texture);
			glColor3fv(DGL::Color::ColorWhite());
			
			for( int j=0; j< _Height; j++)
			{
				for( int i=0; i< _Width; i++)
				{
					frag = &this->fragments[i][j];
//					DGL::MatrixOp::BalloonPoint::Begin( frag->position);
					glPushMatrix();
					glTranslatef(frag->position.x_(), frag->position.y_(), frag->position.z_());
//					glRotatef( frag->angle, 0, 1, 0);
						glBegin(GL_QUADS);
							const float w = 0.5f;
							const float h = 0.5f;
							glTexCoord2f( frag->texCoords[0].s, frag->texCoords[0].t);
							glVertex2f( -w, -h);
							glTexCoord2f( frag->texCoords[1].s, frag->texCoords[1].t);
							glVertex2f( w, -h);
							glTexCoord2f( frag->texCoords[2].s, frag->texCoords[2].t);
							glVertex2f( w, h);
							glTexCoord2f( frag->texCoords[3].s, frag->texCoords[3].t);
							glVertex2f( -w, h);
						glEnd();
//					DGL::MatrixOp::BalloonPoint::End();				
					glPopMatrix();
				}
			}
		}
		void Destroy()
		{
			this->texture.Delete();
		}
	};



	class MrFrench 
	{
		DGL::Texture	texture;
		Vector			position;
	public:
		const static int _Radius = 3; 
		void Init()
		{
			this->texture.Build("textures/glasslogo.tga");
            position.clear();
			position.z_() = 10;
		}

		void Update(float msec)
		{
			static float time;
			time += msec/1000.0f;
			this->position.x_() = 10 * cos( time );
			this->position.y_() = 10 * sin( time );
		}

		void Render()
		{
			glBindTexture(GL_TEXTURE_2D, this->texture);
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable( GL_BLEND);
//			DGL::MatrixOp::BalloonPoint::Begin( this->position );
			glPushMatrix();
			glTranslatef(this->position.x_(), this->position.y_(), this->position.z_());
			glNormal3f( 0,0,1);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2f( -_Radius, -_Radius);
				glTexCoord2f(1,0);
				glVertex2f( _Radius, -_Radius);
				glTexCoord2f(1,1);
				glVertex2f( _Radius, _Radius);
				glTexCoord2f(0,1);
				glVertex2f( -_Radius, _Radius);
			glEnd();
//			DGL::MatrixOp::BalloonPoint::End();
			glPopMatrix();
			glDisable( GL_BLEND);
		}

		
		void Destroy()
		{
			this->texture.Delete();
		}
	};

	class SoundFX {
			private:
				DAudio::Sound2D		hit;
				DAudio::Sound2D		throwPowerup;
				DAudio::Sound2D		throwRelease;

				DAudio::Sound2D		roundStart;
				DAudio::Sound2D		roundEnd;
				DAudio::Sound2D		newLevel;

				DAudio::Sound2D		anthem;

			public:
				void Init()
				{
					this->hit.Load("sounds/hit.wav");
					this->throwPowerup.Load("sounds/throwpowerup.wav");
					this->throwRelease.Load("sounds/throwRelease.wav");

					this->roundStart.Load("sounds/roundStart.wav");
					this->roundEnd.Load("sounds/roundEnd.wav");
					this->newLevel.Load("sounds/newLevel.wav");

					this->anthem.Load("sounds/frenchanthem.wav");
				}
				void TargetHit()
				{
					this->hit.Play();
				}
				void ThrowPowerup()
				{
					this->throwPowerup.Play();
				}
				void ThrowRelease()
				{
					if( this->throwPowerup.IsPlaying() )
						this->throwPowerup.Stop();
					this->throwRelease.Play();
				}
				void RoundStart()
				{
					this->roundStart.Play();
				}
				void RoundEnd()
				{
					this->roundEnd.Play();
				}
				void NewLevel()
				{
					this->newLevel.Play();
				}
				void Anthem()
				{
					this->anthem.Play();
				}
				void Destroy()
				{
					this->hit.Destroy();
					this->throwPowerup.Destroy();
					this->throwRelease.Destroy();

					this->roundStart.Destroy();
					this->roundEnd.Destroy();
					this->newLevel.Destroy();

					this->anthem.Destroy();
				}

		} soundFX;

	class Game: public Demo::Scene
	{
		Background				background;
		DGL::ParticleEngine		stars;
		DGL::Camera				camera;
		MrFrench				mrFrench;

//		Missile					missile;
		DGL::Light				light;

	public:
		void Init()
		{
			this->mrFrench.Init();
			this->background.Init();
			this->camera.Set( DGL::Camera::Param::POSITION, Vector ( 0, 0, _CameraZ));
			this->camera.Set( DGL::Camera::Param::FOCUS, Vector ( 0, 0, 0));
			this->stars.Init(
					Vector(0,0,-10),
					Vector(0,0,5),
					Vector(0,0,10),
					DGL::Color::ColorWhite(),
					DGL::Color(9.5f, 9.5f, 1.0f),
					_Width/2.0f,
					3,
					0.001f);

			this->light.SetSlot(GL_LIGHT0);
			this->light.SetColor( DGL::Light::AMBIENT, Color(0.5f, 0.3f, 0.1f, 1.0f));
			this->light.SetColor( DGL::Light::DIFFUSE, Color(0.8f, 0.7f, 0.5f, 1.0f));
			this->light.SetColor( DGL::Light::SPECULAR, Color(1.0f, 1.0f, 1.0f, 1.0f));
			this->light.SetAttenuation( 0.1f, 0.2f, 0.005f);
		}

		void Restart()
		{
			this->camera.Set( DGL::Camera::Param::POSITION, Vector ( 0, -30, 70));
			this->camera.Set( DGL::Camera::Param::DIRECTION, Vector ( 0, 0, -1));
		}
		
		bool Done()
		{
			return false;
		}

		void Update(float msec)
		{
			float x = DSys::Input::MouseXDelta() / 10.0f;
			float y =-DSys::Input::MouseYDelta() / 10.0f;

			this->camera.MovePlaneRight( x );
			this->camera.MovePlaneUp( y );

			this->background.Update(msec);
			this->mrFrench.Update(msec);

			if( DSys::Input::MouseChar( 0 ) )
			{
				Vector expPos;
				this->camera.Get( DGL::Camera::POSITION, expPos);
				expPos.z_() = -0.25f;
				this->background.Explode( expPos );
			}

			this->stars.Update(msec);
		}

		void Render()
		{
			glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
			if( _localCamera )
				this->camera.Setup();
			glPushAttrib( GL_LIGHTING_BIT );
			glDisable(GL_LIGHTING);
				this->background.Render();
				this->mrFrench.Render();
			glPopAttrib();

			this->stars.Render();
		}

		void Destroy()
		{
			this->mrFrench.Destroy();
			this->background.Destroy();
			this->stars.Destroy();
		}
	};
}

#endif // ___DE_GETMRFRENCH_H
