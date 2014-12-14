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

#include "DE_Config.h"
#ifdef DE_SCENE
#undef DE_SCENE

#include "DShared.H"
#include "DSys.H"
#include "DGL.H"
#include "DUI_Main.H"

#include "Demo_Camera.h"
#include "Demo_Effect.h"
#include "Demo_Scene.h"
#include "Demo_Registery.h"

// Demos
#include "DE_IntroScene.h"


//////////////////////////////////////////
// Scene Objects

namespace objects
	{
	enum objects_e
		{
		no_object,
		wall_bottom,
		wall_front,
		wall_left,
		wall_right,
		wall_back
		};
	}

#include "DE_Lighting_fx.h"
#include "DE_TerrainScene.h"
#include "DE_GetMrFrench.h"
//#include "DE_CG.h"


//////////////////////////////////////////
// Shared variables


DSys::Var_float	gl_fov				("gl_Fov",				"90");
DSys::Var_float	gl_near				("gl_Near",				"2");
DSys::Var_float	gl_far				("gl_Far",				"25");
DSys::Var_bool	gl_drawFaces		("gl_drawfaces",		"1");
DSys::Var_bool	gl_avi				("gl_avi",				"0");

DSys::Var_bool	r_drawFPS			("r_drawFPS",			"1");
DSys::Var_bool	r_reflections		("r_reflections",		"1");
DSys::Var_bool	r_loopDemo			("r_loopDemo",			"1");
DSys::Var_bool	r_drawTimer			("r_drawTimer",			"0");
DSys::Var_bool	r_modelBounds		("r_modelBounds",		"0");
DSys::Var_bool	r_Lighting			("r_lighting",			"0");
DSys::Var_bool	r_VSync				("r_VSync",				"1");

DSys::Var_int	g_speed				("g_speed",				"1");
DSys::Var_int	g_cameraMotion		("g_cameraMotion",		"0");


///////////////////////////////////////////
// User Interface Declerations

void UI_Init();
void UI_OnLeftClick(int button);
void UI_OnRightClick(int button);
void UI_Update(float msec);
void UI_OnKey(char key);
void UI_Render();

//////////////////////////////////////////
// Scene Mode
enum HScene_Mode {
	MODE_PLAYBACK,
	MODE_UI
} SceneMode;

//////////////////////////////////////////
// Macros

#define AT_KEY_DOWN(key) if(DSys::Input::KeyDown((key)))
#define AT_KEY_CHAR(key) if(DSys::Input::KeyChar((key)))
#define CHECK_QUIT		{	DSys::Input::Update();\
							if(DSys::Input::KeyDown(DIK_ESCAPE)){\
							DSys::SendConsoleCommand("quit");\
							DMacro_TraceLeave();\
							return false;}\
}

//////////////////////////////////////////
// Scene Objects

static DGL::LoadingScreen		g_loadingScreen;
static DAudio::Sound2D			g_bakmusic;
static DGL::ParticleEngine		g_fireEngine;
static DGL::FadeScreen			g_fadeOut;
static DGL::FadeScreen			g_fadeIn;
static DGL::UserMessage			g_userMessage;
static DGL::Font3D				g_font3D;
static DGL::Font				g_font;
static GLFT_Font				g_fontft;
static DGL::Texture				g_refMap;
static DGL::Texture				g_menuBack;
static float					g_timeCounter;
static DGL::Camera				g_camera;
static DGL::Train				g_train;
static bool						g_trainUpdate;
static bool						g_bQuitting;

static Horror::IntroScene		*g_introScene;
static DGL::Lighting_fx			*g_lightingScene;
static Horror::TerrainScene		*g_terrainScene;
static GetMrFrench::Game		*g_mrFrenchGame;
//static cg::cgDemo				*g_cgDemo;
static Demo::Registery			g_registery;




//////////////////////////////////////////
// Final Scene Functions


/* Add New Console Commands Here */
void AddConsoleCommands(){
}

