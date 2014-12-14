/*
-------------------------------------------
Daher Engine
------------

	$Author: Daher Alfawares $
	$Revision: 1.3 $
	$Date: 2004/07/31 19:22:01 $
	$Id: DGL_Shader.cpp,v 1.3 2004/07/31 19:22:01 Daher Alfawares Exp $
	$Name:  $

	Copyright (c) 2004, Daher Alfawares
-------------------------------------------
*/


#include "DGL_Main.h"
#include "DGL_Texture.h"
#include "DGL_Extensions.h"
#include "DGL_Shader.h"

#include <fstream>
#include <cstring>

namespace DGL {

	///////////////////////////////////////////
	// Shader

		void Shader::Init()
		{
			if( this->activeStage = this->stageList )
			{
				Stage *stage = this->stageList;
				while ( stage->next != this->stageList )
				{
					stage->Init();
					stage = stage->next;
				}
				stage->Init();
			}
		}
		void Shader::Destroy()
		{
			if( this->activeStage = this->stageList )
			{
				Stage *stage = this->stageList;
				while ( stage->next != this->stageList )
				{
					stage->Destroy();
					stage = stage->next;
				}
				stage->Destroy();
			}
		}

	///////////////////////////////////////////
	// Shader::Stage
		Shader::Stage::Stage(): next(NULL), stageTime(0), time(0)
		{
		}
		Shader::Stage::~Stage()
		{
			this->next = NULL;
			this->stageTime = 0;
			this->time = 0;
		}


		void Shader::Stage::Init()
		{
			this->time = 0;
			this->unit0.Init();
			this->unit1.Init();
		}
		void Shader::Stage::Destroy()
		{
			this->time = 0;
			this->unit0.Destroy();
			this->unit1.Destroy();
		}
	///////////////////////////////////////////
	// Shader::Stage::Unit
		Shader::Stage::Unit::Unit()
		{
			this->flags			= Flags::FLAG_NOFLAGS;
			this->alphaFactor	= 0;
			this->texgenFactorS = 0;
			this->texgenFactorT = 0;
			this->texgenFactorRotate = 0;
			this->time			= 0;
			this->mapName[0]	= '\0';
		}

		void Shader::Stage::Unit::Init()
		{
			this->time = 0;
			if( *this->mapName )
			{// Load the texture
				bool edgeClamp = bool( this->flags & Unit::Flags::FLAG_TEXTURE_EDGE_CLAMP );
				Texture::Filter filter( Texture::Filter::FilterName::TRILINEAR, edgeClamp);
				this->texture.Build( this->mapName, filter);
			}
		}

		void Shader::Stage::Unit::Destroy()
		{
			this->texture.Delete();
		}
		// commands
		void Shader::Stage::Unit::SetMap( const char *param)
		{
			strcpy(this->mapName, param);
		}

