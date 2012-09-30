#include "stdafx.h"
#include "windowUniMod.h"

// сюда все функции которые связанны с окнами и юзаются в нескольких файлах

void *(__cdecl *noxCallWndProc)(void* Window,int Msg,int A,int B);
void *(__cdecl *noxWndLoad)(char const *WndName,void *WndProc);
int (__cdecl *noxWndGetPostion) (void* Window,int *xLeft,int *yTop);

wndStruct *wndGetHandleByLua(int idx) // достаем хендл окна по идиксу
{
	int Top=lua_gettop(L);
	wndStruct *H=0;
	if(lua_type(L,idx)==LUA_TLIGHTUSERDATA)
			H=(wndStruct*) lua_touserdata(L,idx);
		else if(lua_type(L,idx)==LUA_TTABLE)
		{
			lua_getfield(L,idx,"handle");
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong args!");
				lua_error(L);
			}
			H=(wndStruct*) lua_touserdata(L,-1);
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
	lua_settop(L,Top);
	return H;
}

extern void windowsInit();
extern void windowMsgInit(lua_State*L);

void windowsAllInit()
{
	windowsInit();
	windowMsgInit(L);

	ASSIGN(noxCallWndProc,0x46B490);
	ASSIGN(noxWndLoad,0x4A0AD0);
	ASSIGN(noxWndGetPostion,0x46AA60);
}