bool Init(void){
	DMacro_TraceEnter(Init);

	//
	// OpenGL
	//
	DGL::Font::Init();
	// Texture
	glEnable( GL_TEXTURE_2D);

	g_loadingScreen.Init();

	CHECK_QUIT;
	g_loadingScreen.SetStage( 5, "Initializing OpenGL ...");
		DGL::InitQuadrics();
		DSys::Physics::Init();
		DGL::Extensions::Init();
//		g_userMessage.SetFont(g_font);

		if(DGL::Extensions::IsEnabled_WGL_EXT_swap_control()){
			DSys::Logger::Print("Setting monitor v-sync ...");
			wglSwapIntervalEXT(r_VSync);
		}
		if(DGL::Extensions::IsEnabled_GL_EXT_clip_volume_hint()){
			DSys::Logger::Print("Enabling volume clipping ...");
			glHint( GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_NICEST);
		}
		
		if(DGL::Extensions::IsEnabled_GL_EXT_point_parameters()){
			DSys::Logger::Print("Setting point parameters ...");
//			glPointSize(5);
//			glPointParameterfEXT( GL_POINT_SIZE_MIN_EXT, 1);
//			glPointParameterfEXT( GL_POINT_SIZE_MAX_EXT, 10);
//			glPointParameterfEXT( GL_POINT_FADE_THRESHOLD_SIZE_EXT, 50);
//			glPointParameterfEXT( GL_DISTANCE_ATTENUATION_EXT, 60);
		}
		
		// Depth buffer
		glClearDepth( 20.0f);
		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LEQUAL );
		
#if 0
		if(DGL::Extensions::IsEnabled_GL_NV_texgen_reflection()){
			glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
			glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
		} else {
			glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		}
#endif

		// shade model
//		glShadeModel(GL_SMOOTH);
		
		// clear color
//		glClearColor(0,0,0,1);
		
		// hints
//		glHint( GL_PERSPECTIVE_CORRECTION_HINT,	GL_NICEST);
//		glHint( GL_POINT_SMOOTH_HINT,			GL_NICEST);
//		glHint( GL_LINE_SMOOTH_HINT,			GL_NICEST);
//		glHint( GL_POLYGON_SMOOTH_HINT,			GL_NICEST);
		
		// polygon mode
//		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );		// Back Face Is Solid

		glEnable( GL_LIGHTING );
//		glDisable( GL_CULL_FACE );
		g_font.Create( "Font");
		g_fontft.open("arial.ttf",16);
		glLoadIdentity();

	//
	// Media
	//
	CHECK_QUIT;
	g_loadingScreen.SetStage(10, "Loading Media...");
#ifndef _DEBUG
	if(!DSys::sv_bDeveloper)
//		g_bakmusic.Load( "sounds/music/The Prophecy.mp3");
//		g_bakmusic.Load( "sounds/music/Lothlorien.mp3");
//		g_bakmusic.Load( "sounds/music/Inshallah.mp3");
#endif

	// User interface
	CHECK_QUIT;
	g_loadingScreen.SetStage(20, "Loading User Interface...");
	UI_Init();

	CHECK_QUIT;
	g_loadingScreen.SetStage(30, "Building Particle Systems ...");
	g_fireEngine.Init(
					Vector(0,-5,-20),
					Vector(0,2,0),
					Vector(0,0,0),
					DGL::Color::ColorYellow(),
					DGL::Color::ColorRed(),
					0.5f,
					3,
					0);

	CHECK_QUIT;
	g_loadingScreen.SetStage(50, "Building 3D Font...");
//	g_font3D.Build("Gaze Bold", DSys::Window::GetDC());
/*/
	CHECK_QUIT;
	g_loadingScreen.SetStage(70, "Initializing MrFrench...");
	g_mrFrenchGame = new GetMrFrench::Game;
	g_registery.Register(g_mrFrenchGame, NULL);
//
	CHECK_QUIT;
	g_loadingScreen.SetStage(60, "Initializing Intro Scene ...");
	g_introScene = new Horror::IntroScene;
	g_registery.Register(g_introScene, NULL);
/**/
	CHECK_QUIT;
	g_loadingScreen.SetStage(75, "Initializing desktop ...");
	g_lightingScene = new DGL::Lighting_fx;
	g_registery.Register( g_lightingScene, DSys::sv_bDeveloper ? NULL : "lfx");
