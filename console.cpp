#include "stdafx.h"
#include "string"

struct packetChat
{
	byte pType;
	byte unk01,unk02;
	byte SomeFlag03;
	byte Some08;


};

struct packetCast
{
	byte pType;// =0x53
	//затем идет буффер спеллов
	DWORD Buf[5]; //+1
	byte X;//+15


};
extern void *(__cdecl *noxCallWndProc)(void* Window,int Msg,int A,int B);
extern int (__cdecl *noxDrawGetStringSize) (void *FontPtr, const wchar_t*String,int *Width,int *H,int);
extern int (__cdecl *noxSetRectColorMB) (int);

extern bool justDoNoxCmd;
int *dword_69FE50=(int*) 0x69FE50;

namespace
{
	void __cdecl onPrintConsole(int color,wchar_t* str)
	{
		int Top=lua_gettop(L);
		getServerVar("onConPrint");
		if (lua_isfunction(L,-1))
		{
			char conStr[0x208]={0};
			wcstombs(conStr,str,0x208);
			lua_pushstring(L,conStr);
			lua_pushinteger(L,color);
			lua_pcall(L,2,0,0);
		}
		lua_settop(L,Top);
	}



	int __declspec(naked) onPrintConsoleTrap()
	{
		__asm
		{
			mov eax,[dword_69FE50+0]
			mov eax,[eax+0]
			push eax
			mov eax,[esp+4+8]
			push eax
			mov eax,[esp+8+4]
			push eax
			call onPrintConsole
			add esp,8
			pop eax
			push 450B95h
			ret
		};
	}


	int conCur=0; // 0 - курсор в самом самом начале, 1 - перед 1 буквой. » т. д.
	int lenStrOld=0;

	int (__cdecl *consoleEditProc)(void* Window,int Msg,int A,int B);
	void (__cdecl *consoleProcFn)();
	int __cdecl consoleEditProcNew(void* Window,int Msg,int A,int B)
	{
		BYTE *P=(BYTE*)Window;
		if (Msg==0x15)
		{
			P=*((BYTE**)(P+0x20));//ѕолучаем данные со строчкой
			if (B==2) // ќтпускают
			{
				std::wstring stroka((const wchar_t*) P);
				int Pos=stroka.size();
				if (A==0x4B+0x80) // 0x4B - код кнопки влево
				{
					if (conCur>0)
						conCur--;
					return 1;
				}else if (A==0x4D+0x80) // вправо 
				{
					if (conCur<Pos)
						conCur++;
					return 1;
				}else if (A==0xd3) // del 
				{
					if (conCur==Pos)
						return 1;
					stroka.erase(conCur,1);
					noxCallWndProc(Window,0x401E,(int)stroka.c_str(),-1); 
					lenStrOld--;
					return 1;
				}else if (A==0xCF) // End
				{
					conCur=Pos;
					return 1;		
				}else if (A==0xC7) // Home
				{
					conCur=0;
					return 1;
				}else if (A==0xE) // Ѕак спайс нажали
				{
					if ((Pos==0) || (conCur==0))
						return 1;
					stroka.erase(conCur-1,1);
					noxCallWndProc(Window,0x401E,(int)stroka.c_str(),-1); 
					conCur--;
					lenStrOld--;
					return 1;
				}else if (A==0x48+0x80) // 48 - вверх, 50- вниз
				{
					int Top=lua_gettop(L); // запоминаем что в начале
					lua_getglobal(L,"conStr");
					if (lua_type(L,-1)!=LUA_TTABLE) // если там нил например
					{
						lua_settop(L,Top);
						return 1;
					}
					lua_getfield(L,-1,"lastItem");
					int lastI=lua_tointeger(L,-1); // дл€ удобства
					lua_getfield(L,-2,"lastStr");
					int lastS=lua_tointeger(L,-1);
					lastS--;
					if (lastS<1) lastS=luaL_getn(L,-3);
					if (lastS!=lastI)
					{
						//lastS(старый) lastI [таблица]
						lua_pushinteger(L,lastS);
						lua_setfield(L,-4,"lastStr");
					}
					lua_rawgeti(L,-3,lastS);
					if (lua_type(L,-1)!=LUA_TSTRING) // если там не строка а нил например
					{
						lua_settop(L,Top);
						return 1;
					}
					const char *V=lua_tostring(L,-1);
					lua_pop(L,3);
					wchar_t *W=(wchar_t*)P;
					mbstowcs(W, V,300);
					noxCallWndProc(Window,0x401E,(int)(W),-1);
					W[strlen(V)]=0;/// на вс€кий случай припишем 0
					conCur=strlen(V); // ѕомещаем в конец курсорчик
					lenStrOld=strlen(V);
					lua_settop(L,Top);
					return 1;
				}
				else if (A==0x50+0x80) 
				{
					int Top=lua_gettop(L); // запоминаем что в начале
					lua_getglobal(L,"conStr");
					if (lua_type(L,-1)!=LUA_TTABLE) // если там нил например
					{
						lua_settop(L,Top);
						return 1;
					}
					lua_getfield(L,-1,"lastItem");
					int lastI=lua_tointeger(L,-1); // дл€ удобства
					lua_getfield(L,-2,"lastStr");
					int lastS=lua_tointeger(L,-1);
					lastS++;
					if (lastS>50 || lastS>luaL_getn(L,-3)) lastS=1;
					if (lastS!=lastI)
					{
						//lastS(старый) lastI [таблица]
						lua_pushinteger(L,lastS);
						lua_setfield(L,-4,"lastStr");
					}
					lua_rawgeti(L,-3,lastS);
					if (lua_type(L,-1)!=LUA_TSTRING) // если там не строка а нил например
					{
						lua_settop(L,Top);
						return 1;
					}
					const char *V=lua_tostring(L,-1);
					lua_pop(L,3);
					wchar_t *W=(wchar_t*)P;
					mbstowcs(W, V,300);
					noxCallWndProc(Window,0x401E,(int)(W),-1);
					W[strlen(V)]=0;// на вс€кий случай припишем 0
					conCur=strlen(V); // курсор в конец
					lenStrOld=strlen(V);
					lua_settop(L,Top);
					return 1;
				}else if (A==0x1C) // нажали энтер?
				{
					int Top=lua_gettop(L); // запоминаеем что в начале
					char string[300]="";
					wchar_t *W=(wchar_t*)P;
					int Len=wcslen(W);
					wcstombs(string,W,Len);
					lua_getglobal(L,"conStr");
					if (lua_type(L,-1)!=LUA_TTABLE)
					{
						lua_newtable(L);
						lua_pushvalue(L,-1);
						lua_setglobal(L,"conStr");
					}
					if (Len==0) 
					{
						lua_settop(L,Top);
						consoleProcFn();// ¬ызываем обработку
						return 1;
					}
					lua_getfield(L,-1,"lastItem"); // достаем последний элемент 
					int lastI=lua_tointeger(L,-1)+1;
					if (lastI>50)
						lastI=1;
					lua_pop(L,1);
					lua_pushinteger(L,lastI);
					lua_setfield(L,-2,"lastItem"); 

					lua_pushstring(L,string);
					lua_rawseti(L,-2,lastI-1);

					lua_pushinteger(L,lastI);
					lua_setfield(L,-2,"lastStr");
					lua_settop(L,Top);
					conCur=0; // курсор в начало
					lenStrOld=0;
					consoleProcFn();// ¬ызываем обработку
					return 1;
				}
				consoleEditProc(Window,Msg,A,B);
				stroka=(const wchar_t*)P;
				if (lenStrOld<stroka.size()) // код дл€ кнопачек
				{
					lenStrOld++;
					stroka.insert(conCur,1,stroka.at(lenStrOld-1));
					stroka.erase(lenStrOld);
					conCur++;
					noxCallWndProc(Window,0x401E,(int)stroka.c_str(),-1);
				}
				return 1;
			}
			
		}
		return consoleEditProc(Window,Msg,A,B);
	}

