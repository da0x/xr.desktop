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


#ifndef ___DUI_H
#define ___DUI_H

/*
TODO List:
- use mouse animations...
- add avi controls (keys)
*/

class DUI_Mouse {
	friend class DUI;

	static int			mx,my;			// position
	static DGL::Texture	cursor;			// cursor texture
	static float		speed;			// mouse speed

	static bool			lclicked;		// left clicked?
	static bool			rclicked;		// right clicked?
public:
	inline static void MouseLClicked(){
		lclicked = true;
	}

	inline static void MouseRClicked(){
		rclicked = true;
	}

	inline static void MouseMoved(int dx, int dy){
		mx += dx*speed; Clamp<int>(0,DSys::sv_iScreenWidth,mx);
		my -= dy*speed; Clamp<int>(0,DSys::sv_iScreenHeight,my);
	}

	inline static void MouseGetPosition(int &x, int &y){
		y = my;
		x = mx;
	}

	inline static void MouseRender(){
		static float x,y,
			w = 20.0f,
			h = 20.0f;

		x = mx - 10;
		y = my - 10;
		
		DGL::MatrixOp::Ortho::Begin();
		glBindTexture(GL_TEXTURE_2D, cursor);
		glColor4f(1,1,1,1);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);
		glVertex2d( x, y);
		glTexCoord2f(1.0f,0.0f);
		glVertex2d( x+w, y);
		glTexCoord2f(1.0f,1.0f);
		glVertex2d( x+w, y+h);
		glTexCoord2f(0.0f,1.0f);
		glVertex2d( x, y+h);
		glEnd();

		DGL::MatrixOp::Ortho::End();
	}
};

int				DUI_Mouse::mx		= 0;
int				DUI_Mouse::my		= 0;
DGL::Texture	DUI_Mouse::cursor;
float			DUI_Mouse::speed	= 1.0f;
bool			DUI_Mouse::lclicked	= false;
bool			DUI_Mouse::rclicked	= false;

/*
---------------------
  Object Base Class
---------------------
*/

class DUI_Object {
	friend class DUI;
	enum { 
		TEX_NORMAL,
		TEX_MOUSEOVER,
		TEX_MAX
	};

	float				mouseOverTime;
protected:
	bool				active;
	char				*toolTip;
	DGL::Texture		tex[TEX_MAX];
	int					x,y,w,h;
public:

	DUI_Object(): 
			active(false),
			mouseOverTime(0),
			x(0),
			y(0),
			w(0),
			h(0),
			toolTip(NULL)
	{
		const char* tt= "No Tool Tip Is Assotiated.";
		toolTip = new char [D_strlen(tt)+1];
		D_strcpy(toolTip, tt);
	}
	virtual ~DUI_Object() {
		active = false;
	}

	virtual void Init(const char *name) {
		tex[ TEX_NORMAL].Build(va("textures/ui/%s_0.tga", name));
		tex[ TEX_MOUSEOVER].Build(va("textures/ui/%s_1.tga", name));
	}

	void SetToolTip(const char *fmt){
		if(toolTip) 
			delete toolTip;

		toolTip = new char [D_strlen(fmt)+1];
		D_strcpy(toolTip, fmt);
	}

	inline void Enable(){ active = true;}
	inline bool Disabled(){ return !active; }

	inline void SetMouseOver(float msec){
		mouseOverTime += msec;
	}

	inline void ResetMouseOver(){
		mouseOverTime = 0;
	}

	inline bool MouseOver() { return mouseOverTime ? true : false; }
	inline float MouseOverTime(){ return mouseOverTime; }