/*/
	CHECK_QUIT;
	g_loadingScreen.SetStage(80, "Initializing Terrain Scene ...");
	g_terrainScene = new Horror::TerrainScene;
	g_registery.Register(g_terrainScene, DSys::sv_bDeveloper ? NULL : "TerrainScene");
//
	CHECK_QUIT;
	g_loadingScreen.SetStage(93, "Initializing cg shaders demo...");
	g_cgDemo = new cg::cgDemo;
	g_registery.Register(g_cgDemo, NULL);

/**/
	CHECK_QUIT;
	g_loadingScreen.SetStage(95, "Initializing Effects...");
	// fade effect
	g_fadeIn.Init( DGL::Color(0.0f,0.0f,0.0f,1.0f), DGL::Color(0.0f,0.0f,0.0f,0.0f), 1000);
	g_fadeOut.Init( DGL::Color(0.0f,0.0f,0.0f,0.0f), DGL::Color(0.0f,0.0f,0.0f,1.0f), 2000);
//	g_refMap.Build("textures/ref.tga");
//	g_menuBack.Build("textures/menuBack.jpg");

	CHECK_QUIT;
//	g_loadingScreen.SetStage(100, "Awaiting Snapshot...");


	///////////////
	// now start the back music
//	g_bakmusic.Play();

	///////////////
	// setup initial camera location
	g_camera.Set( g_camera.POSITION, Vector( 0, 4, -4 ) ); 

	DMacro_TraceLeave();
	return true;
}

void Update(float msec){
	DMacro_TraceEnter(Update);

	DSys::Input::Update();
	DSys::Physics::Update(msec/1000.0f);

	/*
	AT_KEY_CHAR(DIK_F10) DSys::SendConsoleCommand("quit");
	
	if( DSys::sv_bDeveloper )
		{
		AT_KEY_CHAR(DIK_ESCAPE)
			(void ( SceneMode == MODE_UI ? SceneMode = MODE_PLAYBACK : SceneMode = MODE_UI ) );
		}
	else
		{
		AT_KEY_CHAR(DIK_ESCAPE) 	g_bQuitting = true;
		}

	*/
	if(SceneMode == MODE_PLAYBACK) {

		if(DSys::Input::KeyDown(DIK_LALT) || DSys::Input::KeyDown(DIK_RALT)){
			////////////////
			//************//
			//* ALT KEYS *//
			//************//
			////////////////

			if(DSys::sv_bDeveloper)
			{
				AT_KEY_CHAR(DIK_A){
					Vector p,d;
					g_camera.Get(g_camera.POSITION, p);
					g_camera.Get(g_camera.DIRECTION, d);
					g_train.PushNode(p,d);
				}

				AT_KEY_CHAR(DIK_Z){
					DSys::Logger::Print("Turning camera train: %s", g_trainUpdate ? "OFF" : "ON");
					g_trainUpdate = !g_trainUpdate;
				}

				AT_KEY_CHAR(DIK_S){
					static int i;
					g_train.Dump(va("autosave%d", i++));
				}

				AT_KEY_CHAR(DIK_C){
					g_train.Destroy();
				}

				AT_KEY_CHAR(DIK_X){
					g_train.RemoveLast();
				}
			}


		} else {
			///////////////////
			//***************//
			//* NORMAL KEYS *//
			//***************//
			///////////////////


			if(!gl_avi){
				AT_KEY_CHAR(DIK_F12)	DSys::SendConsoleCommand("screenshot");
			} else {
				DSys::SendConsoleCommand("screenshot");
			}


			if(DGL::r_freeCam)
				{
				/////////////
				// Camera movement
				AT_KEY_DOWN(DIK_W)		g_camera.MoveCameraFarward((DSys::Input::KeyValue(DIK_W)/10000.0f) * g_speed.floatval() * msec );
				AT_KEY_DOWN(DIK_S)		g_camera.MoveCameraFarward(-(DSys::Input::KeyValue(DIK_S)/10000.0f) * g_speed.floatval() * msec );
				AT_KEY_DOWN(DIK_D)		g_camera.MovePlaneRight((DSys::Input::KeyValue(DIK_D)/10000.0f) * g_speed.floatval() * msec );
				AT_KEY_DOWN(DIK_A)		g_camera.MovePlaneRight(-(DSys::Input::KeyValue(DIK_A)/10000.0f) * g_speed.floatval() * msec );
				AT_KEY_DOWN(DIK_SPACE)	g_camera.MovePlaneUp((DSys::Input::KeyValue(DIK_SPACE)/10000.0f) * g_speed.floatval() * msec );
				AT_KEY_DOWN(DIK_C)		g_camera.MovePlaneUp(-(DSys::Input::KeyValue(DIK_C)/10000.0f) * g_speed.floatval() * msec );

				g_camera.RotateRight( DSys::Input::MouseXDelta() /200.0f );
				g_camera.RotateUp( -DSys::Input::MouseYDelta() /200.0f );
				}

		}


		g_registery.Update(msec);
		/*
		if(g_registery.Done())
			DSys::SendConsoleCommand("quit");

		if(DSys::sv_bDeveloper)
		{
			if(g_trainUpdate){
				Vector p,d;
				g_train.Update(msec,p,d);
				g_camera.Set(g_camera.POSITION, p);
				g_camera.Set(g_camera.DIRECTION, d);
			}
		}
		*/

	} else if( SceneMode == MODE_UI ){
		// user interface should think now
		UI_Update(msec);
	}

	///////////////////////
	//*******************//
	//* Non-Key Related *//
	//*******************//
	///////////////////////

//	static float sceneTime;
//	if(sceneTime>2){
//		sceneTime += msec/1000.0f;
//		g_userMessage.ShowMessage("Master Is Teh Tant = 6an6", 3,g_userMessage.STYLE_POPUP,6,DGL::Color::ColorGray());
//	}

//	g_fireEngine.Update(msec);
//	g_userMessage.Update(msec);
//	g_timeCounter += msec/50.0f;

//	g_fadeIn.Update(msec);
//	if(g_bQuitting)
//		g_fadeOut.Update(msec);

	DMacro_TraceLeave();
}