		void Shader::Stage::Unit::AddFlag( const char *param)
		{
			if( !strcmp("FLAG_NOFLAGS", param))			{ this->flags |= FLAG_NOFLAGS;			return;}
			if( !strcmp("FLAG_BLEND", param))			{ this->flags |= FLAG_BLEND;			return;}
			if( !strcmp("FLAG_BLEND_COLOR", param))		{ this->flags |= FLAG_BLEND_COLOR;		return;}
			if( !strcmp("FLAG_BLEND_ALPHA", param))		{ this->flags |= FLAG_BLEND_ALPHA;		return;}
			if( !strcmp("FLAG_ALPHA_TIMER", param))		{ this->flags |= FLAG_ALPHA_TIMER;		return;}
			if( !strcmp("FLAG_ENVMAP", param))			{ this->flags |= FLAG_ENVMAP;			return;}
			if( !strcmp("FLAG_TEXGEN_FUNCTION", param))	{ this->flags |= FLAG_TEXGEN_FUNCTION;	return;}
			if( !strcmp("FLAG_TEXGEN_LINEAR_S", param))	{ this->flags |= FLAG_TEXGEN_LINEAR_S;	return;}
			if( !strcmp("FLAG_TEXGEN_LINEAR_T", param))	{ this->flags |= FLAG_TEXGEN_LINEAR_T;	return;}
			if( !strcmp("FLAG_TEXGEN_SIN_S", param))	{ this->flags |= FLAG_TEXGEN_SIN_S;		return;}
			if( !strcmp("FLAG_TEXGEN_SIN_T", param))	{ this->flags |= FLAG_TEXGEN_SIN_T;		return;}
			if( !strcmp("FLAG_TEXGEN_COS_S", param))	{ this->flags |= FLAG_TEXGEN_COS_S;		return;}
			if( !strcmp("FLAG_TEXGEN_COS_T", param))	{ this->flags |= FLAG_TEXGEN_COS_T;		return;}
			if( !strcmp("FLAG_TEXGEN_SCALE", param))	{ this->flags |= FLAG_TEXGEN_SCALE;		return;}
			if( !strcmp("FLAG_TEXGEN_ROTATE", param))	{ this->flags |= FLAG_TEXGEN_ROTATE;	return;}
			if( !strcmp("FLAG_TEXTURE_EDGE_CLAMP", param)) { this->flags |= FLAG_TEXTURE_EDGE_CLAMP; return;}

			LogError("Unknown Flag: \"%s\" ... Ignouring...", param);
		}							
		void Shader::Stage::Unit::AlphaFactor( float param)
		{
			this->alphaFactor = param;
		}
		void Shader::Stage::Unit::TexgenFactorS( float param)
		{
			this->texgenFactorS = param;
		}
		void Shader::Stage::Unit::TexgenFactorT( float param)
		{
			this->texgenFactorT = param;
		}
		void Shader::Stage::Unit::TexgenFactorRotate( float param)
		{
			this->texgenFactorRotate = param;
		}
	
	///////////////////////////////////////////
	// Shader Library
		ShaderLib::ShaderLib(): currentFree(0)
		{
		}
		ShaderLib::~ShaderLib()
		{
			if ( this->currentFree )
				this->DestroyLibrary();
		}

		void ShaderLib::LoadLibrary( const char *filename)
		{
			
			std::ifstream inputFile( filename );
			if ( inputFile.fail())
			{
				LogPrint("Shader library \"%s\" was not found, falling to plain texture mode.", filename);
				return;
			}
			
			LogPrint("Compiling Shader Library \"%s\"", filename );
			
			inputFile.seekg( 0, std::ios::end );
			long fileSize = inputFile.tellg();
			inputFile.seekg( 0, std::ios::beg );
			
			char *tmpBuffer = new char[fileSize+1];
			inputFile.read( tmpBuffer, fileSize );
			tmpBuffer[fileSize] = EOF;
			inputFile.close();

			char *buffer = new char[fileSize];
			if( !this->cleanup(tmpBuffer, fileSize, buffer) )
			{
				delete [] tmpBuffer;
				delete [] buffer;
				return;
			}			
			delete [] tmpBuffer;

			this->analyze(buffer);			
			delete [] buffer;
		}

		void ShaderLib::DestroyLibrary()
		{
			for ( int i=0; i< this->currentFree; i++)
			{
				if( this->shaders[i].stageList )
				{
					Shader::Stage *stage = this->shaders[i].stageList, *tmp;
					while( stage->next != this->shaders[i].stageList )
					{
						tmp = stage;
						stage = stage->next;
						delete tmp;
					}
					this->shaders[i].stageList = NULL;
				}
			}
			this->currentFree = 0;
		}

		const Shader *ShaderLib::GetShaderForTexture( const char *texName)
		{
			for( int i=0; i< this->currentFree; i++)
			{
				if( !_strcmpi( texName, this->shaders[i].name) )
				{
					this->shaders[i].inUse = true;
					return & this->shaders[i];
				}
			}
			
			// Here we create a new shader
			Shader *shader = &this->shaders[this->currentFree++];
			strcpy( shader->name, texName );
			shader->stageList = new Shader::Stage;
			shader->stageList->next = shader->stageList;
			shader->stageList->unit0.SetMap( texName );
			shader->inUse = true;
			return shader;
		}

