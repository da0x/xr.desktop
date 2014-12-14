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
#ifdef DE_TD
#undef DE_TD

#include "DShared.H"
#include "DSys.H"
#include "DGL.H"
#include "DE_TD.h"
#include "DUI_Main.H"

//////////////////////////////////////////
// Shared variables


DSys::Var_float	gl_fov				("gl_Fov",				"80");
DSys::Var_float	gl_near				("gl_Near",				"0.5");
DSys::Var_float	gl_far				("gl_Far",				"1000");
DSys::Var_bool	gl_drawFaces		("gl_drawfaces",		"1");
DSys::Var_bool	gl_avi				("gl_avi",				"0");

DSys::Var_bool	r_drawFPS			("r_drawFPS",			"1");
DSys::Var_bool	r_reflections		("r_reflections",		"1");
DSys::Var_bool	r_loopDemo			("r_loopDemo",			"1");
DSys::Var_bool	r_drawTimer			("r_drawTimer",			"0");
DSys::Var_bool	r_modelBounds		("r_modelBounds",		"0");

DSys::Var_int	g_speed				("g_speed",				"1");


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
enum HScene_Mode
{
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
							return false;}}

//////////////////////////////////////////
// Scene Objects

static DGL::LoadingScreen		g_loadingScreen;
static DAudio::Sound2D			g_bakmusic;
static DGL::FadeScreen			g_fadeOut;
static DGL::FadeScreen			g_fadeIn;
static DGL::Font				g_font;
static DGL::Texture				g_menuBack;
static float					g_timeCounter;
static bool						g_bQuitting;


//////////////////////////////////////////
// Scene Classes



//////////////////////////////////////////
// Final Scene Functions



/* Add New Console Commands Here */
void AddConsoleCommands()
{
}

bool Init(void)
{
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

		DGL::Extensions::Init();

		if(DGL::Extensions::IsEnabled_WGL_EXT_swap_control())
		{
			DSys::Logger::Print("Disabling monitor sync ...");
			wglSwapIntervalEXT(0);
		}
		if(DGL::Extensions::IsEnabled_GL_EXT_clip_volume_hint())
		{
			DSys::Logger::Print("Enabling volume clipping ...");
			glHint( GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_NICEST);
		}
		
		if(DGL::Extensions::IsEnabled_GL_EXT_point_parameters())
		{
			DSys::Logger::Print("Setting point parameters ...");
			glPointSize(5);
			glPointParameterfEXT( GL_POINT_SIZE_MIN_EXT, 1);
			glPointParameterfEXT( GL_POINT_SIZE_MAX_EXT, 10);
			glPointParameterfEXT( GL_POINT_FADE_THRESHOLD_SIZE_EXT, 50);
			glPointParameterfEXT( GL_DISTANCE_ATTENUATION_EXT, 60);
		}
		
		// Depth buffer
		glClearDepth( 100.0f);
//		glDepthFunc( GL_LEQUAL);
//		glEnable( GL_DEPTH_TEST);

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

		// shade model
		glShadeModel(GL_SMOOTH);
		
		// clear color
		glClearColor(0,0,0,1);
		
		// hints
		glHint( GL_PERSPECTIVE_CORRECTION_HINT,	GL_NICEST);
		glHint( GL_POINT_SMOOTH_HINT,			GL_NICEST);
		glHint( GL_LINE_SMOOTH_HINT,			GL_NICEST);
		glHint( GL_POLYGON_SMOOTH_HINT,			GL_NICEST);
		
		// polygon mode
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );		// Back Face Is Solid
		glDisable( GL_CULL_FACE);
		g_font.Create( "Font");
		glLoadIdentity();
		
	//
	// Media
	//
	CHECK_QUIT;
	g_loadingScreen.SetStage(10, "Loading Media...");
	if(!DSys::sv_bDeveloper)
		g_bakmusic.Load( "sounds/music/bouncing.mp3");

	// User interface
	CHECK_QUIT;
	g_loadingScreen.SetStage(40, "Loading User Interface...");
	UI_Init();

	CHECK_QUIT;
	g_loadingScreen.SetStage(60, "Initializing Bouncing Game ...");
	TD::gameServer = new TD::game_server;
	TD::gameServer->init();

	CHECK_QUIT;
	g_loadingScreen.SetStage(90, "Initializing Effects...");
	// fade effect
	g_fadeIn.Init( DGL::Color(0.0f,0.0f,0.0f,1.0f), DGL::Color(0.0f,0.0f,0.0f,0.0f),1000);
	g_fadeOut.Init( DGL::Color(0.0f,0.0f,0.0f,0.0f), DGL::Color(0.0f,0.0f,0.0f,1.0f),2000);
	g_menuBack.Build( "textures/menuBack.jpg");

	CHECK_QUIT;
	g_loadingScreen.SetStage(100, "Awaiting Snapshot...");


	///////////////
	// now start the back music
	g_bakmusic.Play();


	DMacro_TraceLeave();
	return true;
}

