#include "stdafx.h"
#include "windowUniMod.h"

/*
====��� ����� �������
0x400F -  clear
0x400D - �������� �������
===������������
0x4016 - ������� �����-�� ���������, �� ������� (ArgA - �����)

====��� ����� ��������� -
0x4010 - ��� ��������� �������


====��� ����� ���������
0x401F - ��� Enter
*/


namespace
{
	int editBoxGetText(lua_State *L)
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
		wchar_t *R=(wchar_t*) noxCallWndProc(H,0x401D,0,0);///������� ������
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
	int editBoxSetText(lua_State *L)
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
		noxCallWndProc(H,0x401E,(int)(Buf),-1);///�������� ������
		delete Buf;
		lua_pushvalue(L,1);
		return 1;
	}
	int buttonSetText(lua_State *L)
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
		noxCallWndProc(H,0x4001,(int)(Buf),-1);///�������� ������
		delete Buf;
		lua_pushvalue(L,1);
		return 1;
	}

	int ListBoxAddText(lua_State *L)
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
		noxCallWndProc(H,0x400D,(int)(Buf),-1);///�������� ������
		delete Buf;
		lua_pushvalue(L,1);
		return 1;
	}
	int ListBoxClear(lua_State *L)
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
			noxCallWndProc(H,0x400F,0,0);///�������� ������
		return 0;
	}
	int ListBoxSetItem(lua_State *L)
	{
		wndStruct *H=wndGetHandleByLua(1);
		if (H==0 && lua_type(L,2)!=LUA_TNUMBER)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		listBoxDataStruct *listBoxData=(listBoxDataStruct*) H->someData;
		int idx=lua_tointeger(L,2);
		if (idx>=-1 && idx<=listBoxData->maxLines)
			listBoxData->lineSelectIdx=lua_tointeger(L,2);
		return 0;
	}

	int buttonSwitchOff(lua_State *L)
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
		BYTE *P=(BYTE*)H;
		P=P+4;
		if ((*P & 8)!=0) 
			*P=*P - 8;
		return 0;
	}

	int buttonSwitchOn(lua_State *L)
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
		BYTE *P=(BYTE*)H;
		P=P+4;
		if ((*P & 8)==0) 
			*P=*P + 8;
		return 0;
	}
}
void windowMsgInit(lua_State*L)
{
	registerclient("wndGetText",&editBoxGetText);
	registerclient("wndSetText",&editBoxSetText);/// ��� ������
	registerclient("wndLbAddText",&ListBoxAddText);/// ��������� ������� � ��������
	registerclient("wndLbClear",&ListBoxClear);
	registerclient("wndLbSelectItem",&ListBoxSetItem);
	registerclient("wndButtonSetText",&buttonSetText);
	registerclient("wndButtonSwitchOn",&buttonSwitchOn);
	registerclient("wndButtonSwitchOff",&buttonSwitchOff);
	
}