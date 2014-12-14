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

#ifndef ___BOUNCINGBALLS_H
#define ___BOUNCINGBALLS_H


namespace DBouncingBalls {
	

		class Arena {
			public:
				const static int MinX = 20;
				const static int MaxX = 600;
				const static int MinY = 40;
				const static int MaxY = 450;
				const static int BorderSize = 2;

				static void DrawBorders(){
					DGL::Draw2DLine(MinX,MinY,MinX,MaxY,BorderSize,DGL::Color::ColorGray());
					DGL::Draw2DLine(MinX,MaxY,MaxX,MaxY,BorderSize,DGL::Color::ColorGray());
					DGL::Draw2DLine(MaxX,MaxY,MaxX,MinY,BorderSize,DGL::Color::ColorGray());
	//				DGL::Draw2DLine(MaxX,MinY,MinX,MinY,BorderSize,DGL::Color::ColorGray());
				}
		};


		class SoundFX {
			private:
				DAudio::Sound2D		hit;
				DAudio::Sound2D		launch;
				DAudio::Sound2D		roundStart;
				DAudio::Sound2D		roundEnd;
				DAudio::Sound2D		newLevel;

			public:
				void Init(){
					this->hit.Load("sounds/hit.wav");
					this->launch.Load("sounds/launch.wav");
					this->roundStart.Load("sounds/roundStart.wav");
					this->roundEnd.Load("sounds/roundEnd.wav");
					this->newLevel.Load("sounds/newLevel.wav");
				}
				void TargetHit(){
					this->hit.Play();
				}
				void BorderHit(){
					this->hit.Play();
				}
				void Launch(){
					this->launch.Play();
				}
				void RoundStart(){
					this->roundStart.Play();
				}
				void RoundEnd(){
					this->roundEnd.Play();
				}
				void NewLevel(){
					this->newLevel.Play();
				}
				void Destroy(){
					this->hit.Destroy();
					this->launch.Destroy();
					this->roundStart.Destroy();
					this->roundEnd.Destroy();
					this->newLevel.Destroy();
				}

		} soundFX;

		class Ball: public DGL::Ball {
			protected:
				GLuint		texture;
			public:
				void SetTexture(GLuint texture){
					this->texture = texture;
				}
			protected:
				void clip(){
					if( this->position.x_()+this->radius >= Arena::MaxX ){
						this->position.x_() = Arena::MaxX-this->radius-1;
						this->velocity.x_() *= -1;
						this->onClip();
					}
					if(	this->position.x_()-this->radius <= Arena::MinX ){
						this->position.x_() = Arena::MinX+this->radius+1;
						this->velocity.x_() *= -1;
						this->onClip();
					}
					if( this->position.y_()+radius >= Arena::MaxY ){
						this->position.y_() = Arena::MaxY-this->radius-1;
						this->velocity.y_() *= -1;
						this->onClip();
					}
				}
				void onClip(){
					soundFX.BorderHit();
				}
				void render(){
					DGL::Draw2DRect(
							this->position.x_()-this->radius,
							this->position.y_()-this->radius,
							this->radius * 2,
							this->radius * 2,
							this->texture);
				}
		};

		class TargetBall : public Ball {
			private:
				int		hitCounter;
				bool	dead;
			public:
				void Init(){
					hitCounter = 0;
					dead = false;
				}
				void Reset(){
					hitCounter = 0;
					dead = false;
				}
				bool Dead(){return dead;}
				int GetHitCounter(){return hitCounter;}
				void SetHitCounter(int count){hitCounter = count;} // developer cheat
				void UpdateRadius(float raduis){this->radius = raduis;}
			protected:
				void update(float msec){
					this->accelerate( msec/1000.0f);
					this->clip();
					if(this->position.y_() < Arena::MinY+5){
						dead = true;
						this->Unlink();
						soundFX.RoundEnd();
					}
				}
				void onCollision(){
					hitCounter++;
				}
		};

		class MessileBall : public Ball {
			private:
				const static int	radius = 8;
				const static int	mass = 10;
			
			public:
				void Init(){
				}
				void Reset(){
					this->Unlink();
				}
				void Shoot(int x){
					this->Unlink();
					this->Launch(
							this->radius,
							Vector(x,Arena::MinY+5,0),
							Vector(0,500,0),
							Vector(0,0,0),
							Vector(0,0,0),
							this->mass);
				}
			protected:
				void update(float msec){
					position += velocity * msec/1000.0f;
					this->clip();
				}
				void onCollision(){
					soundFX.TargetHit();
				}
		};

