#include "stdafx.h"
/*
sub_450B70 - стереть все из консоли?
*/


int (__cdecl *noxWndGetPostion) (void* Window,int *xLeft,int *yTop);
void *(__cdecl *noxWndLoad)(char const *WndName,void *WndProc);
int (__cdecl *noxWndGetID)(void *Window);
void *(__cdecl *noxCallWndProc)(void* Window,int Msg,int A,int B);
void *(__cdecl *noxWndGetChildByID)(void *Window,int Id);
void (__cdecl *parseWindowStatus)(void *WddPtr,const char *Str);


void *(__cdecl *wndLoadControl)
	(const char *ControlName, void *ParentWnd, int wndFlags, 
		int Left, int Top, int Width, int Height, void *Wdd, void *DataPtr);

BYTE* (__cdecl *wndCreate2)(void *parentWnd, int wndFlags, 
							int screenLeft, int screenTop, int screenWidth, int screenHeight, 
							void *wndProc);
void (__cdecl *wndShowHide)(void *Wnd,int Hide);
void (__cdecl *wndShowModalMB)(void *Wnd);
void (__cdecl *wndSetFocusMainMB)(void *Wnd);


// какое окно схарчило мышь
void *(__cdecl *wndGetCaptureMain)(); 
int (__cdecl *wndSetCaptureMain)(void *Wnd);
void (__cdecl *wndClearCaptureMain)(void *Wnd);

int (__cdecl *noxWindowDestroy)(void *Window);

//ASSIGN(,0x);

extern int (__cdecl *noxSetRectColorMB) (int);
extern int (__cdecl *noxDrawGetStringSize) (int FontPtr, wchar_t *String,int *Width,int,int);
extern void *(__cdecl *guiFontPtrByName)(const char *FontName);
extern DWORD parseColor(const char *Color);
extern void wstringFromLua(lua_State *L,wchar_t *Dst,int MaxLen);
extern void *imageFromLua(lua_State *L);

extern int (__cdecl uniWindowDrawFn) (void *Window,void *WindowDD);
extern int (__cdecl uniListBoxDrawFn) (void *Window,void *WindowDD);


namespace 
{
	int nowCreating=0;

	bool getChildLuaByPtr(void *Ptr)
	{
		lua_getfield(L,-1,"children");
		if (!lua_istable(L,-1))
			return false;
		lua_pushlightuserdata(L,Ptr);
		lua_gettable(L,-2);
		if (!lua_istable(L,-1)) // дочернее окно
			return false;
		return true;

	}
	int __cdecl parseChildProc(byte* Window,int Msg,int A,int B)
	{
		int Top=lua_gettop(L);
		bool isChild=false;
		switch(Msg)
		{
		case 0x15: /// нажатие на кнопку A- код, B - 1-отпускание,2- нажатие
			/// похоже return 1 съедает кнопку, return 0 передает дальше
			return 0;
		case 0x11: /// mouse Enter
		case 0x12: /// mouse Leave  A-coords,
			return 1;
		case 5: // тут ловим клик (левый) что бы он дальше не ушел
			lua_getfield(L,-1,"onLMDown");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2); /// таблица окна
			lua_pushvalue(L,-5);  /// таблица - родитель 
			if (0!=lua_pcall(L,2,1,0))
			{
				const char *S=lua_tostring(L,-2);
				break;
			}
			if (lua_tonumber(L,-1)==1)
				return 1;
			return 0;
		case 6: //LMB up
		case 7:
			if (wndGetCaptureMain()==Window)
			{
				wndClearCaptureMain(Window);
				lua_getfield(L,-1,"onGrabMouse");
				if (!lua_isfunction(L,-1))
					break;
				lua_pushvalue(L,-2);
				lua_pushinteger(L,(short)(A&0xFFFF));
				lua_pushinteger(L,(short)(A>>16));
				lua_pcall(L,3,0,0);
				return 0;
			}
			else
			{
				lua_getfield(L,-1,"onClick");
				if (!lua_isfunction(L,-1))
					break;
				lua_pushvalue(L,-2);
				lua_pushinteger(L,1);
				lua_pushinteger(L,(short)(A&0xFFFF));
				lua_pushinteger(L,(short)(A>>16));
				lua_pcall(L,4,0,0);
				return 0;
			}
			break;
		case 9: // Ловим правый клик (тут он долгий)
			lua_getfield(L,-1,"onRMDown");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2); /// таблица окна
			lua_pushvalue(L,-5);  /// таблица - родитель 
			if (0!=lua_pcall(L,2,1,0))
			{
				const char *S=lua_tostring(L,-2);
				break;
			}
			if (lua_tonumber(L,-1)==1)
				return 1;
			return 0;
		case 12: // Ловим правый клик (тут долгий)
			lua_getfield(L,-1,"onRMDown");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2); /// таблица окна
			lua_pushvalue(L,-5);  /// таблица - родитель 
			if (0!=lua_pcall(L,2,1,0))
			{
				const char *S=lua_tostring(L,-2);
				break;
			}
			if (lua_tonumber(L,-1)==1)
				return 1;
			return 0;