void Update(float msec)
{
	DMacro_TraceEnter(Update);

	DSys::Input::Update();

	AT_KEY_CHAR(DIK_F10) DSys::SendConsoleCommand("quit");
	
	AT_KEY_CHAR(DIK_ESCAPE)
		(void ( SceneMode == MODE_UI ? SceneMode = MODE_PLAYBACK : SceneMode = MODE_UI ) );

	if(SceneMode == MODE_PLAYBACK)
	{
		if(DSys::Input::KeyDown(DIK_LALT) || DSys::Input::KeyDown(DIK_RALT))
		{
			////////////////
			//************//
			//* ALT KEYS *//
			//************//
			////////////////
		}
		else
		{
			///////////////////
			//***************//
			//* NORMAL KEYS *//
			//***************//
			///////////////////

/*			if(!DSys::r_avi)
			{
				AT_KEY_CHAR(DIK_F12)	DSys::SendConsoleCommand("screenshot");
			}
			else
			{
				DSys::SendConsoleCommand("screenshot");
			}
*/		}
		// update the TD::gameServer
		TD::gameServer->update(msec);
	}
	else
	if( SceneMode == MODE_UI )
	{
		// user interface should think now
		UI_Update(msec);
	}

	///////////////////////
	//*******************//
	//* Non-Key Related *//
	//*******************//
	///////////////////////

	static float sceneTime;

	g_timeCounter += msec/50.0f;

	g_fadeIn.Update(msec);
	if(g_bQuitting)
		g_fadeOut.Update(msec);

	DMacro_TraceLeave();
}



void Render3D()
{
	DMacro_TraceEnter(Render3D);
	
	if(SceneMode != MODE_PLAYBACK)
	{
		glClearColor( 1,1,1,1);
		glClear( GL_COLOR_BUFFER_BIT);
		glClearColor( 0,0,0,1);
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		DGL::Draw2DRect( 320, 300, 140, 140, g_menuBack);
		DGL::Font::Print2D(10, 20, "Programming By Daher Alfawares", 1, DGL::Font::STYLE_LEFT|DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGray(),0);
		DGL::Font::Print2D(10, 10, "http://dee.digitalthinker.org/", 1, DGL::Font::STYLE_LEFT|DGL::Font::STYLE_SHADOWS,DGL::Color::ColorGray(),0);
		glPopAttrib();
		
		DMacro_TraceLeave();
		return;
	}


	// render the game
	glClear( GL_COLOR_BUFFER_BIT );
	TD::gameServer->render();
}


void Render2D(float msec)			// all 2d drawings happens here
{
	DMacro_TraceEnter(Render2D);


	glPushAttrib( GL_LIGHTING_BIT| GL_ENABLE_BIT);
	glDisable( GL_LIGHTING);
	glDisable( GL_CULL_FACE);

//==============================================================

		if(r_drawFPS.intval())
		{
			g_font.SetStyle(g_font.STYLE_SHADOWS|g_font.STYLE_RIGHT);
			g_font.SetColor(DGL::Color::ColorGray());
			g_font.Print(638,468,	"%ifps", DGL::GetFPS(msec));
		}

		static float totaltime;
		static char	 *timer;
		totaltime+= msec;
		if(r_drawTimer.intval())
		{
			g_font.SetStyle(g_font.STYLE_SHADOWS|g_font.STYLE_RIGHT);
			g_font.SetColor(DGL::Color::ColorGray());
			g_font.Print(635, 458, "%s", D_Msec2String(totaltime));
		}

		g_font.SetColor( DGL::Color::ColorGray() );
		g_font.SetStyle( g_font.STYLE_SHADOWS | g_font.STYLE_LEFT );
//		g_font.Print( 20, 460, "Hello abu hani", DSys::sv_sUserName.strval());
		g_font.Print( 20, 470, "Daher Bouncing Balls");

		if( SceneMode == MODE_UI )
		{
			UI_Render();
		}
		else
		{
			g_fadeIn.Render();
			if(g_bQuitting)
				if(!g_fadeOut.Render())
					DSys::SendConsoleCommand("quit");
		}

//===========================================================\\

	glPopAttrib();

	DMacro_TraceLeave();
}


