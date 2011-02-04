#include "stdafx.h"
/*
Здесь начисление/получение фрагов и т.п.
*/
int (__cdecl *netReportScore)(void *Player);
void (__cdecl *playerAddFragDeathmatch)(void *Victim,void *Attacker,void *A,void *B);

int (__cdecl *noxTeamCanPlayGame)(); // есть ли достаточно игроков в команде и т.п.

void *(__cdecl *netCommonByCode)(int NetCode);

void *(__cdecl *noxGetTeamFirst)();
void *(__cdecl *noxGetTeamNext)(void *Prev);
void *(__cdecl *noxGetTeamByN)(int N);
void (__cdecl *netSendTeamFrags)(void *Team,int Frags);

void *(__cdecl *noxTeamCreate)(int N);
void (__cdecl *noxTeamDelete)(void *TeamPtr,int SendClient);
void (__cdecl *teamSendTeam)(void *TeamPtr);
void (__cdecl *noxTeamUpdate)(void *newName, void *TeamPtr);
void (__cdecl *serverFlagsSet)(DWORD V);
void (__cdecl *serverFlagsClear)(DWORD V);

namespace
{
	struct pTeam
	{
	  char name[20];
	  char gap_14[20];
	  __int16 nameLastChar;
	  char gap_2a[2];
	  int firstAllyTeam;
	  int membersCount;
	  int teamFrags;
	  char teamColorNMB;
	  char teamId;
	  char teamId2;
	  char gap_3b[1];
	  void* somePtr;
	  int field_40;
	  int haveNameMB;
	  int flagPtr;
	  int field_4C;
	};

	int teamCanPlayGameImpl() // чтобы можно было 
	{
		int Top=lua_gettop(L);
		getServerVar("teamCanStart");
		int Ret=0;
		if (lua_type(L,-1)!=LUA_TNIL)
			Ret=(lua_toboolean(L,-1))?2:1;
		lua_settop(L,Top);
		return Ret;
	}
	
