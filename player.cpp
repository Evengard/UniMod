#include "stdafx.h"
#include <vector>

void (__cdecl *playerObserveCre)(void *Player,void *Creature);
void (__cdecl *playerObserveCreUndo)(void *Player);
void (__cdecl *sub_4FC2B0) (void*);
void (__cdecl *sub_4E6040) (void*);
/// не вполне понятная функция
void *(__cdecl *netUnitFromPacketMB)(int NetIdx);

extern int (__cdecl *printCentered)(wchar_t *Text);
extern bigUnitStruct *(__cdecl *unitDamageFindParent) (void *Unit);
/*
во вселенном состоянии надо фильтровать сообщение 0x79 тру спелл и делать на него
обход до 0051C16C, подменив источник
*/
// это функции, которые принимают команды игроков
void *(*playerControlBufferFirst)(int N);
void *(*playerControlBufferNext)(int N, void *Prev);

void *(__cdecl *playerAtCursor) (void *Player); 
extern DWORD *GameFlags;

DWORD *dword_834ABC=(DWORD*)0x834ABC;


extern void *(__cdecl *noxGetTeamFirst)();
extern void *(__cdecl *noxGetTeamNext)(void *Prev);
extern void (__cdecl *noxTeamDelete)(void *TeamPtr,int SendClient);

extern int (__cdecl *noxGetUnitsInRect)(FloatRect *Rect, void (__cdecl *SomeFn)(void *Unit, void *Arg), void *Arg);

extern unitBigStructPtr	(__cdecl* objectCreateByName)(char const *ObjName);
extern void 			(__cdecl* noxCreateAt)(unitBigStructPtr Obj,unitBigStructPtr ParentUnit, float X,float Y);

extern void (__cdecl *serverFlagsSet)(DWORD V);
extern wchar_t *(__cdecl *noxTeamDefaultName)(int color);
extern void *(__cdecl *noxTeamCreate)(int N);
extern void (__cdecl *teamSendTeam)(void *TeamPtr);
extern int teamAutoAssign(lua_State *L);

namespace
{
	void __cdecl onPlayerJoin(void *Player)
	{
		int Top=lua_gettop(L);
		getServerVar("playerOnJoin");
		if (lua_isfunction(L,-1))
		{
			lua_pushlightuserdata(L,Player);
			if (0!=lua_pcall(L,1,0,0))
				conPrintI(lua_tostring(L,-1));
		}
		lua_settop(L,Top);
	}

	int __declspec(naked) onPlayerJoinTrap(DWORD TestVal)
	{
		__asm
		{
			push ebp
			call onPlayerJoin
			add esp,4
			mov ecx,GameFlags
			mov eax,[ecx]
			and eax,[esp+4]
			ret
		};//просто это была функция CheckGameFlags :)
	}

	void __cdecl onPlayerLeaveObs(void *Player)
	{
		int Top=lua_gettop(L);
		getServerVar("playerOnLeaveObs");
		if (lua_isfunction(L,-1))
		{
			lua_pushlightuserdata(L,Player);
			if (0!=lua_pcall(L,1,0,0))
			conPrintI(lua_tostring(L,-1));
		}
		lua_settop(L,Top);
	}

	void __declspec(naked) onPlayerLeaveObservTrap()
	{
		__asm
		{
			push edi
			call onPlayerLeaveObs
			add esp,4
			call sub_4E6040
			push 4E6658h
			ret
		}
	}

	void __cdecl onPlayerGoObs(void *Player)
	{
		int Top=lua_gettop(L);
		getServerVar("playerOnGoObs");
		if (lua_isfunction(L,-1))
		{
			lua_pushlightuserdata(L,Player);
			if (0!=lua_pcall(L,1,0,0))
			conPrintI(lua_tostring(L,-1));
		}
		lua_settop(L,Top);
	}

	void __declspec(naked) onPlayerGoObservTrap()
	{
		__asm
		{
			push ebp
			call onPlayerGoObs
			add esp,4
			call sub_4FC2B0
			push 4E6896h
			ret
		}
	}