		class BallLauncher {
			private:
				int				xPosition;
				DGL::Texture	texture;
				MessileBall		messile;
				DGL::Texture	messileTex;
			public:

				void LoadTextures(){
					this->texture.Build("textures/launcher.tga");
					this->messileTex.Build("textures/messile.tga");
					this->messile.SetTexture(messileTex);
				}

				void Init(){
					this->xPosition = 320;
					this->messile.Init();
				}

				void Reset(){
					this->messile.Reset();
				}

				void Launch(){
					this->messile.Shoot(xPosition);
					soundFX.Launch();
				}

				void Move(int dx){
					this->xPosition += dx;
					Clamp<int>( Arena::MinX+5, Arena::MaxX-5, this->xPosition);
				}

				void Render(){
					DGL::Draw2DRect( this->xPosition-10, Arena::MinY-30,20,30, this->texture);
				}

				void Destroy(){
					this->texture.Delete();
					this->messileTex.Delete();
				}
		};

		class HeighestScore {
			private:
				int		numOfGames;
				int		scoreRecord;
			public:
				HeighestScore() : numOfGames(0), scoreRecord(0) { }
				void Init(){
					FILE *f = fopen("game.dat","rb");
					if(!f)	return;
					fread(this,sizeof(*this),1,f);
					fclose(f);
				}
				void NewGameStarted(){
					numOfGames++;
				}
				bool CheckNewRecord(int hits){
					if(hits>scoreRecord){
						scoreRecord = hits;
						if(numOfGames >1) // don't notify new score in the first game
							return true;
					}
					return false;
				}
				void Destroy(){
					FILE *f = fopen("game.dat","wb");
					if(!f){
						DSys::Logger::Error("Cannot save game data");
						return;
					}
					fwrite(this,sizeof(*this),1,f);
					fclose(f);
				}
				int GetNumOfGames(){ return numOfGames;}
				int GetScoreRecord(){ return scoreRecord;}
		};

	class Game: public Demo::Scene {
		private:
				const static int		targetMass	= 15;
				const static int		addTime		= 1000;
				const static int		dieDelay	= 2000;
				const static int		MaxLevels	= 7;
				const static int		hitLevels[MaxLevels];
				const static int		targetRadius[MaxLevels];

				TargetBall			target;
				BallLauncher		launcher;
				HeighestScore		heighestScore;
				DGL::UserMessage	userMsg;

				bool				firstTime;
				bool				gameOver;
				int					lives;
				int					level;
				float				addPrev;
				float				dieWait;
				bool				newRecordNotified;
				DGL::Font			font;
				DGL::Texture		targetTexture;
				DGL::Texture		texBouncingGame;

				void launchTarget(){
					target.Launch(	this->targetRadius[level],
									Vector(	(Random::Float()*(Arena::MaxX-Arena::MinX*3))+1.5*Arena::MinX,
													400,
													0),
													Vector(Random::Float()*30+20,Random::Float()*20+30,0),
									Vector(0,0,0),
									Vector(0,0,0),
									this->targetMass);
				}
		public:
			void Init(){
				Random::Randomize();
				
				this->targetTexture.Build("textures/target.tga");
				this->texBouncingGame.Build("textures/bouncinggame.jpg");
				this->target.SetTexture(targetTexture);
				this->launcher.LoadTextures();
				this->heighestScore.Init();
				this->font.Create("font");
				this->userMsg.SetFont(font);
				this->launcher.Init();
				this->target.Init();
				this->gameOver = true;
				this->firstTime = true;
				this->level = 0;
				soundFX.Init();
			}
			void StartNewGame(){
				if(!gameOver){
					target.Unlink();
				}

				this->firstTime = false;
				this->gameOver = false;
				this->lives = 3;
				this->level = 0;
				this->addPrev = 0;
				this->heighestScore.NewGameStarted();

				this->StartNewRound();
			}
			
			void Restart()
				{
				this->StartNewGame();
				}

			void StartNewRound()
			{
				// new round
				this->target.Reset();
				this->launcher.Reset();
				// reset score to the level reached
				this->target.UpdateRadius( this->targetRadius[level]);
				this->target.SetHitCounter( this->hitLevels[this->level]);

				this->newRecordNotified = false;
				this->launchTarget();
				this->dieWait = 0;

				// play the round start fx.
				soundFX.RoundStart();
			}

