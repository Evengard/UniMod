#include "stdafx.h"
#include "unit.h"

#define ASSIGN2(A,B) *((void**)&(A))=(B);

extern int (__cdecl *spellAccept)(int SpellType,void *Caster,void *CasterMB2,void *Carrier,
								SpellTargetBlock *Block,int Power);

int (__cdecl *monsterGetVoice)(void* Monster); // вообще птр возвращается
creatureAction* (__cdecl *monsterActionPush)(void *Unit,int ActionType);
void (__cdecl *monsterActionPop)(void *Unit);
void (__cdecl *noxReportComplete)(void *Unit);
void printI(const char *S);
void (__cdecl *noxMonsterCallDieFn) (void *Unit);
namespace
{
	void (unitFlyActivate)(int SpellType,void *Owner,void *Source,void *Carrier,
								SpellTargetBlock *Block,int Power)
	{
		int Top=lua_gettop(L);
		do
		{
			lua_getglobal(L,"unitFlyActivate");
			if (!lua_istable(L,-1))
				break;
			lua_pushlightuserdata(L,Carrier);
			lua_gettable(L,-2);
			if (!lua_isfunction(L,-1))
				break;
			lua_pushinteger(L,SpellType);
			lua_pushlightuserdata(L,Owner);
			lua_pushlightuserdata(L,Source);
			lua_pushlightuserdata(L,Carrier);
			lua_pushnumber(L,Block->targX);
			lua_pushnumber(L,Block->targY);
			lua_pushinteger(L,Power);
			lua_pcall(L,7,0,0);
			lua_settop(L,Top);
			return;
		}
		while(0);
		lua_settop(L,Top);
		spellAccept(SpellType,Owner,Source,Carrier,Block,Power);
	}
	
	int hookMonsterMeleeAttack(void* Unit)
	{
		int voice = monsterGetVoice(Unit);
		int Top=lua_gettop(L);
		do
		{
			lua_getglobal(L,"monsterAttackSuccess");
			if (!lua_istable(L,-1))
				break;
			lua_pushlightuserdata(L,Unit);
			lua_gettable(L,-2);
			if (!lua_isfunction(L,-1))
				break;
			lua_pushlightuserdata(L,Unit);
			lua_pcall(L,1,0,0);
			lua_settop(L,Top);
			return voice;
		}
		while(0);
		lua_settop(L,Top);
		return voice; // хук был поставлен на эту функцию
	}