	void RenderToolTip(int mouseX, int mouseY) {
		if(!toolTip) return;
		// wait 1500 milliseconds before drawing the tool tips
		if(mouseOverTime > 1500){
			// render a small box
			const DGL::Color borderColor(0,0,0,.5f);
			const DGL::Color backColor(0,0,0,.25f);
			const float tipSize = .8f;
			const int boxX = mouseX;
			const int boxY = mouseY;
			const int boxWidth = DGL::Font::CharWidth() * tipSize * (strlen(this->toolTip) + 2);
			const int boxHeight = DGL::Font::CharHeight() * tipSize;

			DGL::Draw2DRect( boxX, boxY, boxWidth, boxHeight, backColor);
			DGL::MatrixOp::Ortho::Begin();
				glDisable(GL_TEXTURE_2D);
				borderColor.MakeCurrent();
				glLineWidth(0.3f);
				glBegin(GL_LINE_STRIP);				
					glVertex2d( boxX,					boxY);
					glVertex2d( boxX + boxWidth,		boxY);
					glVertex2d( boxX + boxWidth,		boxY + boxHeight);
					glVertex2d( boxX,					boxY + boxHeight);
					glVertex2d( boxX,					boxY);
				glEnd();
				glLineWidth(1.0f);
				glEnable(GL_TEXTURE_2D);
			DGL::MatrixOp::Ortho::End();

            const int textX = mouseX + 10 * tipSize;
			const int textY = mouseY + 6 * tipSize;
			DGL::Font::Print2D(textX, textY, this->toolTip, tipSize, DGL::Font::STYLE_LEFT|DGL::Font::STYLE_SHADOWS, DGL::Color::ColorWhite(), 0);
		}
	}

	virtual void Render(){
		GLuint texture;
		if(active && mouseOverTime)
			texture = tex[TEX_MOUSEOVER];
		else
			texture = tex[TEX_NORMAL];

		DGL::Draw2DRect(x,y,w,h,texture);
	}

	bool OnLeftClick(){
		if( !active && !MouseOver()) return false;
		return true;
	}

	bool OnRightClick(){
		if( !active && !MouseOver()) return false;
		return true;
	}

	virtual void OnKey(char key) { }

	virtual void Destroy()
	{
		tex[ TEX_NORMAL].Delete();
		tex[ TEX_MOUSEOVER].Delete();
	}
};

class DUI : public DUI_Mouse {
	static DUI_Object	*current;
	static int			font;
	static DAudio::Sound2D mouseOverSound;
	static DAudio::Sound2D mouseLeftClickSound;

public:

	static void Init(){
		cursor.Build("textures/ui/cursor.tga");
		font = DGL::Font::CreateFont("Font");
		mouseOverSound.Load("sounds/button_mo.wav");
		mouseLeftClickSound.Load("sounds/button_lc.wav");
	}

	static void CreateObject( DUI_Object *obj, const char *name, int x, int y, int w, int h, const char *tip=NULL, bool enable = true){
		obj ->x = x;
		obj ->y = y;
		obj ->w = w;
		obj ->h = h;
		if(tip)
			obj ->SetToolTip(tip);
		obj ->Init(name);
		if(enable)
			obj->Enable();
	}

	static void SelectObject( DUI_Object *obj){
		current = obj;
	}

	static void CheckMouseOver( float msec){
		if( current->Disabled()) return;
		if( mx > current->x && 
			mx < current->x+current->w &&
			my > current->y &&
			my < current->y+current->h ){

				if(!current->MouseOver())
					mouseOverSound.Play();
				current->SetMouseOver(msec);
			} else {
				current->ResetMouseOver();
			}
	}

	static inline bool OnMouseLeftClick(){
		if(current->MouseOver()){
			mouseLeftClickSound.Play();
			return true;
		}
		return false;
	}
	static inline bool OnMouseRightClick(){
		return current->OnRightClick();
	}
	static inline void OnKey(char key){
		current->OnKey(key);
	}
	static void RenderObject(){
		current->Render();
	}
	static void RenderObjectToolTip(){
		current->RenderToolTip(mx+10,my+10);
	}

	static int Font() { return DUI::font; }

	static void Destroy()
	{
		mouseLeftClickSound.Destroy();
		mouseOverSound.Destroy();
		cursor.Delete();
	}
};

DUI_Object	*DUI::current = NULL;
int			DUI::font = 0;
DAudio::Sound2D DUI::mouseOverSound;
DAudio::Sound2D DUI::mouseLeftClickSound;

/*
================================

  CREATE YOUR NEW OBJECTS HERE

================================
*/

class DUI_Button : public DUI_Object {
public:
	DUI_Button(): DUI_Object() {  }

};

class DUI_MenuButton : public DUI_Object {
public:
	DUI_MenuButton() : 
					DUI_Object()
	{
	}
	static float size;
	int cx;
	

	void Init(const char *name) {
		// calculate width and height
		w = DGL::Font::CharWidth() * (strlen(toolTip)) * size;
		h = DGL::Font::CharHeight() * size;

		cx = x;
		x-= w/2 - DGL::Font::CharWidth() * size;
	}