/*
сообщение о изменении фокуса 
A=0 фокус получен
A=1 фокус потерян
B - ChildId
*/
		case 0x4003:
			{
				void *P=noxWndGetChildByID(Window,B);
				if (P==0 || !getChildLuaByPtr(P))
					return 1;
				lua_getfield(L,-1,"onFocus");
				if (!lua_isfunction(L,-1))
					return 1;
				lua_pushvalue(L,-2);
				lua_pushvalue(L,-5);
				lua_pushinteger(L,A);
				if (0!=lua_pcall(L,3,1,0))
				{
					const char *S=lua_tostring(L,-1);
					return 1;
				}
				return 1;//lua_toboolean(L,-1);
			}
		}
		lua_settop(L,Top);
/*		char Buf[40]="";
		sprintf(Buf,"%04x %p %p",Msg,A,B);
		conPrintI(Buf);*/

		if (getChildLuaByPtr((void*)A))
			switch (Msg)
		{
		case 0x4007:
			lua_getfield(L,-1,"onClick");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2); /// таблица окна
			lua_pushvalue(L,-5);  /// таблица - родитель //  control click control childrens table <top>
			if (0!=lua_pcall(L,2,0,0))
			{
				const char *S=lua_tostring(L,-1);
				break;
			}
			return 0;
		case 0x4010: /// листбокс, смена элемента
			lua_getfield(L,-1,"onSelChange");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2); /// таблица окна
			lua_pushvalue(L,-5);  /// таблица - родитель //  control click control childrens table <top>
			lua_pushinteger(L,B); /// Номер в списке
			if (0!=lua_pcall(L,3,0,0))
			{
				const char *S=lua_tostring(L,-1);
				break;
			}
			break;
		case 0x401F: // эдит текст, Enter
			lua_getfield(L,-1,"onChange");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2); /// таблица окна
			lua_pushvalue(L,-5);  /// таблица - родитель //  control click control childrens table <top>
			if (0!=lua_pcall(L,2,0,0))
			{
				const char *S=lua_tostring(L,-1);
				break;
			}
			break;
		default:
			break;
		}
		
		return 0;
	}

	int __cdecl newWindowProc(void* Window,int Msg,int A,int B)
	{
		int Top=lua_gettop(L);
		int Ret=0;
		switch(Msg)
		{
			case 0x02://destroy window
				lua_pushlightuserdata(L,&noxWndLoad);
				lua_gettable(L,LUA_REGISTRYINDEX);
				lua_pushlightuserdata(L,Window);// Удаляем таблицу из реестра
				lua_pushnil(L);
				lua_settable(L,-3);
				break;
			case 0x01:
				lua_pushlightuserdata(L,&noxWndLoad);
				lua_gettable(L,LUA_REGISTRYINDEX);
				lua_pushinteger(L,nowCreating);
				lua_gettable(L,-2);
				if(lua_type(L,-1) ==LUA_TNIL)
					break;
				lua_pushlightuserdata(L,Window);
				lua_pushvalue(L,-2);
				lua_settable(L,-4);

				lua_getfield(L,-1,"wndProc");
				if(lua_type(L,-1)==LUA_TFUNCTION)
				{
					lua_pushvalue(L,-2);///сама таблица
					lua_pushinteger(L,Msg);
					lua_pushinteger(L,A);
					lua_pushinteger(L,B);
					lua_pcall(L,4,1,0);
				}
				if (!lua_isboolean(L,-1) || !lua_toboolean(L,-1))
				{
					lua_getfield(L,-2,"oldWndProc");
					if (lua_type(L,-1)==LUA_TLIGHTUSERDATA)
					{
						int (__cdecl *oldProc)(void* Window,int Msg,int A,int B);
						*((void**)&oldProc)=lua_touserdata(L,-1);
						if (oldProc)
							Ret=oldProc(Window,Msg,A,B);
					}
				}
				break;
			case 0x17:/// уничтожено дочернее окно
/*здесь надо дочернее удалить
				lua_pushlightuserdata(L,&noxWndLoad);
				lua_gettable(L,LUA_REGISTRYINDEX);
				lua_pushlightuserdata(L,Window);// Удаляем таблицу из реестра
				lua_pushnil(L);
				lua_settable(L,-3);*/
			/// прочие случаи
			case 0x16:// создано дочернее окно + A= ID
/// надо добавить
			case 0x04:/// RB click MB
			case 0x05:/// LB Down
			case 0x06:/// LB Up - причем даже после драга
			case 0x08:/// DRAG?
			
			case 0x09:
			case 0x0A:

			case 0x4000:// lmb press (непрерывно) посылают дочернии кнопки 
			
			

			default:
				lua_pushlightuserdata(L,&noxWndLoad);
				lua_gettable(L,LUA_REGISTRYINDEX);
				lua_pushlightuserdata(L,Window);
				lua_gettable(L,-2);
				if(lua_type(L,-1) ==LUA_TNIL)
					break;
				lua_getfield(L,-1,"wndProc");
				if (lua_isnil(L,-1)) /// возможно используем реакции на дочерние функции
				{
					lua_pop(L,1);
					Ret=parseChildProc((byte*)Window,Msg,A,B);
				}
				else
				{
					if(lua_type(L,-1)==LUA_TFUNCTION)
					{
						lua_pushvalue(L,-2);///сама таблица
						lua_pushinteger(L,Msg);
						lua_pushlightuserdata(L,(void *)A);
						lua_pushinteger(L,B);
						lua_pcall(L,4,1,0);
					}
					if (!lua_isboolean(L,-1) || !lua_toboolean(L,-1))
					{
						lua_getfield(L,-2,"oldWndProc");
						if (lua_type(L,-1)==LUA_TLIGHTUSERDATA)
						{
							int (__cdecl *oldProc)(void* Window,int Msg,int A,int B);
							*((void**)&oldProc)=lua_touserdata(L,-1);
							if (oldProc)
								Ret=oldProc(Window,Msg,A,B);
						}
					}
				}
		}
		lua_settop(L,Top);
		return Ret;
	}
	int __cdecl newDrawProc(byte *Window,byte *drawData)
	{
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L,&noxWndLoad);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Window);
		lua_gettable(L,-2);
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return 0;
		}
		lua_getfield(L,-1,"drawFn");
		lua_pushlightuserdata(L,Window);
		lua_pcall(L,1,0,0);

		lua_settop(L,Top);
		return 0;
	}
	ParseAttrib attrOffsets[]=
	{
		{"textColor",0x68,3},
		{0,0,0}
	};

	int wndSetAttr(lua_State *L)
	{
		byte *H;
		lua_settop(L,3);
		if (lua_type(L,2)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
			H=(byte*)lua_touserdata(L,1);
		else if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			H=(byte*)lua_touserdata(L,-1);
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		parseAttr(L,2,3,H,attrOffsets);
		return 1;
	}
	int wndDestroyL(lua_State *L)
	{
		void *H;
		if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
			H=lua_touserdata(L,1);
		else if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			H=lua_touserdata(L,-1);
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		lua_pushinteger(L,noxWindowDestroy(H));
		return 1;
	}
	int getChildByIdL(lua_State *L)
	{
		void *R,*H=0;
		if(lua_type(L,2)!=LUA_TNUMBER)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
			H=lua_touserdata(L,1);
		else if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			H=lua_touserdata(L,-1);
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		if(H==0)
		{
			lua_pushnil(L);
			return 1;
		}
		R=noxWndGetChildByID(H,lua_tonumber(L,2));
		if(R==0)
			lua_pushnil(L);
		else
			lua_pushlightuserdata(L,R);
		return 1;
	}
	int wndShow(lua_State *L)
	{
		void *H=0;
		lua_settop(L,2);
		if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
			H=lua_touserdata(L,1);
		else if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			H=lua_touserdata(L,-1);
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		wndShowHide(H,lua_toboolean(L,2)?0:1);
		return 0;
	}	
	int wndUnGrabMouse(lua_State *L)
	{
		void *H=0;
		lua_settop(L,1);
		if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
			H=lua_touserdata(L,1);
		else if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			H=lua_touserdata(L,-1);
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		wndClearCaptureMain(H);
		return 0;
	}
	int wndGrabMouse(lua_State *L)
	{
		void *H=0;
		lua_settop(L,2);
		if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
			H=lua_touserdata(L,1);
		else if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			H=lua_touserdata(L,-1);
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		if (0==lua_toboolean(L,2) && ( 0!=wndGetCaptureMain() ))// если второй параметр true - схарчим в любом случае
			return 0;
		wndSetCaptureMain(H);
		return 0;
	}
	int wndGetIdL(lua_State *L)
	{
		void *H=0;
		if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
			H=lua_touserdata(L,1);
		else if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			H=lua_touserdata(L,-1);
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}

		if(H==0)
		{
			lua_pushinteger(L,0);
			return 1;
		}
		lua_pushinteger(L,noxWndGetID(H));
		return 1;
	}
	int callWndProcL(lua_State *L)
	{
		void *R;
		void *H=0;
		if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
			H=lua_touserdata(L,1);
		else if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			H=lua_touserdata(L,-1);
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		lua_settop(L,4);
		R=noxCallWndProc(H,lua_tointeger(L,2),
			(lua_type(L,3)==LUA_TLIGHTUSERDATA)?(int)lua_touserdata(L,3):lua_tointeger(L,3),
			(lua_type(L,4)==LUA_TLIGHTUSERDATA)?(int)lua_touserdata(L,4):lua_tointeger(L,4));
		if(R==0)
			lua_pushinteger(L,0);
		else if(R==(void *)1)
			lua_pushinteger(L,1);
		else
		{
			lua_pushlightuserdata(L,R);
		}
		return 1;
	}
	int wndSetProcL(lua_State *L) // wnd+0x178 - прок окна
	{
		BYTE * Window=0;
		bool haveTable=false;
		if(lua_type(L,1)==LUA_TLIGHTUSERDATA)
			Window=(BYTE *)(lua_touserdata(L,1));
		else if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			haveTable=true;
			Window=(BYTE *)(lua_touserdata(L,-1));
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}			
		if(lua_type(L,2)!=LUA_TFUNCTION)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		lua_pushlightuserdata(L,&noxWndLoad);
		lua_gettable(L,LUA_REGISTRYINDEX);

		if (haveTable)
			lua_pushvalue(L,1);
		{
			lua_newtable(L);
			lua_pushlightuserdata(L,Window);
			lua_setfield(L,-2,"handle");
			lua_pushvalue(L,2);
			lua_setfield(L,-2,"wndProc");
		}
		lua_pushlightuserdata(L,*((void**)(Window+0x178)));
		lua_setfield(L,-2,"oldFn");/// сохраним старую функцию
		lua_pushlightuserdata(L,Window);
		lua_pushvalue(L,-2);
		/// сейчас стэк устроен так: -1=новаятаблица, -2=юзердата, новая таблица, 
		// табл из реестра, [юзердата из первого], функция,  второй аргумент
		lua_settable(L,-4);
		/// 174 - 178 - какой из проков?
		*((void**)(Window+0x178))=&newWindowProc;

		return 1;
	}
