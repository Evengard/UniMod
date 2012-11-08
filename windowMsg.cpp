#include "stdafx.h"
#include "windowUniMod.h"

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


namespace
{
	int editBoxGetText(lua_State *L)
	{
		wndStruct *H=wndGetHandleByLua(1);
		if(H==0 || ((H->drawData.controlType & ctEditBox)==0))
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
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
	int editBoxSetText(lua_State *L)
	{
		wndStruct *H=wndGetHandleByLua(1);
		if(H==0 || lua_type(L,2)!=LUA_TSTRING || ((H->drawData.controlType & ctEditBox)==0))
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
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
	int buttonSetText(lua_State *L)
	{
		wndStruct *H=wndGetHandleByLua(1);
		if(H==0 || lua_type(L,2)!=LUA_TSTRING || ((H->drawData.controlType & ctPushButton)==0))
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
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

	int ListBoxAddText(lua_State *L)
	{
		wndStruct *H=wndGetHandleByLua(1);
		if(H==0 || lua_type(L,2)!=LUA_TSTRING || ((H->drawData.controlType & ctListBox)==0))
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
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
	int ListBoxClear(lua_State *L)
	{
		wndStruct *H=wndGetHandleByLua(1);
		if(H==0 || ((H->drawData.controlType & ctListBox)==0))
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		noxCallWndProc(H,0x400F,0,0);///ќчистить список
		return 0;
	}
	int ListBoxSetLine(lua_State *L)
	{
		wndStruct *H=wndGetHandleByLua(1);
		if ((H==0) || (lua_type(L,2)!=LUA_TNUMBER) || ((H->drawData.controlType & ctListBox)==0))
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
		wndStruct *H=wndGetHandleByLua(1);
		if(H==0 || ((H->drawData.controlType & ctPushButton)==0))
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}	
		H->flags=(H->flags ^ (H->flags & wfEnabled));
		return 0;
	}

	int buttonSwitchOn(lua_State *L)
	{
		wndStruct *H=wndGetHandleByLua(1);
		if(H==0 || ((H->drawData.controlType & ctPushButton)==0))
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}	
		H->flags|=wfEnabled;
		return 0;
	}

}
void windowMsgInit(lua_State*L)
{
	registerclient("wndGetText",&editBoxGetText);
	registerclient("wndSetText",&editBoxSetText);/// дл€ эдитов
	registerclient("wndLbAddText",&ListBoxAddText);/// добавл€ет строчку в листбокс
	registerclient("wndLbClear",&ListBoxClear);
	registerclient("wndLbSelectLine",&ListBoxSetLine);
	registerclient("wndButtonSetText",&buttonSetText);
	registerclient("wndButtonSwitchOn",&buttonSwitchOn);
	registerclient("wndButtonSwitchOff",&buttonSwitchOff);
	
}