	void Render(){
		DGL::Color color;
		float fsize;
		const float fadeDelay = 100.0f;

		if(active) {
			if(this->MouseOver()){
				color = DGL::Color::ColorCyan();
				fsize = 0.3f;
				if(MouseOverTime()<fadeDelay)
					fsize *= Abs<double>(sin(MouseOverTime()/fadeDelay));
			} else {
				color = DGL::Color::ColorBlue();
				fsize = 0.0f;
			}
		} else {// not active
			color = DGL::Color::ColorGray();
			fsize = 0.0f;
		}

		DGL::Font::Print2D(
			cx,y+h/2,
			toolTip,
			size + fsize,
			DGL::Font::STYLE_CENTER|
				DGL::Font::STYLE_SHADOWS,
			color,
			1);
	}
};
float DUI_MenuButton::size = 1.5f;




class DTextBuffer {
		protected:
		char			texBuf[1024];
		int				currentChar;
		
		void close(){
			texBuf[currentChar] = '\0';
		}
	public:
		void Clear(){
			texBuf[0]= '\0';
			currentChar= 0;
		}
		void Add(char c){
			if(! (currentChar< (1024-1)))
				return;
			texBuf[currentChar++]= c;
			close();
		}
		void AddStr( const char *fmt){
			int len = D_strlen(fmt)-1;
			for ( int i=0; i <len; i++ )
				Add(fmt[i]);
		}
		void Remove(){
			if(currentChar == 0)
				return;
			currentChar--;
			close();
		}
		void GetBuffer(char *fmt){
			D_strcpy(fmt, texBuf);
		}
		DTextBuffer(){Clear();}
};

class DUI_EditList : public DUI_Object {
protected:
	bool		active;
	DTextBuffer	buffer;
public:
	DUI_EditList(): DUI_Object(), active(false) { }

	bool OnLeftClick(){
		if(MouseOver()){
			active = true;
			buffer.Clear();
			return true;
		} else {
			active = false;
			return false;
		}
	}

	void OnKey(char key){
		if(!active) return;

		switch(key){
			case VK_ESCAPE:
				buffer.Clear();
				break;
			case VK_BACK:
				buffer.Remove();
				break;
			default:
				if( key >= 0x0020 )
					buffer.Add(key);
		}
	}

	void Init(const char *name){
	}

	void Render(){
		DGL::Color color;
		if(MouseOver())
			color.Set(0,.5f,1,.4f);
		else
			color.Set(0,.2f,1,.4f);

		if(active)
			color.a = .8f;

		DGL::Draw2DRect(x,y,w,h,color);

		DGL::Draw2DLine(x,y,		x+w,y,2,DGL::Color::ColorRed());
		DGL::Draw2DLine(x+w,y,		x+w,y+h,2,DGL::Color::ColorRed());
		DGL::Draw2DLine(x+w,y+h,	x,y+h,2,DGL::Color::ColorRed());
		DGL::Draw2DLine(x,y+h,		x,y,2,DGL::Color::ColorRed());

		static char text[1024];
		buffer.GetBuffer(text);
		DGL::Font::Print2D(x+5, y+10, text, .8f,
			DGL::Font::STYLE_SHADOWS, DGL::Color::ColorGreen(), DUI::Font());
	}

	void GetBuffer(char *fmt){	buffer.GetBuffer(fmt);	}
};

class DUI_Lable : public DUI_Object {
	char textBuffer[1024];
public:
	DUI_Lable(): DUI_Object() {
		textBuffer[0] = 0;
	}

	void Init(const char *name) {
		D_strcpy(textBuffer, name);
	}

	void Render(){
		static DGL::Color color;
		if(MouseOver())
			color = DGL::Color::ColorBlue();
		else
			color = DGL::Color::ColorCyan();

		DGL::Font::Print2D(x,y, textBuffer, .8f, DGL::Font::STYLE_SHADOWS, color, DUI::Font());
	}
};


class DUI_Picture : public DUI_Object {
	DGL::Texture	texture;
public:
	void Init(const char *name) {
		texture.Build(va("textures/ui/pictures/%s.jpg", name));
	}
	void Render(){
		static DGL::Color color;
		static float alpha;
		if(MouseOver()){
			color.Set(.5F,.5F,1,1);
			alpha = .6f;
		} else {
			color.Set(1,.2F,.2F,1);
			alpha = 1;
		}

		DGL::Draw2DRect(x,y,w,h,color);
		DGL::Draw2DRect(x+1,y+1,w-2,h-2,texture,alpha);
	}
	void Destroy()
	{
		texture.Delete();
	}
};