void Render3D(){
	DMacro_TraceEnter(Render3D);

	if( r_Lighting )
		glEnable( GL_LIGHTING );
	else
		glDisable( GL_LIGHTING );

	// render the scene
//	if(DSys::sv_bDeveloper)
//		g_camera.Setup();

	g_registery.Render();
	DSys::Physics::Render();

	DMacro_TraceLeave();
}


void Render2D(float msec){			// all 2d drawings happens here
	DMacro_TraceEnter(Render2D);

	DGL::Color::ColorWhite().MakeCurrent();
	DGL::MatrixOp::Ortho::Begin();
//==============================================================

	if(r_drawFPS.intval())
		{
		g_font.SetStyle(g_font.STYLE_SHADOWS|g_font.STYLE_RIGHT);
		g_font.SetColor(DGL::Color::ColorGray());
//			g_font.Print(638,468,	"%ifps", DGL::GetFPS(msec));
		std::stringstream str;
		str << DGL::GetFPS(msec)  << " fps";
		
	
		g_fontft.drawText(10,10, str.str() );
		}

	if(r_drawTimer.intval()){
		static float totaltime;
		static char	 *timer;
		totaltime+= msec;
		g_font.SetStyle(g_font.STYLE_SHADOWS|g_font.STYLE_RIGHT);
		g_font.SetColor(DGL::Color::ColorGray());
		g_font.Print(10, 20, "%s", D_Msec2String(totaltime));
	}

//		g_font.SetColor( DGL::Color::ColorCyan());
//		g_font.SetStyle( g_font.STYLE_SHADOWS|g_font.STYLE_LEFT);
//		g_font.Print( 20, 460, "Hello %s", DSys::sv_sUserName.strval());
//		g_font.Print( 20, 470, "DaherEngine \"%s\"", SceneTitle.strval());


		if(SceneMode == MODE_UI)
		{
			UI_Render();
		}
		else
		{
//			g_userMessage.Render();
//			g_fadeIn.Render();
//			if(g_bQuitting)
//				if(!g_fadeOut.Render())
//					DSys::SendConsoleCommand("quit");
		}

//===========================================================

	DGL::MatrixOp::Ortho::End();

	DMacro_TraceLeave();
}


void Shutdown(){
	g_registery.Destroy();
	delete g_introScene;
	delete g_terrainScene;
//	delete g_cgDemo;

	DUI::Destroy();
	g_bakmusic.Destroy();

	g_refMap.Delete();
	g_menuBack.Delete();

}














//////////////////////////
// User Interface


