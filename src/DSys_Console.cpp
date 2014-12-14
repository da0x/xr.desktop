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

#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>

#include "DShared.h"
#include "DSys.h"
#include "DGL_Main.h"
#include "DGL_Color.h"
#include "DGL_Vertex.h"
#include "DGL_Texture.h"
#include "DGL_Font.h"
#include "DGL_FX.h"

// Define undefined keys
#define VK_PAGEUP				33
#define VK_PAGEDOWN				34

#define CON_MAX_Y				DSys::sv_iScreenHeight
#define CON_DST_Y				(CON_MAX_Y / 2.0f)
#define CON_SPEED				6

/* Console text type */
#define CON_TEXT_NORMAL			0
#define CON_TEXT_GOOD			1
#define CON_TEXT_WARNING		2
#define CON_TEXT_ERROR			3


DSys::Console::DCommand				DSys::Console::cmdlist[CON_MAX_COMMANDS];
struct DSys::Console::ConsoleGFX
{
	DGL::Texture	back1;
	DGL::Texture	back2;
	DGL::Texture	back3;
	DGL::Texture	logo;
	DGL::Font		font;
}									DSys::Console::gfx;
DSys::Console::ConsoleBuffer		DSys::Console::buffer;
DSys::Console::ConsoleCommandLine	DSys::Console::cmdline;
DSys::Console::DCommandHistory		DSys::Console::cmdhistory;
int								DSys::Console::lines_up = 0;
float							DSys::Console::starttime = 0;
float							DSys::Console::exittime = 0;
bool							DSys::Console::starting = false;
bool							DSys::Console::exiting = false;
int								DSys::Console::y = CON_MAX_Y;
int								DSys::Console::emptycmdslot = 0;
bool							DSys::Console::active = false;




/*
==================
main console class
==================
*/
#define CON_MAX_COMMANDS 1024

/*
==============
console buffer
==============
*/

void DSys::Console::ConsoleBuffer::clear(){
					for(int i=0; i< CON_MAX_LINES; i++){
						flags[i]= CON_TEXT_NORMAL;
						for( int j=0; j< MAX_DSTRING; j++ )
							buffer[i][j]= '\0';
					}
					currentline=0;
				}
int DSys::Console::ConsoleBuffer::findchar( DSTR fmt, char c){
					int i=0;
					while( fmt [i] != '\0' ){
						if ( fmt [i] == c )
							return i;
						i++;
					}
					return 0;
				}
void DSys::Console::ConsoleBuffer::add(DCSTR fmt){
					if(! (currentline< CON_MAX_LINES)){
						clear();
					}
					DSTR	fmt1,fmt2;

					D_strcpy(fmt1, fmt);

					// first check for '\n'
					int c;
					while ( c = findchar( fmt1, '\n' ) ){
						D_strcpy( fmt2, &fmt1[c+1] );
						fmt1[c] = '\0';
						add(fmt1);
						flags[currentline] = flags[currentline-1];
						D_strcpy( fmt1 , fmt2 );
					}

					// now check the length of the line
					int		i= CON_MAX_CHARS;
					while ( D_strlen( fmt1 ) > CON_MAX_CHARS ){
						// find the sutable position to cut the string
                        while( i > 0 ){
							if ( fmt1[i] == ' ' || fmt1[i] == '\n' ){
								break;
							}
							i--;
						}
						if ( i <= 0 )
							i = CON_MAX_CHARS;
						else
							i ++;
						// save the extra string
						D_strcpy( fmt2, &fmt1[i] );
						// terminate the string
						fmt1[i] = '\0';

						// write to the buffer
						D_strcpy(buffer[currentline++], fmt1);
						// update the next flag
						flags[currentline] = flags[currentline-1];

						// return the extra string to fmt1
						D_strcpy( fmt1, fmt2 );
					}
					D_strcpy(buffer[currentline++], fmt1);
				}

void DSys::Console::ConsoleBuffer::addGood(DCSTR fmt){
					if(! (currentline< CON_MAX_LINES)){
						clear();
					}
					flags[currentline] = CON_TEXT_GOOD;
					add( fmt );
				}
