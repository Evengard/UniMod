#include "stdafx.h"
#include "unit.h"

short (__cdecl *unitGetHP)(void *U);
void (__cdecl *unitSetHP)(void *U, int New);
short (__cdecl *unitGetMaxHP)(void *U);
void (__cdecl *unitSetMaxHP)(void *U, int New);
void (__cdecl *modifSetItemAttrs)(void *U,void *Enchs[5]);
int (__cdecl *modifGetIdByName)(const char *Name);
void *(__cdecl *modifGetDescById)(int Id);
int (__cdecl *servInventoryPlace)(void* A, void* B, int report, int unkn);

extern void *(__cdecl *noxAlloc)(int Size);
extern void *(__cdecl *noxCAlloc)(int NumElements,int Size);	  


//ASSIGN(,0x);

namespace
{
	struct EnchantDesc
	{
	  char *namePtr;
	  int attrId;
	  void *commentText;
	  void *commentText2;
	  void *attribText;
	  int worth;
	  int color;
	  int allowedWeapon;
	  int allowedArmor;
	  int allowedPosition;
	  void *attackEffectFn;
	  int attackEffectParamFloat;
	  int attackEffectParamDword;
	  void *attackPrehitEffectFn;
	  float attackPrehitEffectParamFloat;
	  int attackPrehitEffectParamDword;
	  int attackPredamageEffectFn;
	  float attackPredamageEffectParamFloat;
	  int attackPredamageEffectParamDword;
	  int defendEffectFn;
	  int defendEffectParamFloat;
	  int defendEffectParamDword;
	  void *unknownHandlerPtr;
	  float defendCollideEffectParamFloat;
	  int defendCollideEffectParamDword;
	  void *updateEffectFn;
	  float updateEffectParamFloat;
	  int updateEffectParamDword;
	  void *engageFn;
	  void *disengageFn;
	  int engageEffectParamFloat;
	  int engageEffectParamDword;
	  float disengageEffectParamFloat;
	  int disengageEffectParamDword;
	  EnchantDesc *nextModif;
	  EnchantDesc *prevModif;
	};
	EnchantDesc **modifLists;

	int *modifCount;

	ParseAttrib enchOffs[]=
	{
		{"name",0x00,2},
		{"tooltip",0x08,4},
		{"text",0x0C,4},
		{"identifyText",0x10,4},
		{"color",0x18,3},
		//{"effectPrehitLvl",
		{0,0,0}
	};
	int itemMakeEnchant(lua_State*L);