/* Add UI Button ID Here */
enum UI_Button_e {
	UIB_CONTINUE,		// continues to the scene
	UIB_RESTART_CURRENT,// restarts the current scene
	UIB_RESTART_ALL,	// restarts the whole demo
	UIB_END_CURRENT,	// ends the current scene (skip)
	UIB_START_MUSIC,	// starts music
	UIB_CONSOLE,		// show console

	UIB_PUSH_CURRENT,	// stores the current camera node
	UIB_REMOVE_LAST,	// remove the last node
	UIB_TOGGLE_TRAIN,	// toggles camera train
	UIB_SAVE_TRAIN,		// saves train data
	UIB_CLEAR_TRAIN,	// clears train data

	UIB_EXIT,			// exits scene
	UIB_NUMBER
};


static DUI_MenuButton	ui_buttons[ UIB_NUMBER];

#define MENU_Y				200
#define BUTTON_H			15
#define BUTTON_X			320
#define BUTTON_Y(id)		MENU_Y - (id) * BUTTON_H

void UI_Init(){
	DUI::Init();

	DUI::CreateObject( &ui_buttons[UIB_CONTINUE], "Start Scene", BUTTON_X, BUTTON_Y(UIB_CONTINUE), 60, 30, "CONTINUE");
	DUI::CreateObject( &ui_buttons[UIB_RESTART_CURRENT], "Restart Current", BUTTON_X, BUTTON_Y(UIB_RESTART_CURRENT), 60, 30, "RESTART CURRENT");
	DUI::CreateObject( &ui_buttons[UIB_RESTART_ALL], "Restart Demo", BUTTON_X, BUTTON_Y(UIB_RESTART_ALL), 60, 30, "RESTART ALL");
	DUI::CreateObject( &ui_buttons[UIB_END_CURRENT], "End Current", BUTTON_X, BUTTON_Y(UIB_END_CURRENT), 60, 30, "END CURRENT");
	DUI::CreateObject( &ui_buttons[UIB_START_MUSIC], "Play", BUTTON_X, BUTTON_Y(UIB_START_MUSIC), 60, 30, "PLAY MUSIC");
	DUI::CreateObject( &ui_buttons[UIB_CONSOLE], "Console", BUTTON_X, BUTTON_Y(UIB_CONSOLE), 60, 30, "TOGGLE CONSOLE");
	DUI::CreateObject( &ui_buttons[UIB_EXIT], "Exit", BUTTON_X, BUTTON_Y(UIB_EXIT), 60, 30, "EXIT TO SYSTEM");

	DUI::CreateObject( &ui_buttons[UIB_PUSH_CURRENT], "Push Current", BUTTON_X, BUTTON_Y(UIB_PUSH_CURRENT), 60, 30, "PUSH CURRENT", DSys::sv_bDeveloper);
	DUI::CreateObject( &ui_buttons[UIB_REMOVE_LAST], "Remove Last", BUTTON_X, BUTTON_Y(UIB_REMOVE_LAST), 60, 30, "REMOVE LAST", DSys::sv_bDeveloper);
	DUI::CreateObject( &ui_buttons[UIB_TOGGLE_TRAIN], "Toggle Train", BUTTON_X, BUTTON_Y(UIB_TOGGLE_TRAIN), 60, 30, "TOGGLE TRAIN", DSys::sv_bDeveloper);
	DUI::CreateObject( &ui_buttons[UIB_SAVE_TRAIN], "Save Train", BUTTON_X, BUTTON_Y(UIB_SAVE_TRAIN), 60, 30, "SAVE TRAIN", DSys::sv_bDeveloper);
	DUI::CreateObject( &ui_buttons[UIB_CLEAR_TRAIN], "Clear Train", BUTTON_X, BUTTON_Y(UIB_CLEAR_TRAIN), 60, 30, "CLEAR TRAIN", DSys::sv_bDeveloper);
}