	void __cdecl onPlayerDie(BYTE* Player)
	{
		int Top=lua_gettop(L);
		getServerVar("playerOnDie");
		if (lua_isfunction(L,-1))
		{
			lua_pushlightuserdata(L,(void*)Player);
			Player=*(BYTE**)(Player+0x208);
			Player=(BYTE*)unitDamageFindParent((void*)Player);
			if (Player==0)
				lua_pushnil(L);
			else
				lua_pushlightuserdata(L,(void*)Player);
			if (0!=lua_pcall(L,2,0,0))
				conPrintI(lua_tostring(L,-1));
		}
		lua_settop(L,Top);
	}

	int __declspec(naked) onPlayerDieTrap()
	{
		__asm
		{
			mov eax,[esp+4]
			push eax 
			call onPlayerDie
			add esp,4
			mov eax,dword_834ABC
			push 54D2B5h
			ret
		};
	}

	void * __cdecl ControlBufferGrab(void *Player,int N,void *Ret)
	{
		bool Skip=false;
		int Top=lua_gettop(L);
		do
		{
			getServerVar("playerOnInput");
			if (!lua_istable(L,-1))
				break;
			lua_pushlightuserdata(L,Player);
			lua_gettable(L,-2);
			if (!lua_isfunction(L,-1))
				break;
			do
			{
				lua_pushvalue(L,-1);
				lua_pushlightuserdata(L,Player);
				lua_pushinteger(L,*((DWORD*)((BYTE*)Ret+8)) );//Code
				BYTE *Targ=(BYTE*)((bigUnitStruct*)Player)->unitController;
				Targ=*((BYTE **)(Targ+0x120));
				if (Targ)
					lua_pushlightuserdata(L,Targ);
				else
					lua_pushnil(L);
				if (0==lua_pcall(L,3,1,0))
					Skip=lua_toboolean(L,-1);
				else
				{
					conPrintI(lua_tostring(L,-1));
					break;
				}
				if (Skip)
					Ret=(BYTE*)playerControlBufferNext(N,Ret);
				else
					break;
				lua_pop(L,1);
			}while(Ret!=0);

		}
		while (0);
		lua_settop(L,Top);
		return Ret;
	}
	// А это фильтры, которые мы вместо них вставим
	// 6=lclick, 2 = rclick 7=jump
	void __declspec(naked) *myPlayerControlBufferFirst(int N)
	{
		__asm { 
			push [esp+4]
			call playerControlBufferFirst
			add esp,4
			test eax,eax
			jz l1
			push eax
			push [esp+8]
			push edi
			call ControlBufferGrab
			add esp,0xC
l1:
			ret
		};
	}
	void __declspec(naked) *myPlayerControlBufferNext(int N, void *PrevBuf)
	{
__asm { 
			push [esp+8]
			push [esp+8]
			call playerControlBufferNext
			add esp,8
			test eax,eax
			jz l1
			push eax
			push [esp+0x8]
			push edi
			call ControlBufferGrab
			add esp,0xC
l1:
			ret
		};
	}
	int playerLookL(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		if (
			(lua_type(L,2)==LUA_TNIL)
			)
		{
			playerObserveCreUndo(lua_touserdata(L,1));
			return 0;
		}
		 if(lua_type(L,2)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		if (lua_type(L,3)==LUA_TFUNCTION)
		{
		}
		playerObserveCre(lua_touserdata(L,1),lua_touserdata(L,2));
		return 0;
	}
	int __cdecl clientOnJoin(wchar_t *A,wchar_t *B)
	{
		int Top=lua_gettop(L);
		getClientVar("clientOnJoin");
		if (lua_type(L,-1)==LUA_TFUNCTION)
		{
			lua_pcall(L,0,0,0);
		}
		lua_settop(L,Top);
		return wcscmp(A,B);
	}

	void* __cdecl teamKotrGetFirstValidTeam()
	{
		BYTE* team = (BYTE*)noxGetTeamFirst();
		if(team!=NULL)
		{
			BYTE* teamCrown = *((BYTE**)(team+0x4C));
			if(teamCrown==NULL)
			{
				noxTeamDelete((void*)team,1);
				teamAutoAssign(L);
				return teamKotrGetFirstValidTeam();
			}
		}
		return (void*)team;
	}

	void* __cdecl teamKotrGetNextValidTeam(void* prevTeam)
	{
		BYTE* team = (BYTE*)noxGetTeamNext(prevTeam);
		if(team!=NULL)
		{
			BYTE* teamCrown = *((BYTE**)(team+0x4C));
			if(teamCrown==NULL)
			{
				noxTeamDelete((void*)team,1);
				teamAutoAssign(L);
				return teamKotrGetFirstValidTeam();
			}
		}
		return (void*)team;
	}

	void* __cdecl teamCreateNewCorrectly()
	{
		void *R=NULL;
		R=noxTeamCreate(0);
		if(*GameFlags&0x800)
		{}
		else
		{
			if (R==NULL)
			{
				return R;
			}
			serverFlagsSet(4);/// включаем тимы
			wchar_t* RR=NULL;
			RR=noxTeamDefaultName(((byte*)R)[0x38]);//Получаем дефолтное название тимы - то что записано в CSF-ке, на основании цвета тимы
			memcpy(R, RR, 0x28);
			((byte*)R)[0x44]=1;
			teamSendTeam(R); //Тут записывается информация о наличии тимы вообще - однако НАЗВАНИЕ тимы не посылается на клиент - клиент юзает СВОЁ ДЕФОЛТНОЕ название (!)
		}
		return R;
	}

}

/// Вызывается перед попыткой колдовать спелл
/// если 0 - все как обычно (проверки на чат, на саммон и т.п.)
/// если 1 - колдуем
/// если -1 - нет
extern "C" int __cdecl  playerOnTrySpell(bigUnitStruct *Unit,byte *Uc,spellPacket *Pckt)
{
	int Top=lua_gettop(L);
	do
	{
		lua_getglobal(L,"playerOnSpell");
		if (lua_isnil(L,-1))
			break;
		lua_pushlightuserdata(L,Unit);
		lua_gettable(L,-2);
		if (!lua_isfunction(L,-1))
			break;
		lua_pushlightuserdata(L,Unit);
		void *Targ=*((void**)(Uc+0x120));/// текущая цель игрока
		if (Targ)
			lua_pushlightuserdata(L, Targ );
		else
			lua_pushnil(L);
		byte *Pi=(byte *)(*((void**)(Uc+0x114)));
		Pi+=0x8EC;
		lua_pushboolean(L,Pckt->Dir);
		lua_pushinteger(L,Pckt->Spells[0]);
		lua_pushnumber(L,*((DWORD *)Pi));Pi+=4;
		lua_pushnumber(L,*((DWORD *)Pi));
		if (0==lua_pcall(L,6,1,0))
		{
			int R=lua_tointeger(L,-1);
			lua_settop(L,Top);
			return R;
		}
	}while(0);
	lua_settop(L,Top);
	return 0;
}

extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);