void DSys::Console::ConsoleBuffer::addWarning(DCSTR fmt){
					if(! (currentline< CON_MAX_LINES)){
						clear();
					}
					flags[currentline] = CON_TEXT_WARNING;
					add( fmt );
				}
void DSys::Console::ConsoleBuffer::addError(DCSTR fmt){
					if(! (currentline< CON_MAX_LINES)){
						clear();
					}
					flags[currentline] = CON_TEXT_ERROR;
					add( fmt );
				}
void DSys::Console::ConsoleBuffer::extract(DSTR fmt, int line){
					if( line < 0 || line >= CON_MAX_LINES)
						return;
					D_strcpy(fmt, buffer[line]);
				}
void DSys::Console::ConsoleBuffer::extract(DSTR fmt, int &flag, int line){
					if( line < 0 || line >= CON_MAX_LINES)
						return;
					flag = flags[line];
					D_strcpy(fmt, buffer[line]);
				}
void DSys::Console::ConsoleBuffer::getcurrentline(int &line){
					line= currentline;
				}
void DSys::Console::ConsoleBuffer::dump(DSTR filename){
					int i;
					FILE *p;

					p= fopen ( filename , "wt" );
					if(!p){
						add("Error: Unable To Create File");
						return;
					}
					fprintf(p, "------ Console Log Startup %s %s ------\n",__TIME__,__DATE__);

					for(i= currentline ; i< CON_MAX_LINES; i++)
						if(buffer[i][0])
							fprintf(p, "%s\n", buffer[i]);
					for(i=0; i< currentline; i++)
						if(buffer[i][0])
							fprintf(p, "%s\n", buffer[i]);

					int len = D_FileLength(p);
					fclose(p);

					add( va("Console text dumped to '%s' with %d bytes.", filename, len ) );
				}

DSys::Console::ConsoleBuffer::ConsoleBuffer(){
					clear();
				}
		
/*
============
command line
============
*/

void DSys::Console::ConsoleCommandLine::clear(){
					cmdline[0]= '\0';
					currentchar= 0;
				}
void DSys::Console::ConsoleCommandLine::close(){
					cmdline[currentchar] = '\0';
				}
void DSys::Console::ConsoleCommandLine::add(char c){
					if(! (currentchar< (MAX_DSTRING-1)))
						return;
					cmdline[currentchar++]= c;
					close();
				}
void DSys::Console::ConsoleCommandLine::addstr( DCSTR fmt){
					int len = D_strlen(fmt)-1;
					for ( int i=0; i <len; i++ )
						add(fmt[i]);
				}
void DSys::Console::ConsoleCommandLine::remove(){
					if(currentchar == 0)
						return;
					currentchar--;
					close();
				}
void DSys::Console::ConsoleCommandLine::get(DSTR fmt){
					D_strcpy(fmt, cmdline);
				}
DSys::Console::ConsoleCommandLine::ConsoleCommandLine(){
					clear();
				}

/*
=======
Command
=======
*/

DSys::Console::DCommand::DCommand(): ready(false) { }
bool DSys::Console::DCommand::Ready(){ return ready;}
void DSys::Console::DCommand::Add(DSTR name, void ( *cmdptr)(char *arg)){
					D_strcpy( cmdname, name);
					cmdfunc = cmdptr;
					flags = false;
					ready = true;
				}
void DSys::Console::DCommand::Add(DSTR name, void ( *cmdptr)( void )){
					D_strcpy( cmdname, name);
					cmdfunc2 = cmdptr;
					flags = true;
					ready = true;
				}
int DSys::Console::DCommand::Compare(const char *cmd){
					if(ready)
						return D_strcmpi(cmd,cmdname);
					return false;
				}
void DSys::Console::DCommand::Exec(char *arg){
					if(ready){
						if(!flags)
							cmdfunc(arg);
						else
							cmdfunc2();
					}
				}

/*
===============
Command History
===============
*/

DSys::Console::DCommandHistory::DCommandHistory():cfree(0), scroll(0) {
					for (int i=0; i<CON_COMMAND_HISTORY_MAX; i++)
						*cmd[i] = '\0';
				}
