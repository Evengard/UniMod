#include "stdafx.h"

/*
====дл€ листа команды
0x400F -  clear
0x400D - добавить строчку
===непроверенно
0x4016 - достать какой-то указатель, мб строчку (ArgA - номер)

====дл€ листа сообщени€ -
0x4010 - при выделении строчки


====дл€ эдита сообщени€
0x401F - при Enter
*/

extern void *(__cdecl *noxCallWndProc)(void* Window,int Msg,int A,int B);
namespace
{
	int getTextL(lua_State *L)
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
			lua_pushnil(L);
			return 1;
		}
		wchar_t *R=(wchar_t*) noxCallWndProc(H,0x401D,0,0);///считать строку
		if(R==0)
		{
			lua_pushnil(L);
			return 1;
		}
		int Len=wcslen(R);
		char *Buf=new char[Len+1];
		Buf[Len]=0;
		wcstombs(Buf,R,Len);
		lua_pushstring(L,Buf);
		delete Buf;
		return 1;
	}
	int setTextL(lua_State *L)
	{
		void *H=0;
		if(lua_type(L,2)==LUA_TNIL)
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
		
		size_t Len;
		const char *R;
		R=lua_tolstring(L,2,&Len);
		if (R==NULL || Len==0)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		wchar_t *Buf=new wchar_t[Len+1];
		mbstowcs(Buf,R,Len);
		Buf[Len]=0;
		noxCallWndProc(H,0x401E,(int)(Buf),-1);///записать строку
		delete Buf;
		lua_pushvalue(L,1);
		return 1;
	}
	int setTextBut(lua_State *L)
	{
		void *H=0;
		if(lua_type(L,2)==LUA_TNIL)
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
		size_t Len;
		const char *R;
		R=lua_tolstring(L,2,&Len);
		if (R==NULL)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		wchar_t *Buf=new wchar_t[Len+1];
		mbstowcs(Buf,R,Len);
		Buf[Len]=0;
		noxCallWndProc(H,0x4001,(int)(Buf),-1);///записать строку
		delete Buf;
		lua_pushvalue(L,1);
		return 1;
	}

	int LBAddTextL(lua_State *L)
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
		if(H==0)
		{
			lua_pushnil(L);
			return 1;
		}
		size_t Len;
		const char *R;
		R=lua_tolstring(L,2,&Len);
		if (R==NULL)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		wchar_t *Buf=new wchar_t[Len+1];
		mbstowcs(Buf,R,Len);
		Buf[Len]=0;
		noxCallWndProc(H,0x400D,(int)(Buf),-1);///записать строку
		delete Buf;
		lua_pushvalue(L,1);
		return 1;
	}
	int wndLbClear(lua_State *L)
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
		if(H!=0)
			noxCallWndProc(H,0x400F,0,0);///ќчистить список
		return 0;
	}
}
void windowMsgInit(lua_State*L)
{
	registerclient("wndGetText",&getTextL);
	registerclient("wndSetText",&setTextL);/// дл€ эдитов
	registerclient("wndLbAddText",&LBAddTextL);/// добавл€ет строчку в листбокс
	registerclient("wndLbClear",&wndLbClear);
	registerclient("wndButtonSetText",&setTextBut);
	
}