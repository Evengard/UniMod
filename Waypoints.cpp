#include "stdafx.h"

waypoint_s *(__cdecl *noxGetWaypointByName) (char const* Name);
waypoint_s *(__cdecl *noxGetWaypointById) (int Id);
waypoint_s *(__cdecl *noxCreateWaypoint) (float X,float Y);
waypoint_s *(__cdecl *noxWaypointNext) (void *Waypoint);
waypoint_s *(*noxGetWaypointList) ();

namespace
{
	int createWaypointL(lua_State*L)
	{
		if ((lua_type(L,1)!=LUA_TNUMBER) || (lua_type(L,2)!=LUA_TNUMBER) || (lua_type(L,3)!=LUA_TSTRING))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		float X=lua_tonumber(L,1); float Y=lua_tonumber(L,2);
		waypoint_s *P = noxCreateWaypoint(X,Y);
		P->flag=P->Id; //незнаю че это но пусть пока будет так
		strncpy(P->Name,lua_tostring(L,3),strlen(lua_tostring(L,3)));
		lua_pushlightuserdata(L,P);
		return 1;
	}
	int getWaypointL(lua_State*L)
	{
 		if (((lua_type(L,1)==LUA_TSTRING) || (lua_type(L,1)==LUA_TNUMBER)) && (lua_gettop(L)==1)) 
		{
			waypoint_s *P=NULL;
			if (lua_type(L,1)==LUA_TNUMBER)
				P = noxGetWaypointById(lua_tointeger(L,1));
			else
				P = noxGetWaypointByName(lua_tostring(L,1));
			if (P==NULL)
				lua_pushnil(L);
			else
				lua_pushlightuserdata(L,P);
			return 1;
		}
		else if ((lua_type(L,1)==LUA_TLIGHTUSERDATA) && (lua_toboolean(L,2)))
		{
			waypoint_s *P=(waypoint_s*) lua_touserdata(L,1);
			lua_pushnumber(L,P->X);
			lua_pushnumber(L,P->Y);
			lua_pushnumber(L,P->Id);
			lua_pushnumber(L,P->flag);
			lua_pushstring(L,P->Name);
			return 5;
		}
		else
		{
		lua_pushstring(L,"wrong args!");
		lua_error_(L);
		}
		return 0;
	}

	int moveWaypointPosL(lua_State*L)
	{
		if ((lua_type(L,1)!=LUA_TLIGHTUSERDATA) || (lua_type(L,2)!=LUA_TNUMBER) || (lua_type(L,3)!=LUA_TNUMBER))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		waypoint_s *P = (waypoint_s *)lua_touserdata(L,1);
		P->X=lua_tonumber(L,2);
		P->Y=lua_tonumber(L,3);
		return 1;
	}


	int setWaypointNameL(lua_State*L)
	{
		if ((lua_type(L,1)!=LUA_TLIGHTUSERDATA) || (lua_type(L,2)!=LUA_TSTRING))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		waypoint_s *P = (waypoint_s *) lua_touserdata(L,1);	
		strncpy(P->Name,lua_tostring(L,2),strlen(lua_tostring(L,2)));
		return 1;
	}
/*
	int setWaypointFlagL(lua_State*L)
	{
		if ((lua_type(L,1)!=LUA_TLIGHTUSERDATA) || (lua_type(L,2)!=LUA_TNUMBER))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		waypoint_s *P = (waypoint_s *) lua_touserdata(L,1);
		P->flag=lua_tonumber(L,2);
		return 1;
	}
*/
	int waypointListL(lua_State*L)
	{
		void *P=noxGetWaypointList();
		if (P==NULL)
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
			P=noxWaypointNext(P);
		}
		return 1;
	}
}

void waypointsInit()
{
	ASSIGN(noxGetWaypointByName,0x00579E30);
	ASSIGN(noxGetWaypointById,0x00579C40);
	ASSIGN(noxCreateWaypoint,0x005798F0);
	ASSIGN(noxWaypointNext,0x00579870);
	ASSIGN(noxGetWaypointList,0x00579860);



	registerserver("waypointMove",&moveWaypointPosL);
	registerserver("waypointGet",&getWaypointL);
	registerserver("waypointSetName",&setWaypointNameL);
	registerserver("waypointCreate",&createWaypointL);
	//registerserver("waypointSetFlag",&setWaypointFlagL);
	registerserver("waypointList",&waypointListL);

}