			void Update(float msec){
				if( !gameOver ){
					this->addPrev += msec;
					DGL::Ball::UpdateAll(msec);
					if(DSys::Input::MouseDown(0)){
						if(this->addPrev >= addTime){
							this->launcher.Launch();
							this->addPrev = 0;
						}
					}

					if(target.Dead()){
						if(!dieWait){
							lives --;
							if( lives < 0)
								gameOver = true;
							else
								userMsg.ShowMessage(
										"%d More To Go",
										1.5f,
										userMsg.STYLE_POPUP,
										2,
										DGL::Color::ColorYellow(),
										lives);
						}
						dieWait += msec;

						if(dieWait >= dieDelay)
							this->StartNewRound();
					}

					// check for new record
					if(this->heighestScore.CheckNewRecord(this->target.GetHitCounter())){
						if(!newRecordNotified){
							userMsg.ShowMessage("New Record",3,userMsg.STYLE_FADE,2,DGL::Color::ColorGreen());
							newRecordNotified = true;
						}
					}

					// check the level
					int newLevel = 0;
					for(int i=0; i< this->MaxLevels; i++)
					{
						if(this->target.GetHitCounter() >= this->hitLevels[i])
							newLevel = i;
					}
					if(newLevel != this->level)
					{// new level
						this->level = newLevel;

						this->userMsg.ShowMessage("Level %i", 3, userMsg.STYLE_POPUP, 2,DGL::Color::ColorRed(), this->level);
						this->target.UpdateRadius(this->targetRadius[level]);
						soundFX.NewLevel();
					}

					// developer cheat for debugging levels
					if(DSys::sv_bDeveloper)
						if(DSys::Input::KeyChar(DIK_INSERT))
							this->target.SetHitCounter(this->target.GetHitCounter()+10);

					userMsg.Update(msec);

					launcher.Move(DSys::Input::MouseXDelta());
				}
			}

			void Render(){
				if( !gameOver ){
					Arena::DrawBorders();
					launcher.Render();
					DGL::Ball::RenderAll();
					DGL::Font::Print2D(500,20,"Hits = %d",1,DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGold(),0,this->target.GetHitCounter());
					DGL::Font::Print2D(500,30,"Lives = %d",1,DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGold(),0,this->lives);
					DGL::Font::Print2D(500,10,"Record = %d",1,DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGold(),0,this->heighestScore.GetScoreRecord());
					DGL::Font::Print2D(500,465,"Level %i",1.5,DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGray(),0,this->level);

					userMsg.Render();
				} else {
					DGL::Draw2DRect( 320 - 120, 300, 256, 64, this->texBouncingGame);
					DGL::Font::Print2D(320,80,"Programming By Daher Alfawares",1,DGL::Font::STYLE_CENTER|DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGray(),0);
					DGL::Font::Print2D(320,70,"http://dee.digitalthinker.org",1,DGL::Font::STYLE_CENTER|DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGray(),0);
					DGL::Font::Print2D(320,170,"Press escape to go to menu",1,DGL::Font::STYLE_CENTER|DGL::Font::STYLE_SHADOWS,DGL::Color::ColorWhite(),0);
					if(!firstTime){
						DGL::Font::Print2D(320,210,"Game Over",2,DGL::Font::STYLE_CENTER|DGL::Font::STYLE_SHADOWS,DGL::Color::ColorYellow(),0);
						DGL::Font::Print2D(20,380,"Last score: %d",1,DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGold(),0,this->target.GetHitCounter());
						DGL::Font::Print2D(20,400,"Total played games: %d",1,DGL::Font::STYLE_LEFT|DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGreen(),0,this->heighestScore.GetNumOfGames());
						DGL::Font::Print2D(20,390,"Heighest score: %d",1,DGL::Font::STYLE_LEFT|DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGreen(),0,this->heighestScore.GetScoreRecord());
					}
				}
		}

		void Destroy(){
			this->heighestScore.Destroy();
			this->launcher.Destroy();
			this->targetTexture.Delete();
			this->texBouncingGame.Delete();
			soundFX.Destroy();
		}
	};

	const int Game::hitLevels[MaxLevels] = {
		0, 10, 25, 50, 75, 100, 125
	};
	const int Game::targetRadius[MaxLevels] = {
		30, 25, 20, 15, 10, 5, 3
	};
}


#endif // ___BOUNCINGBALLS_H