void DSys::Console::DCommandHistory::add(const char *cmd) {
					D_strcpy(this->cmd[cfree % CON_COMMAND_HISTORY_MAX],cmd);
					*(this->cmd[++cfree% CON_COMMAND_HISTORY_MAX]) = '\0';
					scroll = cfree;
				}
void DSys::Console::DCommandHistory::get(char *cmd) {
					D_strcpy(cmd, this->cmd[scroll % CON_COMMAND_HISTORY_MAX]);
				}
void DSys::Console::DCommandHistory::scrollup() {
					scroll --;
					if(scroll<0) scroll = 0;
				}
void DSys::Console::DCommandHistory::scrolldown() {
					scroll ++;
					if(scroll>cfree) scroll = cfree;
				}


/*
=================
Console Functions
=================
*/
void DSys::Console::executefile(char *s){
			
			FILE *fp;
			fp= fopen(s, "rt");
			if(!fp)
			{
				Logger::Warning("Could not execute %s...", s);
				return;
			}
			else
			{
				Logger::Print("executing %s...", s);
			}
			char cmd[1024];
			while( D_fGetLine(fp, cmd) ){
				SendConsoleCommand( cmd );
			}
			fclose(fp);
		}
void DSys::Console::viewtextfile(char *s){
			FILE *fp;
			fp= fopen(s, "rt");
			if(!fp){
				Logger::Print("Error: 'File Not Found'");
				return;
			}
			char line[1024];
			while( D_fGetLine(fp, line) ){
				Log( line);
			}
			fclose(fp);
		}
int DSys::Console::findMatches( const char * search , int len , int match[] ){
			char cmd[1024];
			int n =0;
			if ( len < 0 )
				len= D_strlen(search)-1;
			for( int i =0; i <emptycmdslot; i++){
				D_strcpy( cmd, cmdlist[i].cmdname);
				cmd[len] = '\0';
				if ( ! D_strcmpi( cmd, search ) ){
					if ( match ) match[n] = i;
					n++;
				}
			}
			return n;
		}


		// adds commands to the console
void DSys::Console::AddCommand(char *name, void ( *cmdptr)(char *) ){
			if(emptycmdslot >= 1024){
				LogError("DSys::Console::AddCommand: Console max commands exeeded");
				return;
			}
			if(!name || !cmdptr){
				LogError("DSys::Console::AddCommand: cannot add command '-null-'");
				return;
			}
			// search if the command name already in use
			int i,c=emptycmdslot;
			for ( i=0; i< c; i++){
				if(!D_strcmpi(cmdlist[i].cmdname, name)){
					LogError(va("DSys::Console::AddCommand: name duplicated '%s'", name));
					return;
				}
			}
			cmdlist[emptycmdslot++].Add( name, cmdptr);
		}
		// for commands with no parameters
void DSys::Console::AddCommand(char *name, void ( *cmdptr)( void ) ){
			if(emptycmdslot >= 1024){
				LogError("DSys::Console::AddCommand: Console max commands exeeded");
				return;
			}
			if(!name || !cmdptr){
				LogError("DSys::Console::AddCommand: cannot add command '-null-'");
				return;
			}
			// search if the command name already in use
			int i,c=emptycmdslot;
			for ( i=0; i< c; i++){
				if(!D_strcmpi(cmdlist[i].cmdname, name)){
					LogError(va("DSys::Console::AddCommand: name duplicated '%s'", name));
					return;
				}
			}
			cmdlist[emptycmdslot++].Add( name, cmdptr);
		}
void DSys::Console::SendConsoleCommand(char *cmdl){
			if ( ! cmdl [0] ) return;

			DSTR cmd;
			D_strcpy(cmd,cmdl);

			char *param;
			strtok( cmd, " " );
			strtok( cmd, "\n");
			if(D_strlen(cmdl)> (D_strlen(cmd)+1)){
				param = cmd + D_strlen(cmd);
				strtok(param, "\n"); // <-- files have this
			} else
				param = NULL;

			// search for commands
			for( int i=0; i< CON_MAX_COMMANDS; i++){
				if ( cmdlist[i].Ready() &&
					!cmdlist[i].Compare(cmd) ){
						cmdlist[i].Exec(param);
						return;
				}
			}
			// search for cvars
			if(param == NULL){
				char value[1024];
				if(DSys::Var::ListGet(cmd, value)){
					Logger::Print("\"%s\" is \"%s\"", cmd,value);
					return;
				}
			} else {
				if(DSys::Var::ListSet(cmd, param))
					return;
			}
			Logger::Print( "'%s' Is An Invalid Command Or Cvar", cmd);
		}