void Shutdown()
{
	TD::gameServer->destroy();
	DUI::Destroy();
	g_bakmusic.Destroy();
	g_menuBack.Delete();
}














//////////////////////////
// User Interface


/* Add UI Button ID Here */
enum UI_Button_e {
	UIB_CONTINUE,		// continues the current game if any
	UIB_NEWGAME,		// starts a new game
	UIB_TOGGLE_MUSIC,	// starts music
	UIB_CONSOLE,		// show console

	UIB_EXIT,			// exits scene
	UIB_NUMBER
};


static DUI_Button	ui_buttons[ UIB_NUMBER];

static const MenuY				= 300;
static const ButtonYSpace		= 0;
static const ButtonWidth		= 128;
static const ButtonHeight		= 32;

#define BUTTON_X				100 //320 - (ButtonWidth/2)
#define BUTTON_Y(id)			MenuY - (id) * (ButtonYSpace + ButtonHeight)

void UI_Init(){
	DUI::Init();

	DUI::CreateObject( &ui_buttons[UIB_CONTINUE], "Continue", BUTTON_X, BUTTON_Y(UIB_CONTINUE), ButtonWidth, ButtonHeight, "Continue");
	DUI::CreateObject( &ui_buttons[UIB_NEWGAME], "NewGame", BUTTON_X, BUTTON_Y(UIB_NEWGAME), ButtonWidth, ButtonHeight, "Starts A New Game");
	DUI::CreateObject( &ui_buttons[UIB_TOGGLE_MUSIC], "Music", BUTTON_X, BUTTON_Y(UIB_TOGGLE_MUSIC), ButtonWidth, ButtonHeight, "Toggles Music (Check: /Sounds/Music/Music.txt)");
	DUI::CreateObject( &ui_buttons[UIB_CONSOLE], "Console", BUTTON_X, BUTTON_Y(UIB_CONSOLE), ButtonWidth, ButtonHeight, "Drops Console");
	DUI::CreateObject( &ui_buttons[UIB_EXIT], "Quit", BUTTON_X, BUTTON_Y(UIB_EXIT), ButtonWidth, ButtonHeight, "Exits To System");
}

void UI_OnLeftClick(int button){
	switch (button)
	{
		case UIB_CONTINUE:
			SceneMode = MODE_PLAYBACK;
			break;
		case UIB_NEWGAME:
			SceneMode = MODE_PLAYBACK;
			TD::gameServer->start_new_game();
			break;
		case UIB_TOGGLE_MUSIC:
			if(g_bakmusic.IsPlaying())
				g_bakmusic.Stop();
			else
				g_bakmusic.Play();
			break;
		case UIB_CONSOLE:
			DSys::SendConsoleCommand("start");
			break;
		case UIB_EXIT:
			{
#ifdef _DEBUG
				DSys::SendConsoleCommand("quit");
#endif
				g_bQuitting = true;
				SceneMode = MODE_PLAYBACK;
			}
			break;
	}
}

void UI_OnRightClick(int button)
{
}

void UI_Update(float msec) { static int i;
	static bool MouseLeftClicked;
	static bool MouseRightClicked;

	MouseLeftClicked = DSys::Input::MouseChar(0);
	MouseRightClicked= DSys::Input::MouseChar(1);

	DUI::MouseMoved(DSys::Input::MouseXDelta(), DSys::Input::MouseYDelta());
	for(i=0; i< UIB_NUMBER; i++)
	{
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
	for(i=0; i< UIB_NUMBER; i++)
	{
		DUI::SelectObject(&ui_buttons[i]);
		DUI::OnKey(key);
	}
}

void UI_Render(){	static int i;
	for(i=0; i< UIB_NUMBER; i++)
	{
		DUI::SelectObject(&ui_buttons[i]);
		DUI::RenderObject();
		DUI::RenderObjectToolTip();
	}

	DUI::MouseRender();
}


#endif // DE_TD