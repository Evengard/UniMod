#include "stdafx.h"
#include "player.h"
#include "unit.h"

bigUnitStruct* (__cdecl *playerFirstUnit)(); 
bigUnitStruct* (__cdecl *playerNextUnit)(void* Prev); 

playerInfoStruct *(__cdecl *playerGetDataFromIndex)(int index);

bigUnitStruct* getPlayerUDataFromPlayerInfo(playerInfoStruct *addr);
bigUnitStruct* getPlayerUDataFromPlayerIdx(int idx);
bigUnitStruct* getPlayerUDataFromPlayerNetCode(int netCode);

int playersListL(lua_State *L);

int playersListL(lua_State *L)
{
	void *P;
	P=playerFirstUnit();
	if(P==0)
	{
		lua_pushnil(L);		
		return 1;
	}
	lua_newtable(L);
	int i=1;
	while(P!=0)
	{
		lua_pushinteger(L,i++);
		lua_pushlightuserdata(L,P);
		lua_settable(L,-3);
		P=playerNextUnit(P);
	}
	return 1;
}

bigUnitStruct* getPlayerUDataFromPlayerInfo(playerInfoStruct *addr)
{
	bigUnitStruct *P=playerFirstUnit();
	while(P!=0)
	{
		playerInfoStruct *PI=((ucPlayer*)P->unitController)->playerInfo;
		if(addr==PI)
			return P;
		P=playerNextUnit(P);
	}
	return 0;
}


bigUnitStruct* getPlayerUDataFromPlayerIdx(int idx)
{
	bigUnitStruct *P=playerFirstUnit();
	while(P!=0)
	{
		int idxFound=(int)((ucPlayer*)P->unitController)->playerInfo->playerIdx;
		if(idx==idxFound)
			return P;
		P=playerNextUnit(P);
	}
	return 0;
}

bigUnitStruct* getPlayerUDataFromPlayerNetCode(int netCode)
{
	bigUnitStruct *P=playerFirstUnit();
	while(P!=0)
	{
		int netCodeFound=((ucPlayer*)P->unitController)->playerInfo->netCode;
		if(netCode==netCodeFound)
			return P;
		P=playerNextUnit(P);
	}
	return 0;
}


extern void playerInit();
void playerFunctionInint()
{
	ASSIGN(playerFirstUnit,0x4DA7C0);
	ASSIGN(playerNextUnit,0x4DA7F0);
	ASSIGN(playerGetDataFromIndex,0x00417090);


	registerserver("playerList",&playersListL);


	playerInit();	
}