void DSys::Console::OnKey(char k){
			if(k == VK_INSERT){
				active?
					SendConsoleCommand("exit"):
					SendConsoleCommand("start");
				return;
			}
			if(active){
				switch(k){
					case VK_ESCAPE:		cmdline.clear();break;
					case VK_BACK:		cmdline.remove();break;
					case VK_UP:			HistoryUp();break;
					case VK_DOWN:		HistoryDown();break;
					case VK_PAGEUP:		PageUp();break;
					case VK_PAGEDOWN:	PageDown();break;
					case VK_HOME:		Home();break;
					case VK_END:		End();break;
					case VK_TAB:		CompleteCommand();break;
				}
			}
		}
void DSys::Console::OnChar(char c){
			if( active ){
				switch(c){
					case VK_RETURN:
										char	cmdl[1024];
										cmdline.get(cmdl);
										cmdline.clear();
										buffer.add( va("]%s", cmdl) );
										cmdhistory.add(cmdl);
										SendConsoleCommand(cmdl);// execute the command line
										break;
					default:			if( c >= 0x0020 )
											cmdline.add(c);
				}
			}
		}
		

void DSys::Console::GFX_Init(){
//				gfx.back1.Build("textures/console/back1.tga");
//				gfx.back2.Build("textures/console/back2.tga");
//				gfx.back3.Build("textures/console/back3.tga");
//				gfx.logo.Build("textures/console/logo.tga");
				gfx.font.Create("Font");
				y = CON_MAX_Y;
			}