		void ShaderLib::InitShaders()
		{
			for( int i= 0; i< this->currentFree; i++)
			{
				if( this->shaders[i].inUse )
					this->shaders[i].Init();
			}
		}
		void ShaderLib::UpdateShaders(float msec)
		{
			for( int i=0; i< this->currentFree; i++)
			{
				if( this->shaders[i].inUse )
					this->shaders[i].Update(msec);
			}
		}
		void ShaderLib::DestroyShaders()
		{
			for( int i= 0; i< this->currentFree; i++)
			{
				if( this->shaders[i].inUse )
					this->shaders[i].Destroy();
			}
		}


		void ShaderLib::analyze( const char *buffer)
		{
			const char *nextShader	= buffer;
			char *shaderBuffer		= new char [1024];
			
			
			while( nextShader )
			{
				nextShader = this->getChildBlock(nextShader, shaderBuffer);
				int i=0;
				char *ptr = shaderBuffer;
				while( *ptr != '{' )
				{
					this->shaders[this->currentFree].name[i++] = *(ptr++);
				}
				this->shaders[this->currentFree].name[i] = '\0';
				LogPrint("Analysing shader: \"%s\"", this->shaders[this->currentFree].name);

				this->analyzeStages(shaderBuffer);
				this->currentFree++;
			}
			delete [] shaderBuffer;
		}

		void ShaderLib::analyzeStages( const char *shaderBuffer)
		{
			const char	*nextStage = shaderBuffer;
			char		*stageBuffer = new char [1024];
			int stage = 0;
			while( nextStage )
			{
				nextStage = this->getChildBlock( nextStage, stageBuffer);
				this->addStage( stageBuffer);
			}
			delete [] stageBuffer;
		}

		void ShaderLib::addStage( const char *stageBuffer)
		{
			Shader::Stage *stage;
			if( !this->shaders[currentFree].stageList )
			{
				this->shaders[currentFree].stageList = new Shader::Stage;
				stage = this->shaders[currentFree].stageList;
				stage->next = stage;
			} else
			{
				stage = this->shaders[currentFree].stageList;
				while ( stage->next != this->shaders[currentFree].stageList )
					stage = stage->next;

				stage->next = new Shader::Stage;
				stage		= stage->next;
				stage->next = this->shaders[currentFree].stageList;
			}

			char * unitCommands = new char[1024];

			// get the time of this stage
			char *timestring = new char[64];
			const char *nextUnit = stageBuffer;
			char *p = timestring;
			while( *nextUnit != ';' )
			{
				if( (p == timestring) && (*nextUnit == '{' || *nextUnit == '}' ))
				{
					LogError("Syntax Error, Stage time was not specified. in shader %s ... Ignouring...", this->shaders[currentFree].name);
					if( stage != this->shaders[currentFree].stageList )
					{
						Shader::Stage *s = this->shaders[currentFree].stageList;
						while( s->next != stage )
							s = s->next;
						s->next = stage->next;
					}

					delete stage;
					stage = NULL;
					return;
				}
				*(p++) = *(nextUnit++);
			}
			*p = '\0';
			nextUnit ++;
			if(!strstr(timestring, "Time"))
				LogError("Time was not specified, check case... Using 0.0");
			sscanf(timestring, "Time %f", & stage->stageTime);
			delete timestring;

			// get first unit
			nextUnit = this->getChildBlock(nextUnit, unitCommands);	
			this->parseUnitCommands(unitCommands, &stage->unit0);

			if( nextUnit )
			{// there is a seconds unit
				this->getChildBlock(nextUnit, unitCommands);
				this->parseUnitCommands(unitCommands, &stage->unit1);
			}
			delete [] unitCommands;
		}

		void ShaderLib::parseUnitCommands( const char *stageCommands, Shader::Stage::Unit *unit)
		{

			char command[64], param[64], tmp[1024];
			const char *nextCommand = stageCommands;
			while( nextCommand )
			{
				nextCommand = this->getNextCommand(nextCommand, tmp);
				sscanf(tmp, "%s %s", command, param);
				this->executeCommand( command, param, unit);
			}
		}

