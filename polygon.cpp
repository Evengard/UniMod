#include "stdafx.h"

polygon_s *(__cdecl *noxPolygonGetByIdx) (int Idx);
polygonAngle_s *(__cdecl *noxPolygonAngleGetByNum) (int Idx);

int *noxNextPolygonIdx;
int *noxNextPolygonAngleNum;

namespace
{

	int polygonGetByIdxL(lua_State *L)
	{
		if (lua_type(L,1)!=LUA_TNUMBER)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int idx=lua_tointeger(L,1);
		if (idx>*noxNextPolygonIdx-1)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushlightuserdata(L,(void*)noxPolygonGetByIdx(idx));
		return 1;
	}

	int polygonInfoL(lua_State *L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		polygon_s *P=(polygon_s*)lua_touserdata(L,1);
		lua_newtable(L);
		lua_pushinteger(L,P->polygonIdx);
		lua_setfield(L,-2,"idx");
		lua_pushinteger(L,P->posX1);
		lua_setfield(L,-2,"posX1");
		lua_pushinteger(L,P->posX2);
		lua_setfield(L,-2,"posX2");
		lua_pushinteger(L,P->posY1);
		lua_setfield(L,-2,"posY1");
		lua_pushinteger(L,P->posY2);
		lua_setfield(L,-2,"posY2");
		lua_pushstring(L,P->Name);
		lua_setfield(L,-2,"name");
		lua_pushinteger(L,P->color);
		lua_setfield(L,-2,"color");
		lua_pushinteger(L,P->kolvoUglov);
		lua_setfield(L,-2,"kolvoUglov");
		lua_pushinteger(L,P->minimapGroup);
		lua_setfield(L,-2,"minimapGroup");
		BYTE *A=(BYTE*)P->strucCoordinat;
		lua_newtable(L); 
		int i=0;
		for (;i<P->kolvoUglov;i++)
		{
			lua_newtable(L);
			polygonAngle_s *Ang=noxPolygonAngleGetByNum(*(A+i*4));
			lua_pushinteger(L,Ang->numAngle);
			lua_rawseti(L,-2,1);
			lua_pushinteger(L,Ang->posX);
			lua_rawseti(L,-2,2);
			lua_pushinteger(L,Ang->posY);
			lua_rawseti(L,-2,3);
			lua_rawseti(L,-2,(P->kolvoUglov)-i);
		}
		lua_setfield(L,-2,"tCord");			
		return 1;
	}
	
	int polygonSetAttrL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA || lua_type(L,2)!=LUA_TTABLE)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		polygon_s *P=(polygon_s *) lua_touserdata(L,1);
		lua_getfield(L,2,"name");
		if (lua_type(L,-1)==LUA_TSTRING)
			conPrintI(lua_tostring(L,-1));
		return 0;
	}





}

void polygonInit()
{

	ASSIGN(noxPolygonGetByIdx,0x004214A0); // функции
	ASSIGN(noxPolygonAngleGetByNum,0x00421030);
	
	ASSIGN(noxNextPolygonIdx,0x00595BC0); // переменные // содержит идкс следующего (еще не существующего полигона)
	ASSIGN(noxNextPolygonAngleNum,0x00595BC4); // номер следующего угла



	registerserver("polygonInfo",&polygonInfoL);
	registerserver("polygonGetByIdx",&polygonGetByIdxL);
	registerserver("polygonSetAttr",&polygonSetAttrL);

}