void DSys::Console::GFX_Render(float dt){
			static float time;
			time += dt;

			// check for animation/sound update
			if( active ){
				if( (starting) ){
					if (y > CON_DST_Y){
						y-= CON_SPEED*(time- starttime)/100.0;
						Clamp<int>( CON_DST_Y, CON_MAX_Y, y );
					} else {
						starting = false;
						starttime= 0;
					}
				}
			} else {
				if( (exiting) ){
					if (y < CON_MAX_Y){
						y+= CON_SPEED*(time- exittime)/100.0;
						Clamp<int>( CON_DST_Y, CON_MAX_Y, y );
					} else {
						exiting = false;
						exittime= 0;
					}
				} else {
					return;
				}
			}
			
			GLboolean lighting= glIsEnabled(GL_LIGHTING);
			if(lighting)
				glDisable(GL_LIGHTING);

#if 0
			const float TexSpeed= time/5000.0f;
			static DGL::Color conback;
			conback.Set(
				Abs<float>(sin((time+1000)/5000.0)),
				Abs<float>(sin((time+4410)/5000.0)),
				Abs<float>(sin((time+7820)/5000.0)),
				0.5f);
			DGL::Draw2DRect(0,y,DSys::sv_iScreenWidth, CON_MAX_Y-y,conback);
			DGL::Draw2DRect(0, y, DSys::sv_iScreenWidth, CON_MAX_Y-y,gfx.back1);
			DGL::Draw2DRect(0,y,DSys::sv_iScreenWidth,CON_MAX_Y-y,gfx.back2,
				0.0f+TexSpeed, 0.0f,
				4.0f+TexSpeed, 0.0f,
				4.0f+TexSpeed, 2.0f,
				0.0f+TexSpeed, 2.0f);
			DGL::Draw2DRect(0,y,DSys::sv_iScreenWidth,CON_MAX_Y-y,gfx.back3,
				0.0f+TexSpeed, 0.0f-TexSpeed/2.0f,
				2.0f+TexSpeed, 0.0f-TexSpeed/2.0f,
				2.0f+TexSpeed, 2.0f-TexSpeed/2.0f,
				0.0f+TexSpeed, 2.0f-TexSpeed/2.0f);
			// draw the rotating logo
			DGL::Draw2DRect(
				550-70*	Abs<float>(sin(time/250.0)),	y+(	(CON_MAX_Y-y)/2),
				140*	Abs<float>(sin(time/250.0)),		(CON_MAX_Y-y)/4,
				gfx.logo);
#else
			DGL::Draw2DRect(0,0,DSys::sv_iScreenWidth, CON_MAX_Y-y, DGL::Color(0,0,0,0.35f));
#endif //0
			DGL::Draw2DLine(0,y,DSys::sv_iScreenWidth,y,2.0f,DGL::Color::ColorRed());

			if(lines_up)
				DGL::Draw2DLine(0,y-27.5,DSys::sv_iScreenWidth,y-27.5,0.1f,DGL::Color::ColorBlack());

			DSTR	cmdl;
			int		n;
			char	*string= NULL;
			cmdline.get(cmdl);

			if( int(DSys::Timer::Time()/300.0)% 2 )
				string = va("]%s_", cmdl);
			else
				string = va("]%s", cmdl);
			gfx.font.SetColor(DGL::Color::ColorYellow());
			gfx.font.Print(10,y+17.5,string);

			int flag;
			for(int i=0; (15+ y+ 10*(i+2))<CON_MAX_Y; i++){
				n= buffer.currentline- i- lines_up;
				if (n<0) break; //dont write the very upper lines
				buffer.extract(string, flag, n);
				switch( flag ){
					case CON_TEXT_NORMAL:
						gfx.font.SetColor(DGL::Color::ColorWhite());
						break;
					case CON_TEXT_GOOD:
						gfx.font.SetColor(DGL::Color::ColorGreen());
						break;
					case CON_TEXT_WARNING:
						gfx.font.SetColor(DGL::Color::ColorYellow());
						break;
					case CON_TEXT_ERROR:	
						gfx.font.SetColor(DGL::Color::ColorRed());
						break;
				}
				gfx.font.Print(10, 15+ y+ 10*(i+2),string);
			}
			gfx.font.SetStyle(DGL::Font::STYLE_SHADOWS|DGL::Font::STYLE_RIGHT);
			gfx.font.SetColor(DGL::Color::ColorGold());
			gfx.font.Print(DSys::sv_iScreenWidth-10, y+8, DSys::Version);

			// reset the style
			gfx.font.SetStyle(DGL::Font::STYLE_SHADOWS|DGL::Font::STYLE_LEFT);
			
			if(lighting)
				glEnable(GL_LIGHTING);
		}


void DSys::Console::GFX_Destroy(){
				gfx.back1.Delete();
				gfx.back2.Delete();
				gfx.back3.Delete();
				gfx.logo.Delete();
				gfx.font.Kill();
			}
/*
================
Console Controls
================
*/

void DSys::Console::PageUp(){
			lines_up+=3;
			Clamp<int>(0, buffer.currentline, lines_up);
		}
void DSys::Console::PageDown(){
			lines_up-=3;
			Clamp<int>(0, buffer.currentline, lines_up);
		}
void DSys::Console::Home(){
			lines_up = buffer.currentline;
		}
void DSys::Console::End(){
			lines_up = 0;
		}
void DSys::Console::HistoryUp(){
			cmdhistory.scrollup();
			cmdline.clear();
			DSTR cmd;
			cmdhistory.get(cmd);
			cmdline.addstr(cmd);
		}
void DSys::Console::HistoryDown(){
			cmdhistory.scrolldown();
			cmdline.clear();
			DSTR cmd;
			cmdhistory.get(cmd);
			cmdline.addstr(cmd);
		}
void DSys::Console::CompleteCommand(){
			DSTR word;
			int		n,
					i,
					len,
					match[CON_MAX_COMMANDS];
			cmdline.get(word);
			strtok( word, " " );
			len= D_strlen(word)-1;
			n = findMatches( word, len, match );
			switch ( n ){
				case 0:
					break;
				case 1:
					cmdline.clear();
					cmdline.addstr( cmdlist[ match[ 0 ] ].cmdname );
					cmdline.add(' ');
					break;
				default:
					buffer.add( va("%d commands found:", n) );
					for ( i =0; i <n; i++)
						buffer.add( cmdlist[ match[i] ].cmdname );
			}
		}



		
		
/* ------------------- */ 
/*  Built-in commands  */
/* ------------------- */