		void ShaderLib::executeCommand( const char * command, const char * param, Shader::Stage::Unit *unit)
		{
			LogPrint("Executing shader command: %s %s;", command, param);
			if ( !_strcmpi( command, "SetMap") )				{ unit->SetMap(param); return;}
			if ( !_strcmpi( command, "AddFlag") )			{ unit->AddFlag(param); return;}
			if ( !_strcmpi( command, "AlphaFactor") )		{ unit->AlphaFactor(atof(param)); return;}
			if ( !_strcmpi( command, "TexgenFactorS") )		{ unit->TexgenFactorS(atof(param)); return;}
			if ( !_strcmpi( command, "TexgenFactorT") )		{ unit->TexgenFactorT(atof(param)); return;}
			if ( !_strcmpi( command, "TexgenFactorRotate") )	{ unit->TexgenFactorRotate(atof(param)); return;}

			LogError("Unknown command: \"%s\"", command);
		}


		bool ShaderLib::cleanup( const char *buffer, int size, char *outBuffer) const
		{
			int bopen= 0, bclose= 0;
			int ln= 0;
			const char	*src = buffer;
			char		*dst = outBuffer;
			while ( *src != EOF )
			{
				if( *src == '\n' )
				{// ignour all spaces and tabs after the \n
					ln ++;
					src ++;
					while( *src == ' ' || *src == '\t' )
						src++;
					continue;
				}

				if( *src == ';' )
				{// ignour all spaces and tabs after the ';'
					*(dst++) = ';';
					src++;
					while( *src == ' ' || *src == '\t' )
						src++;
					continue;
				}

				if( *src == '/' )
				{// comments are ignoured
					if ( *(src+1) == '/' )
					{// to end of line comment
						while ( *src != EOF && *src != '\n' )
						{
							if( *(src++) == '\n' )
								ln ++;
						}
						src++;
						continue;
					}
					if ( *(src+1) == '*' )
					{// long comment, thus looking for */
						int line = 0;
						while( true )
						{
							if( *src == EOF )
							{
								LogError("Syntax Error (line:%d): Counldn't fine end of comment. End of file found.", ln);
								return false;
							}

							if( *src == '*' && *(src+1) == '/' )
							{
								src += 2;
								break;
							}

							src++;
							line++;
						}

						ln += line;
					}
				}

				if( *src == '\t' || *src == ' ' )
				{// ignour tabs
					do src ++; while( *src == '\t' || *src == ' ' );
					*(dst++) = ' ';
					continue;
				}

				if( *src == '{' )
					bopen ++;
				if( *src == '}' )
				{
					bclose ++;
					if( bclose > bopen )
					{
						LogError("Syntax Error, unexpected end of block '}' was found (line %d).", ln);
						return false;
					}
				}


				// clear to copy
				*(dst++) = *(src++);
			}
			*dst = EOF;

			if( bopen > bclose )
			{
				LogError("Syntax Error, could not find a matching '}'.");
				return false;
			}
			if( bopen < bclose )
			{
				LogError("Syntax Error, found '}' was not expected.");
				return false;
			}
			return true;
		}

		const char *ShaderLib::getChildBlock( const char *buf, char *bufOut) const
		{
			const char	*src = buf;
			char		*dst = bufOut;
			int			blocks = 0;
			bool		block = false;
			while( *src != EOF )
			{
				if( *src == '{' )
				{
					blocks ++;
					if(!block)
					{
						block = true;
						src++;
						continue;
					}
				}
				if( *src == '}' )
				{
					if( --blocks  == 0 )
						break;
				}

				if( block )
				{
					if( blocks > 0 )
					{
						*dst = *src;
						dst++;
					}
				}
				src++;
			}
			*dst = EOF;

			while ( true )
			{// find the next block
				if( *src == EOF )
					return NULL;
				if( *src == '{')
					return src;
				src++;
			}
		}

		const char *ShaderLib::getNextCommand( const char* cmdBuff, char *stringCommand) const
		{
			const char *ptr = cmdBuff;
			char *cmdPtr = stringCommand;
			while( *ptr != EOF && *ptr != '}' )
			{
				if(*ptr == ';')
				{
					*cmdPtr = '\0';
					break;
				}
				*(cmdPtr++) = *(ptr++);
			}
			ptr++; // pass over the ';'
			if( *ptr == EOF || *ptr == '}' )
				return NULL;

			return ptr;
		}

}