class StaticTextData /// данные для статикстекста
{
public:
	typedef StaticTextData My_t;

	//сами данные - должны быть первыми и в верном порядке
	wchar_t*DataPtr;
	int BoolA;
	int BoolB;
	
	size_t Size;
	wchar_t Buf[]; // чисто студийное жульничество - говорим что здесь  будет строка, но не говорим размер

	static void *Create(lua_State *L,int Idx)/// получает таблицу с параметрами окна, и кладет в нее себя
	{
		lua_getfield(L,Idx,"text");
		int NeedSize=lua_objlen(L,-1)*2; // какой длинны строку нам прислали
		NeedSize=(sizeof(My_t)+NeedSize+0x3F)&(~0x3F);//вычисляем сколько нам надо памяти (с шагом в 64байта)
		
		My_t *Me=(My_t *)lua_newuserdata(L,NeedSize); // пускай луа выделит нам память - она же и будет за ней следить
		Me->BoolA=0;Me->BoolB=0;
		Me->Size=NeedSize-sizeof(My_t);
		Me->DataPtr=Me->Buf;
		mbstowcs(Me->Buf,lua_tostring(L,-2),Me->Size>>1);
		lua_remove(L,-2);
		return Me;
	}
};
class EditboxData
{
public:
	typedef EditboxData My_t;