#define Macro_PrintUsage(msg) if(!param) {\
								DSys::Console::Log(msg);\
								return;}
namespace DSys {
	void start()
	{
		if ( DSys::Console::active ){
			DSys::Console::Log("Console already started.");
		} else {
			DSys::Console::starttime	= DSys::Timer::Time();
			DSys::Console::starting		= true;
			DSys::Console::active		= true;
//			PlaySound("sounds/constart.wav",NULL,SND_ASYNC|SND_NODEFAULT);
		}
	}

	void exit()
	{
		DSys::Console::cmdline.clear();
		DSys::Console::exittime	= DSys::Timer::Time();
		DSys::Console::exiting	= true;
		DSys::Console::active	= false;
//			PlaySound("sounds/conexit.wav",NULL,SND_ASYNC|SND_NODEFAULT);
	}

	void toggle()
	{
		if( DSys::Console::active )
			start();
		else
			exit();
	}

	void quit()
	{
		PostMessage(DSys::Window::GetHWND(),WM_QUIT,0,0);
	}
	void cmdlist()
	{
		DSys::Console::Log( "..." );
		int i;
		for(i=0;i< DSys::Console::emptycmdslot; i++){
			if(DSys::Console::cmdlist[i].Ready())
				DSys::Console::Log(DSys::Console::cmdlist[i].GetName());
		}
		DSys::Console::Log( va("%d command", i));
	}

	void varlist()
	{
		DSys::Var::ListEchoToConsole();
	}
	void clear()
	{
		DSys::Console::buffer.clear();
	}
	void exec(char *param)
	{
		Macro_PrintUsage("usage: exec <filename>");
		DSys::Console::executefile(param);
	}
	void echo(char *param)
	{
		Macro_PrintUsage("usage: echo <string>");
		DSys::Console::Log(param);
	}
	void dump(char *param)
	{
		Macro_PrintUsage("usage: condump <filename>");
		DSys::Console::buffer.dump(param);
	}
	void viewtext(char *param)
	{
		Macro_PrintUsage("usage: viewtext <text_filename>");
		DSys::Console::viewtextfile(param);
	}
	void set(char *param)
	{
		Macro_PrintUsage("usage: set <cvar name> <value>");

		size_t len = D_strlen(param);
		char p1[1024];
		char p2[1024];
		D_strcpy(p1, strtok(param, " \n"));
		if(len < D_strlen(p1)+1){
			DSys::Logger::Print("usage set <cvar name> <value>");
			return;
		}
		D_strcpy(p2, strtok(NULL, " \n"));
		if(!DSys::Var::ListSet(p1, p2))
			DSys::Console::SendConsoleCommand(va("echo variable not found: %s", p1));
	}

	void get(char *param)
	{
		Macro_PrintUsage("usage: get <cvar name>");

		char val[1024];
		if(DSys::Var::ListGet(param,val))
			DSys::Console::Log(va("%s is %s",param,val));
		else
			DSys::Console::Log(va("variable not found: %s", param));
	}
	void writeconfig(char *param){
		Macro_PrintUsage("usage: writeconfig <filename>");
		DSys::Var::ListDump(param);
	}

//video Options
	struct vid_Mode_t
	{
		int w,h;
		char *str;
		char *wstr;
		char *hstr;
	}	vid_ModeList[] =
		{
			{DSys::sv_iScreenWidth,DSys::sv_iScreenHeight,"DSys::sv_iScreenWidthxDSys::sv_iScreenHeight", "DSys::sv_iScreenWidth", "DSys::sv_iScreenHeight"},
			{800,600,"800x600", "800", "600"},
			{1024,768,"1024x768", "1024", "768"},
			{1152,864,"1152x864", "1152", "864"},
			{1600,1200,"1600,1200", "1600", "1200"}
		};
	const int vidNumOfModes = 5;
	
	void vid_modeList()
	{
		int i=0;
		DSys::Logger::Print("%i Modes",vidNumOfModes);
		while(i<vidNumOfModes)
		{
			DSys::Logger::Print("%i = %s", i, vid_ModeList[i++].str);
		}
	}