class DUI_Movie : public DUI_Object {
	DGL::Avi	avi;
	GLuint		texture;
	bool		playing;
public:
	DUI_Movie(): DUI_Object(), avi(), texture(0), playing(false) {  }

	void Init(const char *name) {
		avi.OpenAVI(va("videos/ui/%s.avi", name));
	}

	void Update(float msec){
		texture = avi.GrabAVIFrame((DWORD)msec);
	}

	void Render(){
		static DGL::Color color;
		static float alpha;
		if(MouseOver()){
			color.Set(.5F,.5F,1,1);
			alpha = .6f;
		} else {
			color.Set(1,.2F,.2F,1);
			alpha = 1;
		}

		DGL::Draw2DRect(x,y,w,h,color);
		DGL::Draw2DRect(x+1,y+1,w-2,h-2,texture,alpha);
	}

	bool OnLeftClick(){
		if(MouseOver())
			playing = true;
		return playing;
	}
	bool OnRightClick(){
		if(MouseOver())
			playing = false;
		return !playing;
	}
	void OnKey(char key) {
	}
};


#if 0

namespace DUserInterface {
	class Control {
		public:
			enum Style {
				CENTER,
				LEFT,
				RIGHT
			};
		protected:
			int				x,
							y,
							width,
							height;
			Style			style;
			float			mouseOverTime;
		public:
			Control(): x(0), y(0), width(0), height(0), style(CENTER), mouseOverTime(0)
				{  }

			////
			virtual void Init() {  };
			virtual void Destroy() {  };
			virtual void Render() {  };
			virtual void OnMouseClick() {  };
			virtual void OnKey(unsigned char key) {  };

			////
			inline bool MouseOver(int mx, int my){
				if(my>y && my<y+height){
					switch (style) {
						case CENTER:
							if( mx>x-width/2 && mx<x+width/2 )
								return true;
						case RIGHT:
							if( mx>x && mx<x+width )
								return true;
						case LEFT:
							if( mx>x-width && mx<x )
								return true;
					}
				}
				return false;
			}
			inline void AdjustMouseOverTime(float msec){
				mouseOverTime += msec;
			}
			inline void ResetMouseOverTime(){
				mouseOverTime = 0;
			}
	};
	class Menu {
			enum { MAX_CONTROLS = 1024 };
		private:
			Control			*controls[MAX_CONTROLS];
			int				numOfControls;

			Menu			*parant;		// parant menu (null = noparant)
		public:
			Menu(): numOfControls(0) {  }
			void RegisterControl( Control *control){
				controls[numOfControls++] = control;
				if(numOfControls >= MAX_CONTROLS)
					throw Daher::Daher::Exception("Menu::MAX_CONTROLS reached.");
			}
			void Init(){
				for(int i=0; i< numOfControls; i++){
					controls[i]->Init();
				}
			}
			void Destroy(){
				for( int i= 0; i< numOfControls; i++){
					controls[i]->Destroy();
				}
			}
			void Render(){
				for( int i= 0; i< numOfControls; i++)
					controls[i]->Render();
			}
			void OnMouseClick(){
				//////////// fill here
			}
	};
	class UI {
		enum { MAX_MENUS = 1024 };
		private:
			Menu			*menus[MAX_MENUS];
			int				numOfMenus;

			int				activeMenu;

		public:
			UI() : numOfMenus(0), activeMenu(0)
				{  }

			void RegisterMenu( Menu *menu){
				menus[numOfMenus++] = menu;
				if(numOfMenus >= MAX_MENUS)
					throw Daher::Daher::Exception("UI_MAX_MENUS reached.");
			}
			
			void SetActiveMenu(Menu *menu){
				activeMenu = menu;
			}
			void Init() {
				for(int i=0; i< numOfMenus; i++)
					menus[i]->Init();
			}
			void Destroy() {
				for(int i=0; i< numOfMenus; i++)
					menus[i]->Destroy();
			}
			void Render() {
				menus[activeMenu]->Render();
			}
			void OnMouseClick() {  }
			void OnKey(unsigned char key) {  };
	};
}
#endif //0
#endif // ___DDUI_H