	int __declspec(naked) teamCanPlayGame()
	{
		__asm 
		{
			call teamCanPlayGameImpl
			test eax,eax
			jz l1
				dec eax
				ret
l1:
			push ebx
			push edi
			push 4
			xor ebx,ebx
			push 0x0040A8A6
			ret
		};
	}
	int teamGet(lua_State *L)
	{
		lua_settop(L,2);
		if ((lua_type(L,1)==LUA_TNUMBER))
		{
			pTeam *Team=(pTeam *)noxGetTeamByN(lua_tointeger(L,1));
			if (!Team)
			{
				lua_pushnil(L);
				return 1;
			}
			lua_newtable(L);
			lua_pushinteger(L,Team->teamId);
			lua_setfield(L,-2,"id");
			char TeamName[0x20]={0};
			wcstombs(TeamName,(wchar_t*)Team,0x14);
			lua_pushstring(L,TeamName);
			lua_setfield(L,-2,"name");
			lua_pushinteger(L,Team->teamColorNMB);
			lua_setfield(L,-2,"color");
			lua_pushinteger(L,Team->membersCount);
			lua_setfield(L,-2,"membersCount");
			lua_pushinteger(L,Team->teamFrags);
			lua_setfield(L,-2,"score");
			if ( lua_toboolean(L,2)==1) /// если хотят юзердат
			{
				lua_pushlightuserdata(L,Team);
				lua_setfield(L,-2,"teamPtr");
				if (Team->somePtr)
					lua_pushlightuserdata(L,Team->somePtr);
				else
					lua_pushnil(L);
				lua_setfield(L,-2,"flagPtr");
			}

			return 1;
		}
		lua_newtable(L);
		pTeam *Test=(pTeam *)noxGetTeamFirst();
		for (int i=1;Test!=NULL;Test=(pTeam *)noxGetTeamNext(Test),i++)
		{
			//lua_pushinteger(L,Test->teamId);
			//lua_pushvalue(L,2);
			pTeam *Team=Test;
			if (!Team)
			{
				lua_pushnil(L);
				return 1;
			}
			lua_newtable(L);
			lua_pushinteger(L,Team->teamId);
			lua_setfield(L,-2,"id");
			char TeamName[0x20]={0};
			wcstombs(TeamName,(wchar_t*)Team,0x14);
			lua_pushstring(L,TeamName);
			lua_setfield(L,-2,"name");
			lua_pushinteger(L,Team->teamColorNMB);
			lua_setfield(L,-2,"color");
			lua_pushinteger(L,Team->membersCount);
			lua_setfield(L,-2,"membersCount");
			lua_pushinteger(L,Team->teamFrags);
			lua_setfield(L,-2,"score");
			if ( lua_toboolean(L,2)==1) /// если хотят юзердат
			{
				lua_pushlightuserdata(L,Team);
				lua_setfield(L,-2,"teamPtr");
				if (Team->somePtr)
					lua_pushlightuserdata(L,Team->somePtr);
				else
					lua_pushnil(L);
				lua_setfield(L,-2,"flagPtr");
			}
			lua_pushinteger(L,i);
			lua_pushvalue(L,-2);
			lua_settable(L,3);/// это таблица
			lua_settop(L,3);//2 + 1 
		}
		return 1;
	}
	int playerInfo(lua_State *L)
	{
		lua_settop(L,1);
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		DWORD *DW=(DWORD*)lua_touserdata(L,1);
		if (0==(DW[2] & 0x4))
		{
			lua_pushstring(L,"wrong args: unit is not a player!");
			lua_error_(L);
		}
		void **PP=(void **)(((char*)lua_touserdata(L,1))+0x2EC);
		PP=(void**)(((char*)*PP)+0x114);
		byte *P=(byte*)(*PP);
		lua_newtable(L);
		lua_pushinteger(L,*((short*)(P+0x864)));
		lua_setfield(L,-2,"ping");
		lua_pushinteger(L,*((int*)(P+0x85C)));
		lua_setfield(L,-2,"score");
		char Name[0x50]={0};
		wcstombs(Name,((wchar_t*)(P+0x1260)),0x50);
		lua_pushstring(L,Name);
		lua_setfield(L,-2,"name");
		char WOL[0x20]={0};
		strncpy(WOL,((char*)(P+0x830)),0x20);
		lua_pushstring(L,WOL);
		lua_setfield(L,-2,"wol");
		int Class=*((byte*)(P+0x8CB));
		lua_pushinteger(L,Class);
		lua_setfield(L,-2,"class");
		if (Class==0)
			lua_pushstring(L,"WAR");
		else if (Class==1)
			lua_pushstring(L,"WIZ");
		else if (Class==2)
			lua_pushstring(L,"CON");
		else 
			lua_pushstring(L,"UNK");

		lua_setfield(L,-2,"className");

		int NetCode=*((short*)(P+0x80C));
		byte *Common=(byte *)netCommonByCode(NetCode);
		lua_pushinteger(L,NetCode);
		lua_setfield(L,-2,"netcode");
		int TeamId=*(Common+4);
		byte *Team=NULL;
		if (TeamId>0)
		{
			Team=(byte *)noxGetTeamByN(lua_tointeger(L,1));
		}
		if (Team!=NULL)
		{
			lua_pushinteger(L,TeamId);
			lua_setfield(L,-2,"teamId");
			wcstombs(Name,(wchar_t*)Team,0x14);
			lua_pushstring(L,Name);
			lua_setfield(L,-2,"teamName");
			lua_pushinteger(L,*((int *)(Team+0x34)));
			lua_setfield(L,-2,"teamScore");

		}

		return 1;
	}
	void *teamGetByLua(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA) &&
			(lua_type(L,1)!=LUA_TNUMBER)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *Team=0;
		if (lua_type(L,1)==LUA_TLIGHTUSERDATA)
		{
			Team=lua_touserdata(L,1);
			void *Test=noxGetTeamFirst();
			for (;Test!=NULL;Test=noxGetTeamNext(Test))
			{
				if (Test==Team)
					break;
			}
			if (Test==NULL)
			{
				lua_pushstring(L,"wrong args - not a team!");
				lua_error_(L);
			}
		}
		else
		{
			Team=noxGetTeamByN(lua_tointeger(L,1));
			if (Team==NULL)
			{
				lua_pushstring(L,"wrong args: wrong team N!");
				lua_error_(L);
			}
		}
		return Team;
	}
	int teamDelete(lua_State *L)
	{
		noxTeamDelete(teamGetByLua(L),1);
		void *Test=noxGetTeamFirst();
		if (Test==NULL)
			serverFlagsClear(4);/// выключаем тимы

		return 0;
	}
	int teamCreate(lua_State *L)
	{
		void *R=NULL;
		lua_settop(L,2);
		int id=0;
		if (lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"id");
			id=lua_tointeger(L,-1);
		}
		if (id>0)
		{
			R=noxGetTeamByN(id);
		}
		else
			R=noxTeamCreate(id);
		if (R==NULL)
		{
			lua_pushnil(L);
			return 1;
		}
		serverFlagsSet(4);/// включаем тимы

		lua_pushlightuserdata(L,R);
		wchar_t* RR;
		if (lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"name");
			const char *S=lua_tostring(L,-1);
			if (S)
			{
				mbstowcs((wchar_t*)R,S,0x14);
				RR = (wchar_t*)R;
				((byte*)R)[0x44]=1;
			}
			else
			{
				mbstowcs((wchar_t*)R,"null",0x14);
				RR = (wchar_t*)R;
				((byte*)R)[0x44]=1;
			}
			lua_getfield(L,1,"color");
			if (!lua_isnil(L,-1))
			{
				((byte*)R)[0x38]=lua_tointeger(L,-1);
			}
		}
		else
		{
			mbstowcs((wchar_t*)R,"null",0x14);
			RR = (wchar_t*)R;
			((byte*)R)[0x44]=1;
		}
		teamSendTeam(R);
		noxTeamUpdate(RR, R);
		return 1;
	}
	
	int teamScore(lua_State *L)
	{
		void *Team=teamGetByLua(L);

		int *Ret=(int *)(((char*)Team)+0x34);
		lua_settop(L,2);
		lua_pushinteger(L,*Ret);
		if (lua_type(L,2)!=LUA_TNIL)
		{
			*Ret=lua_tointeger(L,2);
			netSendTeamFrags(Team,*Ret);
		}
		return 1;
	}
	int teamName(lua_State *L)
	{
		void *Team=teamGetByLua(L);
		
		return 1;
	}
	int playerScore(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		DWORD *DW=(DWORD*)lua_touserdata(L,1);
		if (0==(DW[2] & 0x4))
		{
			lua_pushstring(L,"wrong args: unit is not a player!");
			lua_error_(L);
		}
		void **P=(void **)(((char*)lua_touserdata(L,1))+0x2EC);
		P=(void**)(((char*)*P)+0x114);
		int *Ret=(int *)(((char*)*P)+0x858);
		lua_settop(L,2);
		lua_pushinteger(L,*Ret);
		if (lua_type(L,2)!=LUA_TNIL)
		{
			*Ret=lua_tointeger(L,2);
			netReportScore(lua_touserdata(L,1));
		}
		return 1;
	}
	void __cdecl onDeathmatchFrag(void *Victim,void *Attacker,void *A,void *B)
	{
		int Top=lua_gettop(L);
		do
		{
			getServerVar("onDeathmatchFrag");
			if (!lua_isfunction(L,-1))
				break;
			if (Victim==NULL)
				lua_pushnil(L);
			else
				lua_pushlightuserdata(L,Victim);
			
			if (Attacker==NULL)
				lua_pushnil(L);
			else
				lua_pushlightuserdata(L,Attacker);

			if (A==NULL)
				lua_pushnil(L);
			else
				lua_pushlightuserdata(L,A);

			if (B==NULL)
				lua_pushnil(L);
			else
				lua_pushlightuserdata(L,B);
			lua_pcall(L,4,1,0);
			if (lua_type(L,-1)==LUA_TNIL ||
					(lua_type(L,-1)==LUA_TBOOLEAN && (0==lua_toboolean(L,-1)))
					)
				break;
			lua_settop(L,Top);
			return;
		}
		while(0);
		lua_settop(L,Top);		
		playerAddFragDeathmatch(Victim,Attacker,A,B);
	}
}
extern int httpGet(lua_State *L);

extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);

extern "C" void scoreInit(lua_State *L);

void scoreInit(lua_State *L)
{
	ASSIGN(serverFlagsClear,0x00417D70);
	ASSIGN(serverFlagsSet,0x00417D50);

	ASSIGN(netReportScore,0x004D8EF0);
	ASSIGN(noxTeamCanPlayGame,0x0040A8A0);
	ASSIGN(playerAddFragDeathmatch,0x0054D980);
	ASSIGN(noxGetTeamFirst,0x00418B10);
	ASSIGN(noxGetTeamNext,0x00418B60);
	ASSIGN(noxGetTeamByN,0x00418AB0);
	ASSIGN(netSendTeamFrags,0x00419090);
	ASSIGN(noxTeamCreate,0x004186D0);
	ASSIGN(noxTeamDelete,0x00418F20);
	ASSIGN(noxTeamUpdate,0x00418CD0);
	ASSIGN(teamSendTeam,0x004184D0);
	ASSIGN(netCommonByCode,0x00418C80);

	InjectJumpTo(0x0040A8A0,teamCanPlayGame);
	InjectOffs(0x0054D588+1,onDeathmatchFrag);
	registerserver("playerScore",&playerScore);
	registerserver("teamScore",&teamScore);
	registerserver("teamGet",&teamGet);

	registerserver("teamCreate",&teamCreate);
	registerserver("teamDelete",&teamDelete);
	registerclient("playerInfo",&playerInfo);
	registerclient("httpGet",&httpGet);
	
}