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

#ifndef ___DGL_SHADER_H
#define ___DGL_SHADER_H


#include <fstream>
#include <cstdio>

namespace DGL
	{

	class ShaderLib;

	class Shader
		{
		friend class ShaderLib;

		public:
			inline void Begin() const
				{
				this->activeStage->Begin();
				}
			inline void End() const
				{
				this->activeStage->End();
				}
			inline bool Multitexture() const
				{
				return this->activeStage->unit1;
				}

		private:
			void Init();
			void Destroy();

			inline void Update( float msec)
				{
				this->activeStage->Update(msec);
				if( this->activeStage->Done() )
					{
					this->activeStage = this->activeStage->next;
					}
				}

		private:		
			class Stage
				{
				friend class Shader;
				friend class ShaderLib;

				public:
					void Init();
					void Destroy();

					inline void Update( float msec)
						{
						this->time += msec;
						this->unit0.Update( msec);
						this->unit1.Update( msec);
						}
					inline void Begin() const
						{
						if( this->unit0 )
							{
							this->unit0.Begin();
							
							if( this->unit1 )
								{
								if( Extensions::IsEnabled_GL_ARB_multitexture() )
									{
									if(Extensions::IsEnabled_GL_EXT_texture_env_combine())
										{
										glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
										glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);
										}
									else if(Extensions::IsEnabled_GL_EXT_texture_env_add())
										{
										glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
										}
									glActiveTextureARB( GL_TEXTURE1_ARB);
									glPushAttrib(GL_TEXTURE_BIT);
									glEnable(GL_TEXTURE_2D);
									if(Extensions::IsEnabled_GL_EXT_texture_env_combine())
										{
										glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
										glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
										}
									else if(Extensions::IsEnabled_GL_EXT_texture_env_add())
										{
										glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
										}

									this->unit1.Begin();	

									glActiveTextureARB( GL_TEXTURE0_ARB);
									}
								}
							}
						else
							{
							glPushAttrib(GL_ENABLE_BIT);
							glDisable(GL_TEXTURE_2D);
							}
						}
					inline void End() const
						{
						if( this->unit0 )
							{
							this->unit0.End();

							if( this->unit1 )
								{
								if( Extensions::IsEnabled_GL_ARB_multitexture() )
									{
									glActiveTextureARB( GL_TEXTURE1_ARB);
									this->unit1.End();
									glPopAttrib();
									glActiveTextureARB( GL_TEXTURE0_ARB);

									if(Extensions::IsEnabled_GL_EXT_texture_env_combine())
										{
										glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
										glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
										}
									else if(Extensions::IsEnabled_GL_EXT_texture_env_add())
										{
										glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
										}
									}
								}
							}
						else
							{
							glPopAttrib();
							}
						}
					inline bool Done()
						{
						if( this->time > this->stageTime)
							{
							this->time = 0;
							return true;
							}
						return false;
						}
				private:

					class Unit
						{
						friend class ShaderLib;
						public:
							void Init();
							void Destroy();

							inline void Update( float msec)
								{
								this->time += msec;
								}
							inline void Begin() const
								{
								glPushAttrib( GL_ENABLE_BIT);
								glBindTexture( GL_TEXTURE_2D, this->texture);

								if( this->flags == Flags::FLAG_NOFLAGS )
									return;

								// blending
								if( this->flags & Flags::FLAG_BLEND )
									{
									glEnable( GL_BLEND);

									// color blending
									if( this->flags & Flags::FLAG_BLEND_COLOR )
										glBlendFunc( GL_DST_COLOR, GL_SRC_COLOR);
									else
										{
										// alpha blending
										if( this->flags & Flags::FLAG_BLEND_ALPHA )
											glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

										// alpha timer
										if( this->flags & Flags::FLAG_ALPHA_TIMER )
											{
											glColor4f( 1, 1, 1, Abs<float>( this->alphaFactor* cos( this->time/1000.0f)) );
											}
										}
									}

								// envirnoment mapping
								if( this->flags & Flags::FLAG_ENVMAP )
									{
									if(DGL::Extensions::IsEnabled_GL_NV_texgen_reflection())
										{
										glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
										glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
										}
									else
										{
										glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
										glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
										}
									glEnable( GL_TEXTURE_GEN_S);
									glEnable( GL_TEXTURE_GEN_T);
									}

								// texture transformation
								if( this->flags & Flags::FLAG_TEXGEN_FUNCTION )
									{
									glMatrixMode(GL_TEXTURE);
									glPushMatrix();

									const float seconds = this->time / 1000.0f;
									float s = this->texgenFactorS;
									float t = this->texgenFactorT;
									
									// linear
									if( this->flags & Flags::FLAG_TEXGEN_LINEAR_S )
										s *= seconds;
									if( this->flags & Flags::FLAG_TEXGEN_LINEAR_T )
										t *= seconds;

									// sin
									if( this->flags & Flags::FLAG_TEXGEN_SIN_S )
										s *= sin( seconds );
									if( this->flags & Flags::FLAG_TEXGEN_SIN_T )
										t *= sin( seconds );

									// cos
									if( this->flags & Flags::FLAG_TEXGEN_COS_S )
										s *= cos( seconds );
									if( this->flags & Flags::FLAG_TEXGEN_COS_T )
										t *= cos( seconds );

									glTranslatef( s, t, 0);

									// scale
									if( this->flags & Flags::FLAG_TEXGEN_SCALE )
										glScalef( s, t, 1);

									// rorate
									if( this->flags & Flags::FLAG_TEXGEN_ROTATE )
										glRotatef( this->time * this->texgenFactorRotate, 0,0,1 );

									glMatrixMode(GL_MODELVIEW);
									}
								}
							inline void End() const
								{
								glPopAttrib();
								if( this->flags & Flags::FLAG_TEXGEN_FUNCTION )
									{
									glMatrixMode(GL_TEXTURE);
									glPopMatrix();
									glMatrixMode(GL_MODELVIEW);
									}
								}
							inline operator bool() const
								{
								return static_cast<bool>( (GLuint) this->texture );
								}


							enum Flags
								{
								FLAG_NOFLAGS			= 0x0000,
								FLAG_BLEND				= 0x0001,
								FLAG_BLEND_COLOR		= 0x0002,
								FLAG_BLEND_ALPHA		= 0x0004,
								FLAG_ALPHA_TIMER		= 0x0008,
								FLAG_ENVMAP				= 0x0010,
								FLAG_TEXGEN_FUNCTION	= 0x0020,
								FLAG_TEXGEN_LINEAR_S	= 0x0040,
								FLAG_TEXGEN_LINEAR_T	= 0x0080,
								FLAG_TEXGEN_SIN_S		= 0x0100,
								FLAG_TEXGEN_SIN_T		= 0x0200,
								FLAG_TEXGEN_COS_S		= 0x0400,
								FLAG_TEXGEN_COS_T		= 0x0800,
								FLAG_TEXGEN_SCALE		= 0x1000,
								FLAG_TEXGEN_ROTATE		= 0x2000,
								FLAG_TEXTURE_EDGE_CLAMP	= 0x4000
								};
								
							private:
								unsigned char	flags;
								char			mapName[64];
								float			alphaFactor;
								float			texgenFactorS;
								float			texgenFactorT;
								float			texgenFactorRotate;
							
								// this
								float			time;
								Texture			texture;
							public:
								Unit();

								// commands
								void SetMap( const char *param);
								void AddFlag( const char *param);							
								void AlphaFactor( float param);
								void TexgenFactorS( float param);
								void TexgenFactorT( float param);
								void TexgenFactorRotate( float param);

						};

					float			time;
					float			stageTime;
					Stage			*next;
					Unit			unit0, unit1;
				public:
					Stage();
					~Stage();
				};

			char			name[128];
			Stage			*stageList;
			Stage			*activeStage;
			bool			inUse;
		public:
			Shader(): stageList(NULL), inUse(false), activeStage(NULL)
				{
				*this->name = '\0';
				}
		};


	class ShaderLib
		{
	public:

		void			LoadLibrary(const char *filename);
		void			DestroyLibrary();
		const Shader*	GetShaderForTexture( const char *texName);

		void			InitShaders();
		void			UpdateShaders(float msec);
		void			DestroyShaders();

		ShaderLib();
		~ShaderLib();
	private:
		Shader			shaders[1024];
		int				currentFree;

		void		analyze(const char *buffer);
		void		analyzeStages(const char *shaderBuffer);
		void		addStage( const char *stageBuffer);
		void		parseUnitCommands( const char *stageCommands, Shader::Stage::Unit *unit);
		void		executeCommand( const char * command, const char * param, Shader::Stage::Unit *unit);
		bool		cleanup( const char *buffer, int size, char *outBuffer) const;
		const char *getChildBlock( const char *buf, char *bufOut) const;
		const char *getNextCommand( const char* cmdBuff, char *stringCommand) const;
		};
	}

#endif // ___DGL_SHADER_H