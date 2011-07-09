#include "stdafx.h"
#include <math.h>

int playersListL(lua_State *L);
void* getPlayerUDataFromPlayerInfo(void *addr);

void (__cdecl *unitSetFollow)(void* Me,void *Him);
void (__cdecl *unitBecomePet)(void* Me,void *Him);
void (__cdecl *noxFrozen) (void* Unit,int Num);
void (__cdecl *noxUnFrozen) (void* Unit,int Num);
void (__cdecl *unitMove)(void *Who,noxPoint *Pt);
void (__cdecl *unitActivate) (void *Unit);
void (__cdecl *dropAllItems)(void*Unit);
void (__cdecl *noxUnitHunt) (void *Unit);

void (__cdecl *inventoryPut)(void *Who,void *What,int A);

bigUnitStruct *(__cdecl *unitDamageFindParent) (void *Unit); // кто источник урона

/// список созданных в текущем кадре на сервере
bigUnitStruct **unitCreatedList;

extern const char *(__cdecl *noxThingNameByType)(int Type);
extern int (__cdecl *noxGetUnitsInRect)(FloatRect *Rect, void (__cdecl *SomeFn)(void *Unit, void *Arg), void *Arg);
extern void *(__cdecl *noxAlloc)(int Size);
extern void (__cdecl *noxFree)(void *Ptr);
extern void printI(const char *S);
struct UnitAndEye_s
{
	float unitX,unitY;
	float unitViewX,unitViewY;
};

int (__cdecl *noxUnknown535250)(UnitAndEye_s *Ptr,int Unk1,int Unk2,int Unk3);//bool
void (__cdecl *unitSetDecayTime)(void *Unit,int Time);//bool
void (__cdecl *noxDeleteUnit)(void *Unit);

bigUnitStruct* (__cdecl *playerFirstUnit)(); ///возвращает первый юнит сетевого игрока
bigUnitStruct* (__cdecl *playerNextUnit)(void* Prev); /// Возвращает следующего сетевого игрока
void* (__cdecl *noxUnitDefByName)(const char *Name);//Возвращает юнитдеф
void (__cdecl *noxDeleteObject)(void *Unit);

unitBigStructPtr	(__cdecl* objectCreateByName)(char const *ObjName);
void 				(__cdecl* noxCreateAt)(unitBigStructPtr Obj,unitBigStructPtr ParentUnit, float X,float Y);


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


void* getPlayerUDataFromPlayerInfo(void* addr)
{
	void *P;
	P=playerFirstUnit();
	while(P!=0)
	{
		BYTE *B=(BYTE *)P;
		B+=0x2EC;//контроллер
		B=*((BYTE**)B);
		B+=0x114;//плэеринфо?
		B=*((BYTE**)B);
		if(addr==B)
			return P;
		P=playerNextUnit(P);
	}
	return 0;
}


void* getPlayerUDataFromPlayerIdx(int idx)
{
	void *P;
	P=playerFirstUnit();
	while(P!=0)
	{
		BYTE *B=(BYTE *)P;
		B+=0x2EC;//контроллер
		B=*((BYTE**)B);
		B+=0x114;//плэеринфо?
		B=*((BYTE**)B);
		B+=0x810;
		int idxFound=*((BYTE*)B);
		if(idx==idxFound)
			return P;
		P=playerNextUnit(P);
	}
	return 0;
}

void* getPlayerUDataFromPlayerNetCode(int netCode)
{
	void *P;
	P=playerFirstUnit();
	while(P!=0)
	{
		BYTE *B=(BYTE *)P;
		B+=0x2EC;//контроллер
		B=*((BYTE**)B);
		B+=0x114;//плэеринфо?
		B=*((BYTE**)B);
		B+=0x80C;
		int netCodeFound=*((short*)B);
		if(netCode==netCodeFound)
			return P;
		P=playerNextUnit(P);
	}
	return 0;
}

namespace
{