	void __cdecl engageFn(EnchantDesc *Me, bigUnitStruct *Owner, bigUnitStruct *Item)
	{
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L,&itemMakeEnchant);
		lua_gettable(L,LUA_REGISTRYINDEX);
		do
		{
			lua_pushlightuserdata(L,Me);
			lua_gettable(L,-2);
			if (!lua_istable(L,-1))
				break;
			lua_getfield(L,-1,"effectEngage");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L, -2);
			lua_pushlightuserdata(L, Owner);
			lua_pushlightuserdata(L, Item);
			if (0 != lua_pcall(L, 3, 0, 0))
			{
				const char *S=lua_tostring(L,-1);
				break;
			}
		}while(0);
		lua_settop(L,Top);
	}
	
	void __cdecl disengageFn(EnchantDesc *Me, bigUnitStruct *Owner, bigUnitStruct *Item)
	{
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L,&itemMakeEnchant);
		lua_gettable(L,LUA_REGISTRYINDEX);
		do
		{
			lua_pushlightuserdata(L,Me);
			lua_gettable(L,-2);
			if (!lua_istable(L,-1))
				break;
			lua_getfield(L,-1,"effectDisengage");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2);
			lua_pushlightuserdata(L, Owner);
			lua_pushlightuserdata(L, Item);
			if (0 != lua_pcall(L, 3, 0, 0))
			{
				const char *S=lua_tostring(L,-1);
				break;
			}
		}while(0);
		lua_settop(L,Top);
	}
	
	// Функция, вызываемая каждый раз при ПОПАДАНИИ атаки зачарованным оружием.
	// Вызывает луа обработчик с аргументами: указатель на чар, указатель на оружие, указатель на владельца, указатель на цель
	void __cdecl attackPrehitFn(EnchantDesc *Me, bigUnitStruct *Weapon, bigUnitStruct *Owner, bigUnitStruct *Target, float *Damage)
	{
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L,&itemMakeEnchant);
		lua_gettable(L,LUA_REGISTRYINDEX);
		do
		{
			lua_pushlightuserdata(L,Me);
			lua_gettable(L,-2);
			if (!lua_istable(L,-1))
				break;
			lua_getfield(L,-1,"effectPrehit");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2);
			lua_pushlightuserdata(L,Weapon);
			lua_pushlightuserdata(L,Owner);
			lua_pushlightuserdata(L,Target);
			if (0!=lua_pcall(L,4,0,0))
			{
				break;
			}
		} while(0);
		lua_settop(L,Top);
	}
	
	// Функция, вызываемая каждый раз при НАЧАЛЕ атаки зачарованным оружием.
	// Вызывает луа обработчик с аргументами: указатель на чар, указатель на оружие, указатель на владельца, урон
	// Если имеется выходное значение, устанавливает его как множитель урона
	void __cdecl attackManualFn(EnchantDesc *Me, bigUnitStruct *Weapon, bigUnitStruct *Owner, void *Zero, float *Damage)
	{
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L, &itemMakeEnchant);
		lua_gettable(L, LUA_REGISTRYINDEX);
		do
		{
			lua_pushlightuserdata(L, Me);
			lua_gettable(L, -2);
			if (!lua_istable(L, -1))
				break;
			lua_getfield(L, -1, "effectAttack");
			if (!lua_isfunction(L, -1))
				break;
			lua_pushvalue(L, -2);
			lua_pushlightuserdata(L, Weapon);
			lua_pushlightuserdata(L, Owner);
			lua_pushnumber(L, *Damage);
			if (lua_pcall(L, 4, 1, 0) == 0)
			{
				if (!lua_isnil(L, -1))
				{
					*Damage = (float) lua_tonumber(L, -1);
				}
			}
		} while(0);
		lua_settop(L,Top);
	}
	
	// Функция, вызываемая каждый тик для брони в инвентаре у монстра/игрока.
	// Вызывает луа обработчик с аргументами: указатель на чар, указатель на броню, указатель на владельца
	void __cdecl updateArmorFn(EnchantDesc *Me, bigUnitStruct *Armor, int zero)
	{
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L, &itemMakeEnchant);
		lua_gettable(L, LUA_REGISTRYINDEX);
		do
		{
			lua_pushlightuserdata(L, Me);
			lua_gettable(L, -2);
			if (!lua_istable(L, -1))
				break;
			lua_getfield(L, -1, "effectUpdate");
			if (!lua_isfunction(L, -1))
				break;
			lua_pushvalue(L, -2);
			lua_pushlightuserdata(L, Armor);
			lua_pushlightuserdata(L, Armor->prevInventoryObj);
			if (lua_pcall(L, 3, 0, 0) != 0)
			{
				break;
			}
		} while(0);
		lua_settop(L,Top);
	}
	
	// Создает пользовательский энчант. Аргумент - таблица
	int itemMakeEnchant(lua_State*L)
	{
		if (
			(lua_type(L,1)!=LUA_TTABLE)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_settop(L,1);
		lua_pushnil(L);
		EnchantDesc *Ench=(EnchantDesc *)noxCAlloc(1,0x90);
		while (lua_next(L,1)!=0)
		{
			lua_pushvalue(L,-2);
			lua_pushvalue(L,-2);
			parseAttr(L,4,5,Ench,enchOffs);
			lua_settop(L,2);
		}
		lua_getfield(L,1,"effectEngage");
		if (lua_isfunction(L,-1))
		{
			Ench->engageFn=&engageFn;
		}
		lua_getfield(L,1,"effectDisengage");
		if (lua_isfunction(L,-1))
		{
			Ench->disengageFn=&disengageFn;
		}
		lua_getfield(L,1,"effectPrehit");
		if (lua_isfunction(L,-1))
		{
			Ench->attackPrehitEffectFn=&attackPrehitFn;
		}
		lua_getfield(L,1,"effectAttack");
		if (lua_isfunction(L,-1))
		{
			Ench->attackEffectFn=&attackManualFn;
		}
		lua_getfield(L,1,"effectUpdate");
		if (lua_isfunction(L,-1))
		{
			Ench->updateEffectFn=&updateArmorFn;
		}
		lua_settop(L,2);

		Ench->attrId=(*modifCount)++;
		EnchantDesc *&Next=modifLists[0];

		Ench->prevModif=NULL;
		Ench->nextModif=Next;
		if (Next)
			Next->prevModif=Ench;
		Next=Ench;
		
		lua_pushlightuserdata(L,&itemMakeEnchant);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Ench);
		lua_pushvalue(L,1);
		lua_settable(L,-3);
		lua_settop(L,1);
		return 1;
	}
	int itemEnchants(lua_State*L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		bigUnitStruct *B=(bigUnitStruct*)lua_touserdata(L,1);
		if (B==NULL || (0==B->Class & (clWeapon|clArmor|clWand) ) ) 
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_settop(L,1+4);
		EnchantDesc **Src=(EnchantDesc**)B->unkFn2B0DataPtr;
		lua_createtable(L,5,0);
		if (Src!=NULL)
		{
			for (int i=1;i<=4;Src++,i++)
			{
				if (NULL == *Src || *Src==((void *)0xFFFFFFFFLL) || (*Src)->namePtr==NULL)
					lua_pushstring(L,"");
				else
					lua_pushstring(L,(*Src)->namePtr);
				lua_rawseti(L,-2,i);
			}
		}
		void *Enchants[5]={0,0,0,0,0};
		bool Set=false;
		for (int i=2;i<=5;i++)
		{
			if (!lua_isnil(L,i))
			{
				Set=true;
				size_t Len;
				const char *S=lua_tolstring(L,i,&Len);
				Enchants[i-2]=NULL;
				if (S!=NULL && Len>0)
				{
					int Id=modifGetIdByName(S);
					if (Id)
						Enchants[i-2]=modifGetDescById(Id);
				}
			}
		}
		if (Set)
			modifSetItemAttrs(B,&Enchants[0]);
		return 1;
	}
	
	// возвращает или устанавливает здоровье
	int unitHP(lua_State*L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *P=lua_touserdata(L,1);
		if (P==NULL)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushinteger(L,(short) unitGetHP(P));
		if (!lua_isnil(L,2))
		{
			int New = (int) lua_tonumber(L,2);
			unitSetHP(P,New<1?1:New);
		}
		return 1;
	}
	
	// наносит урон обьекту. аргументы: ю1, ю2, ю3, урон, тип урона
	int unitDamageL(lua_State *L)
	{
		if ((lua_type(L, 1) != LUA_TLIGHTUSERDATA))
		{
			lua_pushstring(L, "wrong args: arg1 != userdata");
			lua_error_(L);
		}
		bigUnitStruct *victim = (bigUnitStruct *) lua_touserdata(L, 1);
		if (victim == NULL)
		{
			lua_pushstring(L, "wrong args: arg1 is nil");
			lua_error_(L);
		}
		void *attacker = NULL;
		if (!lua_isnil(L, 2)) { attacker = lua_touserdata(L, 2); }
		void *dealtBy = NULL;
		if (!lua_isnil(L, 3)) { dealtBy = lua_touserdata(L, 3); }
		if (!lua_isnil(L, 4) && !lua_isnil(L, 5))
		{
			int damage = (int) lua_tonumber(L, 4);
			int dmgType = (int) lua_tonumber(L, 5);
			victim->damageFnPtr(victim, attacker, dealtBy, damage, dmgType);
		}
		return 0;
	}
	
	// исправленная версия unitInventoryPut
	// принимает два аргумента userdata, возвращает integer результат
	int unitPlaceInvL(lua_State *L)
	{
		if ((lua_type(L, 1) != LUA_TLIGHTUSERDATA))
		{
			lua_pushstring(L, "wrong args: arg1 is not userdata");
			lua_error_(L);
		}
		void* whotake = lua_touserdata(L, 1);
		if ((lua_type(L, 2) != LUA_TLIGHTUSERDATA))
		{
			lua_pushstring(L, "wrong args: arg2 is not userdata");
			lua_error_(L);
		}
		void *toplace = lua_touserdata(L, 2);

		int result = servInventoryPlace(whotake, toplace, 1, 1);
		lua_pushinteger(L, result);
		return 1;
	}
}

void unit2Init()
{
	ASSIGN(unitGetHP,0x004EE780);
	ASSIGN(unitSetHP,0x004E4560);
	ASSIGN(unitGetMaxHP, 0x4EE7A0);
	ASSIGN(unitSetMaxHP, 0x4EE7C0);
	ASSIGN(modifSetItemAttrs,0x004E4990);
	ASSIGN(modifGetDescById,0x00413330);
	ASSIGN(modifGetIdByName,0x00413290);
	ASSIGN(servInventoryPlace, 0x4F36F0);
	ASSIGN(modifLists,0x00611C54);
	ASSIGN(modifCount,0x00611C60);
	
	registerserver("unitHP",&unitHP);	
	registerserver("itemEnchants",&itemEnchants);
	registerserver("itemMakeEnchant",&itemMakeEnchant);
	registerserver("unitDamage",&unitDamageL);
	registerserver("unitInventoryPlace",&unitPlaceInvL);
	
	lua_pushlightuserdata(L,&itemMakeEnchant);
	lua_newtable(L);
	lua_settable(L,LUA_REGISTRYINDEX);
}