void UI_OnLeftClick(int button){
	switch (button) {
		case UIB_CONTINUE:
			SceneMode = MODE_PLAYBACK;
			break;
		case UIB_RESTART_CURRENT:
			g_registery.RestartCurrent();
			SceneMode = MODE_PLAYBACK;
			break;

		case UIB_RESTART_ALL:
			g_registery.RestartAll();
			SceneMode = MODE_PLAYBACK;
			break;
		case UIB_END_CURRENT:
			g_registery.EndCurrent();
			SceneMode = MODE_PLAYBACK;
			break;
		case UIB_START_MUSIC:
//			Play(&g_bakmusic, 1);
			break;
		case UIB_CONSOLE:
			DSys::SendConsoleCommand("toggle");
			break;
		case UIB_EXIT:
			{
#ifdef _DEBUG
				DSys::SendConsoleCommand("quit");
#endif
				g_bQuitting = true;
				SceneMode = MODE_PLAYBACK;
				g_userMessage.ShowMessage("Good Bye",3,g_userMessage.STYLE_POPUP,3,DGL::Color::ColorGray());
			}
			break;


		case UIB_PUSH_CURRENT:
			{
				g_userMessage.ShowMessage("Node Pushed", 2, g_userMessage.STYLE_FADE,2,DGL::Color::ColorCyan());
				Vector p,d;
				g_camera.Get(g_camera.POSITION, p);
				g_camera.Get(g_camera.DIRECTION, d);
				g_train.PushNode(p,d);

				SceneMode = MODE_PLAYBACK;
			}
			break;
		case UIB_REMOVE_LAST:
			{
				if(g_train.RemoveLast())
					g_userMessage.ShowMessage("Last Removed", 2, g_userMessage.STYLE_FADE,2,DGL::Color::ColorCyan());
				else
					g_userMessage.ShowMessage("Error: No more nodes", 2, g_userMessage.STYLE_FADE,2,DGL::Color::ColorRed());
				SceneMode = MODE_PLAYBACK;
			}
			break;
		case UIB_TOGGLE_TRAIN:
			{
				DSys::Logger::Print("Turning camera train: %s", g_trainUpdate ? "OFF" : "ON");
				g_userMessage.ShowMessage("Train %s", 2, g_userMessage.STYLE_FADE,2,DGL::Color::ColorCyan(), g_trainUpdate? "OFF" : "ON");
				g_trainUpdate = !g_trainUpdate;
				SceneMode = MODE_PLAYBACK;
			}
			break;
		case UIB_SAVE_TRAIN:
			{
				static int i;
				g_train.Dump(va("autosave%d", i));
				g_userMessage.ShowMessage("Saved to %s", 2, g_userMessage.STYLE_FADE,2,DGL::Color::ColorCyan(), va("autosave%d", i++));
				SceneMode = MODE_PLAYBACK;
			}
			break;
		case UIB_CLEAR_TRAIN:
			{
				g_userMessage.ShowMessage("Train Cleared", 2, g_userMessage.STYLE_FADE,2,DGL::Color::ColorCyan());
				g_train.Destroy();
				SceneMode = MODE_PLAYBACK;
			}
			break;
	}
}

void UI_OnRightClick(int button){
}

void UI_Update(float msec) { static int i;	
	static bool MouseLeftClicked;
	static bool MouseRightClicked;

	MouseLeftClicked = DSys::Input::MouseChar(0);
	MouseRightClicked= DSys::Input::MouseChar(1);

	DUI::MouseMoved(DSys::Input::MouseXDelta(), DSys::Input::MouseYDelta());
	for(i=0; i< UIB_NUMBER; i++){
		DUI::SelectObject(&ui_buttons[i]);
		DUI::CheckMouseOver(msec);
		if(MouseLeftClicked)
			if(DUI::OnMouseLeftClick())
				UI_OnLeftClick(i);
		if(MouseRightClicked)
			if(DUI::OnMouseRightClick())
				UI_OnRightClick(i);
	}
}

void UI_OnKey(char key){ static int i;
	for(i=0; i< UIB_NUMBER; i++){
		DUI::SelectObject(&ui_buttons[i]);
		DUI::OnKey(key);
	}
}

void UI_Render(){	static int i;
	for(i=0; i< UIB_NUMBER; i++){
		DUI::SelectObject(&ui_buttons[i]);
		DUI::RenderObject();
	}

	DUI::MouseRender();
	
	for(i=0; i< UIB_NUMBER; i++){
		DUI::SelectObject(&ui_buttons[i]);
		DUI::RenderObjectToolTip();
	}
}


#endif // DE_SCENE