	int __cdecl reactUse(bigUnitStruct *User,bigUnitStruct *Thing)
	{
		lua_pushlightuserdata(L,&reactUse);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Thing);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,2);
			return 0;
		}
		lua_pushlightuserdata(L,User);
		lua_pushlightuserdata(L,Thing);
		void **P=(void **)(Thing->useFnData);
		if(*P==NULL)
			lua_pushnil(L);
		else
			lua_pushlightuserdata(L,*P);
		if(0!=lua_pcall(L,3,1,0))
		{
			printI("Error calling react");
			lua_pop(L,2);
			return 0;
		}
		int R=lua_toboolean(L,-1);
		lua_pop(L,2);
		return R;
	}
	int setOnUseL(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)||
			(lua_type(L,2)!=LUA_TFUNCTION)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushvalue(L,1);
		///Здесь неплохо бы зарегить уничтожение объекта, чтобы таблица не росла
		lua_pushvalue(L,2);
		lua_settable(L,lua_upvalueindex(2));
		bigUnitStruct *Unit=(bigUnitStruct *)lua_touserdata(L,1);
		Unit->useFnPtr=&reactUse;
		return 0;
	}
	int unitSetAction(lua_State*L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)||
			(lua_type(L,2)!=LUA_TNUMBER)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *P=monsterActionPush(lua_touserdata(L,1),lua_tointeger(L,2));
		lua_pushlightuserdata(L,P);
		return 1;
	}
	int unitActionPop(lua_State*L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		monsterActionPop(lua_touserdata(L,1));
		return 0;
	}
	void __cdecl unitReportComplete(bigUnitStruct *Unit)
	{
		lua_pushlightuserdata(L,&unitReportComplete);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Unit);
		lua_gettable(L,-2);
		lua_pushlightuserdata(L,Unit);
		lua_pushnil(L);
		lua_settable(L,-4);// сразу удалим, чтобы не получить рекурсию
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			noxReportComplete(Unit);
			lua_pop(L,2);
			return;
		}
		lua_pushlightuserdata(L,Unit);
		if(0!=lua_pcall(L,1,1,0))
		{
			lua_pop(L,2);
			return;
		}
		lua_pop(L,2);
		return;
	}
	int setOnCompleteL(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)||
			(lua_type(L,2)!=LUA_TFUNCTION)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushvalue(L,1);
		///Здесь неплохо бы зарегить уничтожение объекта, чтобы таблица не росла
		lua_pushvalue(L,2);
		lua_settable(L,lua_upvalueindex(1));
		return 0;
	}
	int callPtr2L(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TNUMBER)||
			(0==lua_tointeger(L,1) ) ||
			(lua_type(L,2)!=LUA_TLIGHTUSERDATA)||
			(lua_type(L,3)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *(__cdecl *Fn)(void *A,void *B);
		Fn=(void *(__cdecl *)(void *,void *))lua_tointeger(L,1);
		lua_pushlightuserdata(L,Fn(lua_touserdata(L,2),lua_touserdata(L,3)));
		return 1;
	}
	void __cdecl dieFn(bigUnitStruct *Me);


	int unitSetDieFnL(lua_State*L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		if(lua_type(L,2)!=LUA_TFUNCTION)/// удаляем нафиг наш обработчик
		{
			lua_pushvalue(L,1);
			lua_gettable(L,lua_upvalueindex(1));
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong arg: can't remove die handler - it doesn't exist!");
				lua_error_(L);
			}
			bigUnitStruct *P=(bigUnitStruct *)lua_touserdata(L,1);
			P->dieFnPtr=lua_touserdata(L,-1);
			lua_pop(L,1);
			lua_pushvalue(L,1);
			lua_pushnil(L);
			lua_settable(L,lua_upvalueindex(1));
			return 0;
		}
		bigUnitStruct *P=(bigUnitStruct *)lua_touserdata(L,1);
		lua_pushvalue(L,1);
		lua_pushlightuserdata(L,P->dieFnPtr);
		lua_settable(L,lua_upvalueindex(1));/// запишем в таблицу
		lua_pushvalue(L,1);
		lua_pushvalue(L,2);
		lua_settable(L,lua_upvalueindex(2));/// запишем в таблицу что нам вызывать
		P->dieFnPtr=&dieFn;
		return 0;
	}
	
	void __cdecl dieFn(bigUnitStruct *Me)
	{
		lua_pushlightuserdata(L,&dieFn);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,(void*)Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,1);
			return;
		}
		
		lua_pushlightuserdata(L, Me);
		bigUnitStruct *Unit=unitDamageFindParent(Me->unitDamaged);
			if (Unit==0)
				lua_pushnil(L);
			else
				lua_pushlightuserdata(L, (void*)Unit);
					
		if(0!=lua_pcall(L,2,1,0))/// BUGBUG косяк в том, что если внутри функции удалить 
			/// объект - то там еще старое значение
		{
			conPrintI(lua_tostring(L,-1));
			lua_pop(L,1);
			lua_pushnil(L);
		}
		lua_pushlightuserdata(L,&unitSetDieFnL);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,(void*)Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pop(L,4);/// что достали - таблица - результат функции- таблица
			return;
		}
		void (__cdecl *Old)(void *);
		Old=(void (__cdecl *)(void *))lua_touserdata(L,-1);

		Me->dieFnPtr=(void*)Old;

		lua_pushlightuserdata(L,Me);
		lua_pushnil(L);
		lua_settable(L,-4);// Удаляем себя из таблицы
		if ( (Old!=NULL) && (0==lua_toboolean(L,-3)))
		{
			Old(Me);
		}
		lua_pop(L,4);/// удаляем таблицу
		return;
	}
	void __cdecl collideFn(bigUnitStruct *Me,bigUnitStruct *Him,noxPoint *Pt);


	int unitSetCollideFnL(lua_State*L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		if(lua_type(L,2)!=LUA_TFUNCTION)/// удаляем нафиг наш обработчик
		{
			lua_pushvalue(L,1);
			lua_gettable(L,lua_upvalueindex(1));
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong arg: can't remove collide handler - it doesn't exist!");
				lua_error_(L);
			}
			bigUnitStruct *P=(bigUnitStruct *)lua_touserdata(L,1);
			P->collideFn=lua_touserdata(L,-1);
			lua_pop(L,1);
			lua_pushvalue(L,1);
			lua_pushnil(L);
			lua_settable(L,lua_upvalueindex(1));
			return 0;
		}
		bigUnitStruct *P=(bigUnitStruct *)lua_touserdata(L,1);
		lua_pushvalue(L,1);
		lua_pushlightuserdata(L,P->collideFn);
		lua_settable(L,lua_upvalueindex(1));/// запишем в таблицу
		lua_pushvalue(L,1);
		lua_pushvalue(L,2);
		lua_settable(L,lua_upvalueindex(2));/// запишем в таблицу что нам вызывать
		P->collideFn=&collideFn;
		return 0;
	}
	
	void __cdecl collideFn(bigUnitStruct *Me,bigUnitStruct *Him,noxPoint *Pt)
	{
		lua_pushlightuserdata(L,&collideFn);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,1);
			return;
		}
		lua_pushlightuserdata(L,Me);
		if(Him==0)
			lua_pushnil(L);
		else
			lua_pushlightuserdata(L,Him);
		lua_pushnumber(L,Pt->X);
		lua_pushnumber(L,Pt->Y);
		if(0!=lua_pcall(L,4,0,0))
			{ lua_pop(L,2); return;}
		lua_pop(L,1);/// удаляем таблицу
		return;
	}
	int unitSetAnyFnL(lua_State*L,int Ofs,void *newAddr)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		if(lua_type(L,2)!=LUA_TFUNCTION)/// удаляем нафиг наш обработчик
		{
			lua_pushvalue(L,1);
			lua_gettable(L,lua_upvalueindex(1));
			if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
			{
				lua_pushstring(L,"wrong arg: can't remove handler - it doesn't exist!");
				lua_error_(L);
			}
			BYTE* P=(BYTE *)lua_touserdata(L,1);
			P+=Ofs;
			*((void **)P)=lua_touserdata(L,-1);
			lua_pop(L,1);
			lua_pushvalue(L,1);
			lua_pushnil(L);
			lua_settable(L,lua_upvalueindex(1));
			return 0;
		}
		BYTE* P=(BYTE *)lua_touserdata(L,1);
		P+=Ofs;
		lua_pushvalue(L,1);
		lua_pushlightuserdata(L,*((void **)P));
		lua_settable(L,lua_upvalueindex(1));/// запишем в таблицу
		lua_pushvalue(L,1);
		lua_pushvalue(L,2);
		lua_settable(L,lua_upvalueindex(2));/// запишем в таблицу что нам вызывать
		*((void **)P)=newAddr;
		return 0;
	}
	void initFn(lua_State*L,const char *Name,lua_CFunction luaFn,void *Fn)
	{
		lua_newtable(L);// для замененных функций
		lua_pushlightuserdata(L,luaFn);
		lua_pushvalue(L,-2);
		lua_settable(L,LUA_REGISTRYINDEX);
		lua_newtable(L);// для старых функций
		lua_pushlightuserdata(L,Fn);
		lua_pushvalue(L,-2);
		lua_settable(L,LUA_REGISTRYINDEX);
		lua_pushcclosure(L,luaFn,2);
		lua_setglobal(L,Name);

	}
	int unitSetDropL(lua_State*L);
	int __cdecl dropFn(void *Him,void *Me,noxPoint* Pt)
	{
		lua_pushlightuserdata(L,&dropFn);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,1);
			return 0;
		}
		lua_pushlightuserdata(L,Me);
		if(Him==0)
			lua_pushnil(L);
		else
			lua_pushlightuserdata(L,Him);
		lua_pushnumber(L,Pt->X);
		lua_pushnumber(L,Pt->Y);
		if(0!=lua_pcall(L,4,1,0))
			{ lua_pop(L,2); return 0;}

		lua_pushlightuserdata(L,unitSetDropL);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pop(L,4);/// что достали - таблица - результат функции- таблица
			return 1;
		}
		int (__cdecl* fn) (void *Him,void *Me,noxPoint* Pt);
		ASSIGN2(fn,lua_touserdata(L,-1));
		int ret=1;
		if((fn!=NULL)&&(0==lua_toboolean(L,-3)))
		{
			ret=fn(Him,Me,Pt);
		}
		lua_pop(L,4);/// удаляем таблицу
		return ret;
	}
	int unitSetDropL(lua_State*L)
	{
		return unitSetAnyFnL(L,0x2C8,&dropFn);
	}
	int unitSetDamageL(lua_State*L);
	int __cdecl damageFn(void *Me,void *Him,void *By,int Value,int Type)
	{
		lua_pushlightuserdata(L,&damageFn);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,1);
			return 0;
		}
		lua_pushlightuserdata(L,Me);
		if(Him==0)
			lua_pushnil(L);
		else
			lua_pushlightuserdata(L,Him);
		if(By==0)
			lua_pushnil(L);
		else
			lua_pushlightuserdata(L,By);
		lua_pushnumber(L,Value);
		lua_pushnumber(L,Type);
		if(0!=lua_pcall(L,5,2,0))
			{ lua_pop(L,2); return 0;}

		lua_pushlightuserdata(L,unitSetDamageL);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pop(L,5);/// что достали - таблица - вызывать си? - дамак - таблица
			return 1;
		}
		int (__cdecl* fn) (void *Him,void *Me,void *By,int Val,int Type);
		ASSIGN2(fn,lua_touserdata(L,-1));
		int ret=1;
		if (lua_isnumber(L,-3))
			Value=lua_tointeger(L,-3);
		if ((fn!=NULL) && (0==lua_toboolean(L,-4)))
		{
			ret=fn(Me,Him,By,Value,Type);
		}
		lua_pop(L,5);/// удаляем таблицу
		return ret;
	}
	int unitSetDamageL(lua_State*L)
	{
		return unitSetAnyFnL(L,0x2CC,&damageFn);
	}
	
	// обработчик UPDATE функции, передает 1 аргумент - юнит
	int unitSetUpdateL(lua_State*L);
	void __cdecl updateFn(bigUnitStruct *Me)
	{
		lua_pushlightuserdata(L,&updateFn);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,1);
			return;
		}
		lua_pushlightuserdata(L,Me);
		if (0!=lua_pcall(L,1,0,0)) return;

		lua_pushlightuserdata(L,&unitSetUpdateL);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pop(L,4);/// что достали - таблица - результат функции- таблица
			return;
		}
		/*
		void (__cdecl *Old)(void *);
		Old=(void (__cdecl *)(void *))lua_touserdata(L,-1);

		Me->onUpdateFn = (void*)Old;

		lua_pushlightuserdata(L,Me);
		lua_pushnil(L);
		lua_settable(L,-4);// Удаляем себя из таблицы
		if ( (Old!=NULL) && (0==lua_toboolean(L,-3)))
		{
			Old(Me);
		}
		lua_pop(L,4);/// удаляем таблицу
		*/
		return;
	}
	int unitSetUpdateL(lua_State*L)
	{
		return unitSetAnyFnL(L,0x2E8,&updateFn);
	}
	
	int unitSetPickupL(lua_State*L);
	int __cdecl pickupFn(void *Him,void *Me,void *A,void *B)
	{
		lua_pushlightuserdata(L,&pickupFn);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,1);
			return 0;
		}
		lua_pushlightuserdata(L,Me);
		if(Him==0)
			lua_pushnil(L);
		else
			lua_pushlightuserdata(L,Him);
		lua_pushlightuserdata(L,A);
		lua_pushlightuserdata(L,B);
		if(0!=lua_pcall(L,4,1,0))
			{ lua_pop(L,2); return 0;}

		lua_pushlightuserdata(L,unitSetPickupL);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Me);
		lua_gettable(L,-2);
		if(lua_type(L,-1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pop(L,4);/// что достали - таблица - результат функции- таблица
			return 1;
		}
		int (__cdecl* fn) (void *Him,void *Me,void *A,void *B);
		ASSIGN2(fn,lua_touserdata(L,-1));
		int ret=1;
		if((fn!=NULL)&&(0==lua_toboolean(L,-3)))
		{
			ret=fn(Him,Me,A,B);
		}
		lua_pop(L,4);/// удаляем таблицу
		return ret;
	}
	int unitSetPickupL(lua_State*L)
	{
		return unitSetAnyFnL(L,0x2C4,&pickupFn);
	}

	void __declspec(naked) asmOnDieMonster() // Важная хрень, что бы unitOnDie для мобов вызывался
	{
		__asm
		{
			mov eax,[esi+2D4h]
			test eax,eax
			jz l1
			call eax 
			add esp,4
			push esi

			l1:
			call noxMonsterCallDieFn
			push 0x4EE6AA
			ret
		}
	}
	
}
extern void InjectJumpTo(DWORD Addr,void *Fn);
extern void InjectOffs(DWORD Addr,void *Fn);
void reactInit()
{
	InjectOffs(0x004E96D5+1,&unitFlyActivate);
	InjectOffs(0x005324FA+1,&hookMonsterMeleeAttack);
	InjectJumpTo(0x4EE6A5,&asmOnDieMonster);
	
	ASSIGN(monsterActionPop,0x0050A160);
	ASSIGN(monsterActionPush,0x0050A260);
	ASSIGN(noxReportComplete,0x544FF0);
	ASSIGN(noxMonsterCallDieFn,0x50A3D0);
	ASSIGN(monsterGetVoice,0x424300);
	void** V=(void **)(0x5BFEC8+0x204);
	*V=&unitReportComplete;


	initFn(L,"unitOnComplete",&setOnCompleteL,&unitReportComplete);
	initFn(L,"unitOnUse",&setOnUseL,&reactUse);
	initFn(L,"unitOnDie",&unitSetDieFnL,&dieFn);
	initFn(L,"unitOnPickup",&unitSetPickupL,&pickupFn);
	initFn(L,"unitOnDrop",&unitSetDropL,&dropFn);
	initFn(L,"unitOnDamage",&unitSetDamageL,&damageFn);
	initFn(L,"unitOnUpdate",&unitSetUpdateL,&updateFn);
	initFn(L,"unitOnCollide",&unitSetCollideFnL,&collideFn);

	registerserver("unitSetAction",&unitSetAction);
	registerserver("unitActionPop",&unitActionPop);
	
	lua_newtable(L);
	registerServerVar("unitFlyActivate");
	
	lua_newtable(L);
	registerServerVar("monsterAttackSuccess");

	registerserver("ptrCall2",&callPtr2L);
}