	//сами данные - должны быть первыми и в верном порядке
	wchar_t Buf[0x100];
	wchar_t Buf2[0x100];
	int Password; // показывать *****
	int onlyNumbers; // проверять что числа
	int onlyAlphaNum;  // проверять что буквы
	int Param_6;
	short maxLen;//0x410 - MaxLenMB
	short entryWidth; // ширина бокса для ввода (общая ширина - с подписью)
	int Param_x414;
	int Param_x418;
	int Param_x41C;//Len - задается автоматически

	void *Create(lua_State *L,int Idx)/// получает таблицу с параметрами окна
	{
		int Top=lua_gettop(L);
		My_t *Me=this;
		lua_getfield(L,Idx,"password");
		Me->Password=lua_toboolean(L,-1);
		lua_getfield(L,Idx,"onlyNumbers");
		Me->onlyNumbers=lua_toboolean(L,-1);
		lua_getfield(L,Idx,"onlyAlphaNum");
		Me->onlyAlphaNum=lua_toboolean(L,-1);
		lua_getfield(L,Idx,"maxLen");
		Me->maxLen=lua_tointeger(L,-1); // возможно MaxLen
		if (Me->maxLen<1)
			Me->maxLen=255;

		Me->entryWidth=-1;// стартовая ширина MB
		Me->Param_6=0;
		lua_settop(L,Top);
		return Me;
	}
};
class ListboxData
{
public:
	typedef ListboxData My_t;