void playerInit()
{
	ASSIGN(playerControlBufferFirst,0x0051AB50);
	ASSIGN(playerAtCursor,0x0054AFA2);
	ASSIGN(playerControlBufferNext,0x0051ABC0);
	ASSIGN(playerObserveCre,0x4DDE80);
	ASSIGN(playerObserveCreUndo,0x4DDEF0);
	ASSIGN(netUnitFromPacketMB,0x0004ECCB0);


	ASSIGN(sub_4FC2B0,0x4FC2B0);
	ASSIGN(sub_4E6040,0x4E6040);

	lua_newtable(L);
	registerServerVar("playerOnSpell");//сюда кладем реакции на спеллы плеера

	lua_newtable(L);
	registerServerVar("playerOnInput"); // сюда реакции на действия плеера

	InjectOffs(0x004E637F+1,&myPlayerControlBufferFirst);
	InjectOffs(0x004E639E+1,&myPlayerControlBufferFirst);
	InjectOffs(0x004E67D2+1,&myPlayerControlBufferNext);
	InjectOffs(0x004DD94B+1,&onPlayerJoinTrap);
	InjectOffs(0x00491EB0+1,&clientOnJoin);

	InjectOffs(0x004D216B+1,&teamKotrGetFirstValidTeam);
	InjectOffs(0x004D220A+1,&teamKotrGetNextValidTeam);
	InjectOffs(0x00419234+1,&teamCreateNewCorrectly);

	InjectJumpTo(0x54D2B0,&onPlayerDieTrap);
	InjectJumpTo(0x4E6891,&onPlayerGoObservTrap);
	InjectJumpTo(0x4E6653,&onPlayerLeaveObservTrap);

	registerserver("playerLook",&playerLookL);
	
}
