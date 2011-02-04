#include "stdafx.h"

int (__cdecl *audMakeSound)(int Snd,noxPoint *Pt,int A,int B);
int (__cdecl *audMakeSyll)(void *Player,int Syll);


extern "C" void initAudServer(lua_State *L);
namespace
{

	int audSound(lua_State *L)
	{
		int i=1;
		if ( 
			(lua_type(L,i++)!=LUA_TNUMBER) ||
			(lua_type(L,i++)!=LUA_TNUMBER) ||
			(lua_type(L,i++)!=LUA_TNUMBER)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		audMakeSound(lua_tointeger(L,1),&noxPoint(lua_tonumber(L,2),lua_tonumber(L,3)),0,0);
		return 0;
	}
/* не работает - либо таймауты подводят, либо нужно состояние Cast игроку */
	int audSyll(lua_State *L)
	{
		int i=1;
		if ( 
			(lua_type(L,i++)!=LUA_TLIGHTUSERDATA) ||
			(lua_type(L,i++)!=LUA_TNUMBER)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		audMakeSyll(lua_touserdata(L,1),lua_tonumber(L,2));
		return 0;
	}
}

void initAudServer(lua_State *L)
{
	ASSIGN(audMakeSyll,0x004FC960);
	ASSIGN(audMakeSound,0x00501A30);
	registerserver("soundMake",&audSound);
	registerserver("soundSyll",&audSyll);
	
}