	int __cdecl conGetWidthForCur(BYTE *Window,BYTE *Wdd)
	{
		Window=*((BYTE**)(Window+0x20));
		std::wstring s((const wchar_t*)Window);
		s.erase(conCur,s.size());
		Wdd=*((BYTE**) (Wdd+0x0c8));
		int Width;
		int *PWidth=&Width;
		noxDrawGetStringSize((void*)Wdd,(wchar_t*)s.c_str(),PWidth,0,0);
		return *PWidth+5;
	}


	void __declspec(naked) consoleEditDraw()
	{
		__asm
		{	

			call    noxSetRectColorMB
			push	eax
			mov		eax,[edi+18Ch]
			test	eax,eax
			jz		l2
			push	ebx
			mov		ebx,0x69FE44
			cmp		eax,[ebx+0]
			jnz		l1
			push	esi
			push	edi
			call	conGetWidthForCur
			add		esp,8
			pop		ebx
			mov		ebx,eax	
			jmp		l2
			l1:
			pop		ebx
			l2:
			pop		eax
			push	0x4884CA  
			ret
		};
	}
	int (__cdecl *consoleParse)(wchar_t*Str,int Mode);
	int conExecL(lua_State *L)
	{
		if ((lua_type(L,1)!=LUA_TSTRING) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		wchar_t Buf[200];
		if(200==mbstowcs(Buf,lua_tostring(L,1),200))
			return 0;
		justDoNoxCmd=true;
		lua_pushinteger(L,consoleParse(Buf,1));
		justDoNoxCmd=false;
		return 1;

	}	
}
extern void InjectAddr(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);

void consoleInit()
{

	ASSIGN(consoleEditProc,0x00450F40);
	ASSIGN(consoleProcFn,0x00450FD0);
	ASSIGN(consoleParse,0x00443C80);

	InjectAddr(0x00450E4C+1,&consoleEditProcNew);
	InjectJumpTo(0x004884C5 ,&consoleEditDraw); // ћен€ем размеры
	InjectJumpTo(0x450B90 ,&onPrintConsoleTrap);

	lua_newtable(L); // делаем таблицу дл€ строк
	lua_pushinteger(L,1);
	lua_setfield(L,-2,"lastItem");
	lua_pushinteger(L,1);
	lua_setfield(L,-2,"lastStr");
	lua_setglobal(L,"conStr");

	registerclient("conExec",&conExecL);
}