#include "stdafx.h"

int (__cdecl *unitGetHP)(void *U);
void (__cdecl *unitSetHP)(void *U,int New);
void (__cdecl *modifSetItemAttrs)(void *U,void *Enchs[5]);
int (__cdecl *modifGetIdByName)(const char *Name);
void *(__cdecl *modifGetDescById)(int Id);

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
	  int attackPrehitEffectFn;
	  float attackPrehitEffectParamFloat;
	  int attackPrehitEffectParamDword;
	  int attackPredamageEffectFn;
	  float attackPredamageEffectParamFloat;
	  int attackPredamageEffectParamDword;
	  int defendEffectFn;
	  int defendEffectParamFloat;
	  int defendEffectParamDword;
	  char gap_58[4];
	  float defendCollideEffectParamFloat;
	  int defendCollideEffectParamDword;
	  void *updateEffectFn;
	  float updateEffectParamFloat;
	  int updateEffectParamDword;
	  void (__cdecl*engageFn)(EnchantDesc *Me,bigUnitStruct *Owner);
	  void (__cdecl*disengageFn)(EnchantDesc *Me,bigUnitStruct *Owner);
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
		{0,0,0}
	};
	int itemMakeEnchant(lua_State*L);

	void __cdecl engageFn(EnchantDesc *Me,bigUnitStruct *Owner)
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
			lua_getfield(L,-1,"onEngage");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2);
			lua_pushlightuserdata(L,Owner);
			if (0!=lua_pcall(L,2,0,0))
			{
				const char *S=lua_tostring(L,-1);
				break;
			}
		}while(0);
		lua_settop(L,Top);
	}
	void __cdecl disengageFn(EnchantDesc *Me,bigUnitStruct *Owner)
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
			lua_getfield(L,-1,"onDisengage");
			if (!lua_isfunction(L,-1))
				break;
			lua_pushvalue(L,-2);
			lua_pushlightuserdata(L,Owner);
			if (0!=lua_pcall(L,2,0,0))
			{
				const char *S=lua_tostring(L,-1);
				break;
			}
		}while(0);
		lua_settop(L,Top);
	}

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
		lua_getfield(L,1,"onEngage");
		if (lua_isfunction(L,-1))
		{
			Ench->engageFn=&engageFn;
		}
		lua_getfield(L,1,"onDisengage");
		if (lua_isfunction(L,-1))
		{
			Ench->disengageFn=&disengageFn;
		}
/*		lua_getfield(L,1,"onUpdate");
		if (lua_isfunction(L,-1))
		{
			Ench->updateEffectFn=&disengageFn;
		}*/
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
		lua_pushinteger(L,unitGetHP(P));
		if (!lua_isnil(L,2))
		{
			int New=lua_tonumber(L,2);
			unitSetHP(P,New<1?1:New);
		}
		return 1;
	}
}

void unit2Init()
{
	ASSIGN(unitGetHP,0x004EE780);
	ASSIGN(unitSetHP,0x004E4560);
	ASSIGN(modifSetItemAttrs,0x004E4990);
	ASSIGN(modifGetDescById,0x00413330);
	ASSIGN(modifGetIdByName,0x00413290);
	
	ASSIGN(modifLists,0x00611C54);
	ASSIGN(modifCount,0x00611C60);
	
	registerserver("unitHP",&unitHP);	
	registerserver("itemEnchants",&itemEnchants);
	registerserver("itemMakeEnchant",&itemMakeEnchant);
	
	lua_pushlightuserdata(L,&itemMakeEnchant);
	lua_newtable(L);
	lua_settable(L,LUA_REGISTRYINDEX);
}