	//сами данные - должны быть первыми и в верном порядке
	short maxLines; /// похоже количество строчек хотя хз
	short LineHeight; //высота одной строчки
	int Param_3;
	int Bool_4;/// если не 0 - то похоже может увеличиватся в размерах вызывая 0x401B родителя (или себя)
	int Param_5;// bool - есть скроллбар
	int Param_6;//0x10
	int Param_7;
	/// это уже после создания ( в структуре не нужно, для справки)
	void *SomeDataPtr; // +18 - указатель на буфер с данными
	void *buttonUp; // если есть скроллбар то кнопка вверх
	void *buttonDown; // если есть скроллбар то кнопка вниз
	void *slider; // указатель на сам скроллбар
	int unk28;
	short freeLinesCount2C; //+2C
	short var2E;// может первый/последний свободный в списке
	int unk_30;
	short unk_34;
	short unk_36;

	void *Create(lua_State *L,int Idx)/// получает таблицу с параметрами окна
	{
		My_t *Me=this;
		memset(this,0,sizeof(My_t));
		int Top=lua_gettop(L);
		lua_getfield(L,Idx,"maxLines");
		Me->maxLines=lua_tointeger(L,-1); // возможно MaxLen
		if (Me->maxLines<1)
			Me->maxLines=100;/// не стоит их много делать, память кушает
		Me->LineHeight=18;
/*		Me->Password=lua_toboolean(L,-1);
		lua_getfield(L,Idx,"maxLen");

		lua_settop(L,Top);
		return Me;*/
		return this;
	}

	void CreateSlider(lua_State *L,void *Wnd, void *Parent)
	{
		int isSlider=false;
		wndStruct *wParent=(wndStruct*) Parent;
		while(true)
		{
			wndStruct *wParent=(wndStruct*) Parent;
			if (wParent->parentWindow==0)
				break;
			Parent=wParent->parentWindow;
		}
		lua_getfield(L,1,"slider"); // что бы это таблица лежала под индексом 2
		getClientVar("wndCreate");
		if (lua_type(L,2)==LUA_TTABLE)
		{
			for (int i=1;i<=3;i++)
			{
				lua_pushnumber(L,i);
				lua_gettable(L,2);
				if (lua_type(L,-1)!=LUA_TTABLE)
					break;
				lua_getfield(L,-1,"type");
				if (lua_type(L,-1)==LUA_TSTRING)
				{
					const char *ControlType=lua_tostring(L,-1);
					if (0==strcmpi(ControlType,"PUSHBUTTON"))
					{
						isSlider=false;
					}else if (0==strcmpi(ControlType,"VERTSLIDER"))
					{
						isSlider=true;
					}
					else
						break; // если ни то и ни другео то нафига мы тут вообще
				}
				lua_pop(L,1);
				lua_pushvalue(L,3);
				lua_pushvalue(L,-2);
				lua_pushlightuserdata(L,Wnd);
				if (0!=lua_pcall(L,2,1,0))
				{
					const char *S=lua_tostring(L,-1);
					lua_settop(L,1);	
					return;
				}
				lua_getfield(L,4,"handle");
				BYTE *P=(BYTE*) Wnd; // ставим слайдер или не слайдер
				P=*((BYTE**)(P+0x20));
				My_t *ListBoxData=(My_t*)P;
				if (isSlider)
					ListBoxData->slider=lua_touserdata(L,-1);
				else if (ListBoxData->buttonUp==0)
					ListBoxData->buttonUp=lua_touserdata(L,-1);
				else 
					ListBoxData->buttonDown=lua_touserdata(L,-1);		
				lua_settop(L,3);
				BYTE *Wndb=(BYTE*)Wnd;
				*((void**)(Wndb+0x17C))=&uniListBoxDrawFn;
			}
		}
		lua_settop(L,1);
		return;
	}


};
class VertSliderData
{
public:
	typedef VertSliderData My_t;

	int Param_1;
	int Param_2;
	int Param_3;
	int Param_4;