	void vid_setMode(char *param)
	{
		Macro_PrintUsage("usage vid_setMode <mode>");

		int mode = atoi(param);
		if(mode <0 || mode > vidNumOfModes)
		{
			DSys::Logger::Error("Video Mode Out Of Range...");
			return;
		}
		if(!DSys::sv_bFullscreen)
		{
			if(!DSys::Window::ChangeSize(vid_ModeList[mode].w,vid_ModeList[mode].h))
			{
				DSys::Logger::Error("Cannot Change Window Size To %s", vid_ModeList[mode].str);
				return;
			}
			DSys::sv_iScreenHeight.setval( vid_ModeList[mode].hstr );
			DSys::sv_iScreenWidth.setval( vid_ModeList[mode].wstr );
			DSys::sv_iScreenHeight.update();
			DSys::sv_iScreenWidth.update();
		}
		else 
		{
			if(!DSys::Window::ChangeRes(vid_ModeList[mode].w, vid_ModeList[mode].h, DSys::sv_iColorBits.intval()))
			{
				DSys::Logger::Error("Cannot Change Video Reseolution To %s", vid_ModeList[mode].str);
				// here the default values are restored automatically
				DSys::sv_iScreenHeight.setval( "DSys::sv_iScreenWidth" );
				DSys::sv_iScreenWidth.setval( "DSys::sv_iScreenHeight" );
				DSys::sv_iScreenHeight.update();
				DSys::sv_iScreenWidth.update();
				return;
			}
			DSys::sv_iScreenHeight.setval( vid_ModeList[mode].hstr );
			DSys::sv_iScreenWidth.setval( vid_ModeList[mode].wstr );
			DSys::sv_iScreenHeight.update();
			DSys::sv_iScreenWidth.update();
		}
	}

	void screenShot(char *param)
	{
		DGL::Texture::ScreenShot();
	}

}//End namespace DSys	

/* ------------------------------------ */
/*  DSys::* console related functions   */
/* ------------------------------------ */

void DSys::AddConsoleCommand(char *name, void ( *cmdptr)(char *) )
{
	DSys::Console::AddCommand(name, cmdptr);
}
void DSys::AddConsoleCommand(char *name, void ( *cmdptr)(void) )
{
	DSys::Console::AddCommand(name, cmdptr);
}
void DSys::InitConsole()
{
	// add built in commands
	DSys::Console::AddCommand( "start", &start);
	DSys::Console::AddCommand( "exit", &exit);
	DSys::Console::AddCommand( "toggle", &toggle);
	DSys::Console::AddCommand( "quit", &quit);
	DSys::Console::AddCommand( "cmdlist", &cmdlist);
	DSys::Console::AddCommand( "varlist", &varlist);
	DSys::Console::AddCommand( "clear", &clear);
	DSys::Console::AddCommand( "exec", &exec);
	DSys::Console::AddCommand( "echo", &echo);
	DSys::Console::AddCommand( "condump", &dump);
	DSys::Console::AddCommand( "set", &set);
	DSys::Console::AddCommand( "get", &get);
	DSys::Console::AddCommand( "writeconfig", &writeconfig);
	DSys::Console::AddCommand( "viewtext", &viewtext);

	// add window commands
	DSys::Console::AddCommand( "vid_modeList", &vid_modeList);
	DSys::Console::AddCommand( "vid_setMode", &vid_setMode);
	DSys::Console::AddCommand( "screenShot", &screenShot);
}

extern "C" int mkdir(const char*);
void DSys::DestroyConsole()
{
	// make sure the user folder exists
	::mkdir("user");

	// write the config list
	DSys::SendConsoleCommand("writeconfig user/user.cfg");
}

void DSys::SendConsoleCommand(char *fmt)
{
	DSys::Console::SendConsoleCommand(fmt);
}

void DSys::EchoToConsole(char *fmt)
{
	DSys::Console::Log(fmt);
}

void DSys::EchoToConsoleError(char *fmt)
{
	DSys::Console::LogError(fmt);
}

void DSys::EchoToConsoleWarning(char *fmt)
{
	DSys::Console::LogWarning(fmt);
}

void DSys::EchoToConsoleGood(char *fmt)
{
	DSys::Console::LogGood(fmt);
}