	int getUnitCoordL(lua_State *L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
			return 0;
		lua_pushnumber(L, *((float*)(P+0x38)) );
		lua_pushnumber(L, *((float*)(P+0x3C)) );
		return 2;
	}
	void unitPickImpl(void *Unit, void *L_)
	{
		void **LL=(void **)L_;
		*LL=Unit;
	}
	int unitPickL(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TNUMBER)||
			(lua_type(L,2)!=LUA_TNUMBER)||
			(lua_type(L,3)!=LUA_TNUMBER) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *P=0;
		float x=lua_tonumber(L,1),y=lua_tonumber(L,2),d=lua_tonumber(L,3);
		if (d<=0)
			d=1;
		noxGetUnitsInRect(
				&FloatRect(x-d,y-d,x+d,y+d)
				,&unitPickImpl,&P);
		if (P)
			lua_pushlightuserdata(L,P);
		else
			lua_pushnil(L);
		return 1;
	}
	int createObjectL(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TSTRING)||
			(lua_type(L,2)!=LUA_TNUMBER)||
			(lua_type(L,3)!=LUA_TNUMBER) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		bigUnitStruct *T=
			(bigUnitStruct *)objectCreateByName(lua_tostring(L,1));
		if (!T)
			return 0;
		noxCreateAt(T,0,lua_tonumber(L,2),lua_tonumber(L,3));
		if (T->Class  & 0x00400000)// immobile
		{
			BYTE Buf[256],*Out=Buf;
			int Size=16;
			netUniPacket(upNewStatic,Out,Size);
			memcpy(Out,&T->thingType,4);Out+=4;
			memcpy(Out,&T->unitX,4);Out+=4;
			memcpy(Out,&T->unitY,4);Out+=4;
			memcpy(Out,&T->netCode,4);Out+=4;
			netSendAll(Buf,Out-Buf);
		}
		lua_pushlightuserdata(L,T);
		return 1;
	}
	int createObjectIn(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TSTRING)||
			(lua_type(L,2)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		bigUnitStruct *T=
			(bigUnitStruct *)objectCreateByName(lua_tostring(L,1));
		if (!T)
			return 0;
		inventoryPut(lua_touserdata(L,2),T,1);
		lua_pushlightuserdata(L,T);
		return 1;
	}
	int deleteUnitL(lua_State *L)
	{
		if ((lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		bigUnitStruct *P=(bigUnitStruct *)lua_touserdata(L,1);
		if(P!=0)
		{
			if (P->Class&0x400000)///immobile
			{
				BYTE Buf[256],*Out=Buf;
				int Size=4;
				netUniPacket(upDelStatic,Out,Size);
				memcpy(Out,&P->netCode,4);Out+=4;
				netSendAll(Buf,Out-Buf);
			}
			noxDeleteObject(P);
		}
		return 0;
	}
	int playerMouseL(lua_State *L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *P=lua_touserdata(L,1);
		void *Q=playerFirstUnit();
		for (;Q!=NULL;Q=playerNextUnit(Q))
		{
			if (Q==P) break;
		}
		if (Q==NULL)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE *B=(BYTE *)P;B+=0x2EC;//контроллер
		B=*((BYTE**)B);B+=0x114;//плэеринфо?
		B=*((BYTE**)B);B+=0x8EC;
		lua_pushinteger(L,*((int*)B));B+=4;
		lua_pushinteger(L,*((int*)B));
		return 2;
	}
	int unitSetFollowL(lua_State*L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA)||(lua_type(L,2)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *P1,*P2;
		P1=lua_touserdata(L,1);P2=lua_touserdata(L,2);
		if(P1==0 || P2==0)
			return 0;
		unitSetFollow(P1,P2);
		return 0;
	}
	int unitBecomePetL(lua_State*L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA)||(lua_type(L,2)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *P1,*P2;
		P1=lua_touserdata(L,1);P2=lua_touserdata(L,2);
		if(P1==0 || P2==0)
			return 0;
		unitBecomePet(P2,P1);/// Вызывается наоборот
		return 0;
	}

	int unitHuntL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		noxUnitHunt(lua_touserdata(L,1));
		return 0;
	}

	int unitCopyUcL(lua_State*L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE *P=(BYTE*)lua_touserdata(L,1);
		int TT=*((WORD*)(P+0x4));
		if(TT==0)
			return 0;
//		char D[100];
///		wsprintf(D,"tt:%x",TT);
//		printI(D);
		const char*Name=noxThingNameByType(TT);
//		wsprintf(D,"tname:%s",Name);
//		printI(D);
//		return 0;
		BYTE *UnitDef=(BYTE*)noxUnitDefByName(Name);
		int DataSize=*((int*)(UnitDef+0xC4));
		void *Data=noxAlloc(DataSize);
		memcpy(Data,*((void**)(P+0x2EC)),DataSize);
		lua_pushlightuserdata(L,Data);
		return 1;
	}
	int unitDecayL(lua_State*L)
	{	/// добавить в список распада
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) ||
			 (lua_type(L,2)!=LUA_TNUMBER) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		unitSetDecayTime(lua_touserdata(L,1),lua_tointeger(L,2));
		return 0;
	}
	int unitMoveL(lua_State*L)
	{	/// добавить в список распада
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) ||
			 (lua_type(L,2)!=LUA_TNUMBER) ||
			 (lua_type(L,3)!=LUA_TNUMBER) ) 
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		noxPoint Pt(lua_tonumber(L,2),lua_tonumber(L,3));
		unitMove(lua_touserdata(L,1),&Pt);
		return 0;
	}
	
	int unitFreezL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		noxFrozen(lua_touserdata(L,1),1);
		return 1;
	}

	int unitUnFreezL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		noxUnFrozen(lua_touserdata(L,1),1);
		return 1;
	}
	int unitDropAll(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		dropAllItems(lua_touserdata(L,1));
		return 1;
	}
	int unitInventoryPut(lua_State*L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)||
			(lua_type(L,2)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		inventoryPut(lua_touserdata(L,1),lua_touserdata(L,2),1);
		return 0;
	}
	int unitSpeedL(lua_State*L)
	{	
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE *P=(BYTE *) lua_touserdata(L,1);		
		lua_pushnumber(L,*((float*)(P+0x50)));
		lua_pushnumber(L,*((float*)(P+0x54)));
		if (lua_gettop(L)<5)
		{
			return 2;
		}
		if( (lua_type(L,2)!=LUA_TNUMBER) ||
			(lua_type(L,3)!=LUA_TNUMBER) ) 
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		if((lua_gettop(L)>5)&& lua_toboolean(L,4))
		{
			float a=lua_tonumber(L,3),v=lua_tonumber(L,2);
			*((float*)(P+0x50))=v*a;
			*((float*)(P+0x54))=-v*a;
		}
		else
		{
			*((float*)(P+0x50))=lua_tonumber(L,2);
			*((float*)(P+0x54))=lua_tonumber(L,3);
		}
		unitActivate(P);
		return 2;
	}
}
float (__cdecl *getFloatByName)(const char *Name);
extern void unit2Init();
void unitInit()
{
	ASSIGN(getFloatByName,0x00419D40);
	ASSIGN(unitCreatedList,0x00750710);
	ASSIGN(unitDamageFindParent,0x004EC580);
	ASSIGN(unitSetFollow,0x5158C0);
	ASSIGN(unitBecomePet,0x4E7B00);
	ASSIGN(noxUnknown535250,0x535250);
	ASSIGN(playerFirstUnit,0x4DA7C0);
	ASSIGN(playerNextUnit,0x4DA7F0);
	ASSIGN(unitSetDecayTime,0x00511660);
	ASSIGN(noxUnitDefByName,0x004E3830);
	ASSIGN(noxDeleteObject,0x004E5CC0);
	ASSIGN(objectCreateByName,0x004E3810);
	ASSIGN(noxCreateAt,0x004DAA50);
	ASSIGN(unitMove,0x004E7010);
	ASSIGN(unitActivate,0x00537610);
	ASSIGN(noxFrozen,0x004E79C0);
	ASSIGN(noxUnFrozen,0x004E7A60);
	ASSIGN(inventoryPut,0x004F3070);
	ASSIGN(dropAllItems,0x004EDA40);
	ASSIGN(noxUnitHunt,0x5449D0); 

	registerserver("unitSetFollow",&unitSetFollowL);
	registerserver("unitBecomePet",&unitBecomePetL);
	registerserver("unitDecay",&unitDecayL);
	registerserver("unitDelete",&deleteUnitL);
	registerserver("unitCopyUc",&unitCopyUcL);
	registerserver("createObject",&createObjectL);
	registerserver("unitPos",&getUnitCoordL);
	registerserver("unitFreeze",&unitFreezL);
	registerserver("unitUnFreeze",&unitUnFreezL);
	registerserver("unitPick",&unitPickL);
	registerserver("unitHunt",&unitHuntL);

	registerserver("playerList",&playersListL);
	registerserver("playerMouse",&playerMouseL);

	registerserver("unitMove",&unitMoveL);
	registerserver("unitSpeed",&unitSpeedL);
	registerserver("unitDropAll",&unitDropAll);
	registerserver("unitInventoryPut",&unitInventoryPut);
	registerserver("createObjectIn",&createObjectIn);
	unit2Init();
}