	void *Create(lua_State *L,int Idx)/// получает таблицу с параметрами окна
	{
		int Top=lua_gettop(L);
		VertSliderData *Me=this;
		memset(this,0,sizeof(My_t));
		return Me;
	}
};

	int wndLoad(lua_State *L)
	{
		void * Window;
		if(lua_type(L,1)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		lua_settop(L,2);
		if(lua_type(L,2)==LUA_TNIL)
			{lua_pop(L,1);lua_newtable(L);}// если ничего не дали - то создадим новую
		else if(lua_type(L,2)!=LUA_TTABLE)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		lua_pushlightuserdata(L,&noxWndLoad);
		lua_gettable(L,LUA_REGISTRYINDEX);

		lua_pushinteger(L,++nowCreating);// записываем элемент
		lua_pushvalue(L,2);
		lua_settable(L,-3);//записываем в реестр ту таблицу, что нам подсунули
		Window=noxWndLoad(lua_tostring(L,1),&newWindowProc);
		lua_pushinteger(L,nowCreating--);// удаляем номерной элемент
		lua_pushnil(L);
		lua_settable(L,-3);
		
		if(Window==0)
		{
			lua_pushnil(L);
			return 1;
		}
		lua_pushlightuserdata(L,Window);
		lua_pushvalue(L,2);
		lua_pushstring(L,"handle");
		lua_pushlightuserdata(L,Window);
		lua_settable(L,-3);// Записываем в таблицу хэндл
		lua_settable(L,-3);//записываем в реестр ту таблицу, что нам подсунули
		lua_settop(L,2);/// и ее же и вернем
		return 1;
	}
	inline int lua_getint(lua_State*L,const char *Name)
	{
		lua_getfield(L,1,Name);
		return lua_tointeger(L,-1);
	}
	ListboxData LD;
	EditboxData ED;
	VertSliderData VS;

	int wndCreate(lua_State *L)
	{
		lua_settop(L,2);
		if (lua_type(L,1)!=LUA_TTABLE)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		void *Parent=0; // родительское окно
		if(lua_type(L,2)==LUA_TTABLE)
		{
			lua_getfield(L,-1,"handle");
			lua_insert(L,2);
			lua_settop(L,2);
		}
		if (lua_type(L,2)==LUA_TLIGHTUSERDATA)
		{
			Parent=lua_touserdata(L,2);
		}
/*
примерное применение
	wndCreate(
	{	
		x=100,y=100,w=200,h=30,
		bgcolor='#FF00FF';
		{
			type="PUSHBUTTON",
			x=10,y=10,w=100,h=20,
			id=1000,
			status = "IMAGE+NOFOCUS"
			wndProc=fn | nil
			click=fn,
			highlight=fn | "none" | "default";
			font="large";
			bgcolor='#332211';
			color='#112233';
			alpha=false;
			hiliteColor='#123233';
			disabledColor='#112233';
			selectedColor='#112233';
			disabledImage="somename";
			selectedImage="somename";
			image="somename"
			hiliteImage="somename"
			text="Test1";
			tooltip="Test1 tooltip";
			drawFn=function (me) end
		}
	}
	)
*/
/*
	WINDOW
	  10112 201 272 45 15 PUSHBUTTON;
	  STATUS = ENABLED+IMAGE+NOFOCUS;
	  STYLE = MOUSETRACK;
	  SELECTEDIMAGE = UIButtonSmLit;
	  HILITEIMAGE = UIButtonSmLit;
	  BACKGROUNDIMAGE = UIButtonSm;
	  ENABLEDIMAGE = NULL;
	  DISABLEDIMAGE = UIButtonSmDis;
	  IMAGEOFFSET = 0 -5;
	  TEXT = Access.wnd:Add;
	  TOOLTIP = Access.wnd:AddTT;
	END


*/
		int x,y,w,h;
		DWORD Flags=0x408;

		x=lua_getint(L,"x");
		y=lua_getint(L,"y");
		w=lua_getint(L,"w");
		h=lua_getint(L,"h");
		if (w<1) w=10;
		if (h<1) h=10;

		wddControl Wdd={0};
		lua_getfield(L,1,"status");
		if (lua_type(L,-1)==LUA_TNUMBER)
			Wdd.status=lua_tointeger(L,-1);
		else if (lua_type(L,-1)==LUA_TSTRING)
			parseWindowStatus(&Wdd,lua_tostring(L,-1));

		lua_getfield(L,1,"bgcolor");
		if (lua_type(L,-1)==LUA_TSTRING)
			Wdd.BgColor=parseColor(lua_tostring(L,-1));
		lua_getfield(L,1,"image");
		Wdd.imageH=imageFromLua(L);
		if (Wdd.imageH)
 			Flags|=0x80;

		lua_getfield(L,1,"enabledColor");
		if (lua_type(L,-1)==LUA_TSTRING)
			Wdd.EnabledRectColor=parseColor(lua_tostring(L,-1));

		lua_getfield(L,1,"disabledColor");
		if (lua_type(L,-1)==LUA_TSTRING)
			Wdd.DisabledRectColor=parseColor(lua_tostring(L,-1));
		lua_getfield(L,1,"disabledImage");
		Wdd.imageDisabledH=imageFromLua(L);
		lua_getfield(L,1,"hiliteColor");
		if (lua_type(L,-1)==LUA_TSTRING)
			Wdd.HiliteColor=parseColor(lua_tostring(L,-1));
		lua_getfield(L,1,"hiliteImage");
		Wdd.imageHiliteH=imageFromLua(L);
		lua_getfield(L,1,"selectedColor");
		if (lua_type(L,-1)==LUA_TSTRING)
			Wdd.SelectedColor=parseColor(lua_tostring(L,-1));
		lua_getfield(L,1,"selectedImage");
		Wdd.imageSelectedH=imageFromLua(L);

		lua_settop(L,1);
		lua_getfield(L,1,"offsetX");
		Wdd.offsetX=lua_tointeger(L,-1);
		lua_getfield(L,1,"offsetY");
		Wdd.offsetY=lua_tointeger(L,-1);

		lua_getfield(L,1,"textColor");
		if (lua_type(L,-1)==LUA_TSTRING)
			Wdd.TextColor=parseColor(lua_tostring(L,-1));
		lua_getfield(L,1,"text");
		if (lua_type(L,-1)==LUA_TSTRING)
			wstringFromLua(L,Wdd.String,60);// возможно 64
		lua_getfield(L,1,"tooltip");
		if (lua_type(L,-1)==LUA_TSTRING)
		{
			wstringFromLua(L,Wdd.tooltipStr,62);// возможно 64
		}
		lua_getfield(L,1,"font");
		if (lua_type(L,-1)==LUA_TSTRING)
		{
			Wdd.FontPtr=guiFontPtrByName(lua_tostring(L,-1));// возможно 64
		}
		lua_settop(L,1);

		static int nextChildId=0x100;
		lua_getfield(L,1,"type");
		if (lua_type(L,-1)==LUA_TSTRING)
		{
			const char *ControlType=lua_tostring(L,-1);
			if (0==strcmpi(ControlType,"listbox"))
				ControlType="SCROLLLISTBOX";
			void *DataPtr=NULL;
			if (0==strcmpi(ControlType,"STATICTEXT"))
			{
				lua_pushstring(L,"__bindata");
				DataPtr=StaticTextData::Create(L,1);
				lua_settable(L,1); // положим данные в таблицу окна
			}else if (0==strcmpi(ControlType,"ENTRYFIELD"))
			{
				ED.Create(L,1);
				DataPtr=&ED;
			}else if (0==strcmpi(ControlType,"SCROLLLISTBOX"))
			{
				LD.Create(L,1);
				DataPtr=&LD;
			}else if (0==strcmpi(ControlType,"VERTSLIDER"))
			{
				VS.Create(L,1);
				DataPtr=&VS;
			}
			void *Wnd=wndLoadControl(ControlType,Parent, Wdd.status,x,y,w,h, &Wdd,DataPtr);
			if (Wnd==NULL)
			{
				lua_pushstring(L,"wrong args - unable to create control");
				lua_error(L);
			}

			if ((Wdd.controlType & 0x20)!=0) // если листбокс, то проверяем на слайдер
			{
				lua_settop(L,1);
				LD.CreateSlider(L,Wnd,Parent);	
			} 

			*((int*)Wnd)=nextChildId;
			lua_pushstring(L,"childId");
			lua_pushinteger(L,nextChildId++);
			lua_settable(L,1);
			lua_pushstring(L,"handle");
			lua_pushlightuserdata(L,Wnd);
			lua_settable(L,1);
/*			lua_pushlightuserdata(L,&noxWndLoad);
			lua_pushlightuserdata(L,Wnd);
			lua_pushvalue(L,1);
			lua_settable(L,-3);*/
			lua_settop(L,1);
			return 1;
		}

		Wdd.controlType|=0x2000;// кастомный контрол


		lua_pushlightuserdata(L,&noxWndLoad);
		lua_gettable(L,LUA_REGISTRYINDEX);
			lua_pushinteger(L,++nowCreating);
			lua_pushvalue(L,1);
		lua_settable(L,-3);/// кладем таблицу для события
	
		/// c флагами еще разобратся надо
		BYTE *Wnd;
		Wnd=wndCreate2(Parent,Wdd.status,x,y,w,h,&newWindowProc);
			lua_pushinteger(L,nowCreating--); /// удаляем номерную таблицу
			lua_pushnil(L);
		lua_settable(L,-3);
		if (Wnd==0)
		{
			lua_pushstring(L,"wndCreate2 Fail!");
			lua_error(L);
		}
		*((void**)(Wnd+0x17C))=&uniWindowDrawFn; // наа рисовалка
		*((void**)(Wnd+0x174))=&newWindowProc;

		*((int*)Wnd)=nextChildId;
		lua_pushstring(L,"childId");
		lua_pushinteger(L,nextChildId++);
		lua_settable(L,1);

		lua_getfield(L,1,"drawFn");
		if (lua_type(L,-1)==LUA_TFUNCTION)
		{
			*((void**)(Wnd+0x17C))=&newDrawProc;
		}
		lua_settop(L,1);

		lua_pushlightuserdata(L,Wnd);
		lua_pushvalue(L,1);
				lua_pushstring(L,"handle");
				lua_pushlightuserdata(L,Wnd);
			lua_settable(L,-3);// Записываем в таблицу хэндл
		lua_settable(L,-3);
		memcpy(Wnd+0x24,&Wdd,sizeof(Wdd));
	

		lua_settop(L,1);
		lua_getfield(L,1,"children");
		if (!lua_istable(L,-1))
		{
			lua_settop(L,1);
			lua_newtable(L);
			lua_pushvalue(L,-1);
			lua_setfield(L,1,"children");
		}
		getClientVar("wndCreate"); //wndcreaet, children, table, <end>
		for (int i=1;;i++)/// создаем дочерние окна
		{
			lua_pushvalue(L,-1);
			lua_pushinteger(L,i);
			lua_gettable(L,1);
			if (lua_type(L,-1)!=LUA_TTABLE)
				break;
			lua_getfield(L,-1,"handle");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				//что еще за хрень? почему дочернее окно уже создано?
				lua_pop(L,3);
				continue;
			}
			lua_pop(L,1);
			lua_pushvalue(L,1);
			if (0!=lua_pcall(L,2,1,0))
			{
				const char *S=lua_tostring(L,-1);
				break;
			}
			lua_getfield(L,-1,"handle");
			lua_pushvalue(L,-2);
			lua_settable(L,2);/// children
			lua_settop(L,3);
		}
		lua_settop(L,1);
		return 1;
	}

};
extern void windowMsgInit(lua_State*L);
extern void InjectAddr(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);
void windowsInit()
{
	ASSIGN(noxWndGetID,0x46B0A0);
	ASSIGN(noxWndGetChildByID,0x46B0C0);
	ASSIGN(noxWndLoad,0x4A0AD0);
	ASSIGN(noxCallWndProc,0x46B490);
	ASSIGN(noxWindowDestroy,0x46C4E0);
	ASSIGN(noxWndGetPostion,0x46AA60);
	ASSIGN(parseWindowStatus,0x004A0A00);

	ASSIGN(wndShowHide,0x0046AC00);
	ASSIGN(wndShowModalMB,0x0046A8C0 );
	ASSIGN(wndSetFocusMainMB,0x0046ADC0);
	
	ASSIGN(wndLoadControl,0x004A1510);
	ASSIGN(wndCreate2,0x0046C3E0);

	ASSIGN(wndGetCaptureMain,0x0046AE00);
	ASSIGN(wndSetCaptureMain,0x0046ADC0);
	ASSIGN(wndClearCaptureMain,0x0046ADE0);


	lua_pushlightuserdata(L,&noxWndLoad);
	lua_newtable(L);
	lua_settable(L,LUA_REGISTRYINDEX);


	registerclient("wndSetProc",&wndSetProcL);
	registerclient("wndCall",&callWndProcL);
	registerclient("wndGrabMouse",&wndGrabMouse);
	registerclient("wndUnGrabMouse",&wndUnGrabMouse);

	registerclient("wndShow",&wndShow); // показывает/скрывает
	registerclient("wndClose",&wndDestroyL);
	registerclient("wndCreate",&wndCreate,1);
	registerclient("wndLoad",&wndLoad);

	registerclient("wndGetId",&wndGetIdL);
	registerclient("wndChildById",&getChildByIdL);
	registerclient("wndSetAttr",&wndSetAttr); // для задания разных параметров


	windowMsgInit(L);

}


