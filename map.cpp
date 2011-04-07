#include "stdafx.h"

struct wallBreakable_s
{
	wallBreakable_s *next;
	wallRec *wall;
};


int getTileByName(const char*);
wallRec *(__cdecl *noxGetWallAtPt)(int x,int y);
int (__cdecl *noxWallTileByName)(const char *Name);
wallRec *(__cdecl *noxWallCreateAt)(int x,int y);
void (__cdecl *noxMapDelWallAtPt)(int x,int y);
int (__cdecl *mapTraceRay)(noxRect *Ray,int arg4,int arg8,int Flags);
// arg4,arg8 - ret structs [0],[4] - some vals
void (__cdecl *netWallCreate)(void *PrevCmd,wallRec *Wall,int newWall,int tileName,int facing, int vari);
//newWall=1 если нова€ стена, 0 - если модифицирована стара€

DWORD *wallNextBreakableId;
wallBreakable_s const *(*noxGetFirstBreakableList)();//0x00410870
void (__cdecl *noxWallBreackableListAdd)(wallRec * Wall);/// ƒобавл€ем в список разрушимых стен
void (__cdecl *noxWallBreackableListRemove)(void const * WallList);/// јргумент - блок, ”дал€ем из списка 

extern void printI(const char *S);
extern void conPrintI(const char *S);
char *(__cdecl *mapGetName)();

DWORD (__cdecl *scriptPopValue)();
void (__cdecl *scriptPushValue)(DWORD X);
char **scriptKey; // все строчки лежат там
DWORD (__cdecl *mapWaypoint)();
void (__cdecl *mapInitialize)();

bigUnitStruct **scriptCallerUnit;
bigUnitStruct **scriptTriggerUnit;

extern bigUnitStruct *(__cdecl *unitDamageFindParent) (void *Unit);
extern void mapUnloadFilesystem();
extern void mapUnloadUtil();
extern void mapLoadFilesystem(const char *);
extern void mapLoadSpells();
namespace {
	DWORD myWaypoint()
	{
		DWORD Idx=scriptPopValue();
		char *S=scriptKey[Idx];
		if (*S!='>')
		{
			scriptPushValue(Idx);
			return mapWaypoint();
		}
		S++;
		int T=lua_gettop(L);
		if (*scriptCallerUnit)
		{
			void *Parent=unitDamageFindParent(*scriptCallerUnit);
			if (Parent)
				lua_pushlightuserdata(L,Parent);
			else
				lua_pushnil(L);
			lua_pushlightuserdata(L,*scriptCallerUnit);
			

		}
		else
		{
			lua_pushnil(L);
			lua_pushnil(L);
		}
		if (*scriptTriggerUnit)
			lua_pushlightuserdata(L,*scriptTriggerUnit);
		else
			lua_pushnil(L);
		
		lua_getfield(L,LUA_REGISTRYINDEX,"server");// смена контекста
		lua_pushstring(L,S);
		lua_gettable(L,-2);
		lua_remove(L,-2);
		if (lua_type(L,-1)==LUA_TFUNCTION)
		{
			lua_getfield(L,LUA_REGISTRYINDEX,"server");// смена контекста
			lua_setfenv(L,-2);

			lua_pushvalue(L,-4);//parent			
			lua_pushvalue(L,-4);//caller
			lua_pushvalue(L,-4);//trigger
			if (0!=lua_pcall(L,3,0,0))
			{
				conPrintI(lua_tostring(L,-1));
			}
		}
		else
		{
			lua_pop(L,1);
			lua_setglobal(L,"scriptTrigger");
			lua_setglobal(L,"scriptCaller");
			lua_setglobal(L,"scriptParent");
			bool Ok=false;
			Ok= ( 0==luaL_loadstring(L,S) );
			if (Ok)
			{
				lua_getfield(L,LUA_REGISTRYINDEX,"server");// смена контекста
				lua_setfenv(L,-2);
			}

			if ( (!Ok)	||  0!=lua_pcall(L,0,0,0) )
			{
				conPrintI(lua_tostring(L,-1));
				conPrintI(S);
			}
		}
		lua_settop(L,T);
		scriptPushValue(Idx);
		return 1;

	}
	int mapGetNameL(lua_State*L)
	{
		lua_pushstring(L,mapGetName());
		return 1;
	}
	void wallBreakListUpdate(wallRec *P)
	{
		bool Add=(P->wallFlags&8);
		wallBreakable_s const *W=noxGetFirstBreakableList();
		for( ;W!=NULL;W=W->next)
		{
			if(W->wall==P)
			{
				if (Add)
				{
//					conPrintI("wall already in breaklist");	
					return;/// ”же есть
				}
				else
				{
//					conPrintI("wall removed from breaklist");	
					noxWallBreackableListRemove(W);
					return;
				}
			}
		}
		if (!Add)
		{
//			conPrintI("wall not in breaklist");	
			return; // » так нету
		}
//		conPrintI("wall appended to breaklist");	
		noxWallBreackableListAdd(P);
	}

	int getTileByName(const char* TileName)
	{
		char *mapFirstTile=0;
		ASSIGN(mapFirstTile,0x865C20);
		int i=0;
		while(i<74)
		{
			char *TileNameN = (i++ * 0x302C) + mapFirstTile;
			if (strcmpi(TileName,TileNameN)==0)
				return i-1;
		}
		return -1;
	}
	int getWallAtPtL(lua_State *L)
	{
		if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_rawgeti(L,1,1);
			lua_rawgeti(L,1,2);
			lua_remove(L,1);
			lua_insert(L,1);
			lua_insert(L,1);
		}
		if ((lua_type(L,1)!=LUA_TNUMBER)||(lua_type(L,2)!=LUA_TNUMBER))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int x=lua_tointeger(L,1),y=lua_tointeger(L,2);
		x=(x-x%23)/23;y=(y-y%23)/23;
			wallRec *P=noxGetWallAtPt(x,y);
			if(P==NULL)
			{
				lua_pushnil(L);
				return 1;
			}
			if ((lua_gettop(L)>2)&& lua_toboolean(L,3)) // тут
			{
			    lua_pushlightuserdata(L,P);
				return 1;
			}
			else
			{
				lua_pushinteger(L,P->tileName);
				lua_pushinteger(L,x);lua_pushinteger(L,y);
				lua_pushinteger(L,P->Dir);
				lua_pushinteger(L,P->wallFlags);
				lua_pushinteger(L,P->variation);
				lua_pushinteger(L,P->unk3);
				lua_pushinteger(L,P->HP);
				return 8;
			}
	}

 int setWallAtPtL(lua_State *L)
 {
  int tileName;
  if(lua_type(L,1)==LUA_TNUMBER)
   tileName=lua_tointeger(L,1);
  else if(lua_type(L,1)==LUA_TSTRING)
   tileName=getTileByName(lua_tostring(L,1));
  else
  {
   lua_pushstring(L,"wrong args!");
   lua_error_(L);
  }
  if ((lua_type(L,2)!=LUA_TNUMBER)||(lua_type(L,3)!=LUA_TNUMBER))
  {
   lua_pushstring(L,"wrong args!");
   lua_error_(L);
  }
  int x=lua_tointeger(L,2),y=lua_tointeger(L,3);
  x=(x-x%23)/23;y=(y-y%23)/23;

  //lua_settop(L,6);///tile,x,y,facing,flags,vari,[3],[7]
	if ((lua_tointeger(L,1)<0) || (tileName<0)) 
	{
		lua_pushstring(L,"wrong args!");
		lua_error_(L);
	}
	  wallRec *P=noxGetWallAtPt(x,y);
	  if(P==NULL)
	{
//		lua_pushstring(L,"there is no wall there!");
//		lua_error_(L);
		P=noxWallCreateAt(x,y);
	  }
	  P->Dir =lua_tointeger(L,4);
	  P->tileName=tileName;
	  P->variation=lua_tointeger(L,6);
	  P->unk3=lua_tointeger(L,7);
	  P->wallFlags=lua_tointeger(L,5);
	  P->HP=lua_tointeger(L,8);
	  if(P->HP==0)
		  P->HP=255;
	  if (P->wallFlags&8)
	  {
			P->wallId=(*wallNextBreakableId)++;
	  }
	  netWallCreate(NULL,P,1,P->tileName,P->Dir,P->variation);
	  BYTE Buf[256],*Out=Buf;
	  int Size=sizeof(*P)+2;
	  netUniPacket(upWallChanged,Out,Size);
	  memcpy(Out,P,sizeof(*P));
	  netSendAll(Buf,Size);
	  if (P==NULL)
		  lua_pushnil(L);
	  else
		lua_pushlightuserdata(L,P);
  return 1;
 }
	int mapDelWallAtPtL(lua_State *L)
	{
		if(lua_type(L,1)==LUA_TTABLE)
		{
			lua_rawgeti(L,1,1);
			lua_rawgeti(L,1,2);
			lua_remove(L,1);
			lua_insert(L,1);
			lua_insert(L,1);
		}
		if ((lua_type(L,1)!=LUA_TNUMBER)||(lua_type(L,2)!=LUA_TNUMBER))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int x=lua_tointeger(L,1),y=lua_tointeger(L,2);
		x=(x-x%23)/23;y=(y-y%23)/23;
		noxMapDelWallAtPt(x,y);
		//TODO: - хорошо бы обновить клиентский список разрушимых стен
		return 1;
	}
	/*
	x1,y1,x2,y2,flags->ret,x,y
	*/
	int mapTraceRayL(lua_State *L)
	{
		int i=1;
		if ( 
			(lua_type(L,i++)!=LUA_TNUMBER) ||
			(lua_type(L,i++)!=LUA_TNUMBER) ||
			(lua_type(L,i++)!=LUA_TNUMBER) ||
			(lua_type(L,i++)!=LUA_TNUMBER) 
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		i=1;
		noxRect T;
		T.X1=lua_tonumber(L,i++);
		T.Y1=lua_tonumber(L,i++);
		T.X2=lua_tonumber(L,i++);
		T.Y2=lua_tonumber(L,i++);
		int Flags=lua_tointeger(L,i++);
		struct {int A;int B;} Ret4;
		i=mapTraceRay(&T,(int)&Ret4,0,Flags);
		lua_pushboolean(L,i);
		lua_pushlightuserdata(L,(void*)Ret4.A);
		lua_pushlightuserdata(L,(void*)Ret4.B);
		return 3;
	}
	int mapGetTileByNameL(lua_State *L)
	{
		if ((lua_type(L,1)!=LUA_TSTRING) || (lua_gettop(L)!=1))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int Tile = getTileByName(lua_tostring(L,1));
		if (Tile<0)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		else
		{
			lua_pushinteger(L,Tile);
		}
		return 1;
	}

	int mapGetNameByTileL(lua_State *L)
	{
		if ((lua_type(L,1)!=LUA_TNUMBER) || (lua_gettop(L)!=1) || (lua_tointeger(L,1)>80))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		char *mapFirstTile=0;
		ASSIGN(mapFirstTile,0x00865C20);
		char* Tile = (lua_tointeger(L,1) * 0x302C) + mapFirstTile;
		lua_pushstring(L,Tile);
		return 1;
	}
	DWORD *mapDoInitialize;
	bool firstRun=true;
}
void __cdecl onLoadLevel() /// вызываетс€ при загрузке левела (сервером???)
{
	/// —ервером или клиентом?
	if(0==*mapDoInitialize)
		return;
	if (firstRun)
		firstRun=false;
	else
	{
		mapUnloadUtil();
		mapUnloadFilesystem();
	}

	mapInitialize();
	int Top=lua_gettop(L);
	do
	{
		getServerVar("onMapLoad");
		if (!lua_isfunction(L,-1))
			break;
		lua_pcall(L,0,0,0);
		lua_settop(L,Top);
	}
	while(0);
	mapLoadFilesystem(mapGetName());
	mapLoadSpells();
}
extern DWORD *GameFlags;
void netOnWallChanged(wallRec *newData) 
{
	char Buf[280];

	int x=newData->posX,y=newData->posY;
	sprintf(Buf,"wall at %d %d",x,y);
	conPrintI(Buf);	

	wallRec *P=noxGetWallAtPt(x,y);
	if (P==NULL)
	{
		P=noxWallCreateAt(x,y);
	}
	if (P==NULL)
		return;
	P->wallId=newData->wallId;
	P->wallFlags=newData->wallFlags;
	wallBreakListUpdate(P);
}
extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectAddr(DWORD Addr,void *Fn);
void mapInit()
{
	ASSIGN(scriptCallerUnit,0x00979720);
	ASSIGN(scriptTriggerUnit,0x00979724);

	ASSIGN(mapInitialize,0x004FC590);
	ASSIGN(mapDoInitialize,0x00753908);
	InjectOffs(0x004D2D31+1,onLoadLevel);

	ASSIGN(scriptPopValue,0x00507250);
	ASSIGN(scriptPushValue,0x00507230);
	ASSIGN(scriptKey,0x0097BB40);
	ASSIGN(mapWaypoint,0x00514800);
	InjectAddr(0x005C3244,myWaypoint);

	ASSIGN(noxGetWallAtPt,0x410580);
	ASSIGN(noxWallCreateAt,0x410250);
	ASSIGN(noxWallTileByName,0x410D60);
	ASSIGN(noxMapDelWallAtPt,0x410430);
	ASSIGN(mapTraceRay,0x535250);
	ASSIGN(netWallCreate,0x4FFE80);
	ASSIGN(mapGetName,0x409B40);
	
	ASSIGN(noxWallBreackableListAdd,0x00410840);
	ASSIGN(noxWallBreackableListRemove,0x00410890);
	ASSIGN(noxGetFirstBreakableList,0x00410870);
	
	ASSIGN(wallNextBreakableId,0x00689574);

	registerserver("mapGet",&getWallAtPtL);
	registerserver("mapSet",&setWallAtPtL);
	registerserver("mapDel",&mapDelWallAtPtL);
	registerserver("mapTraceRay",&mapTraceRayL);
	registerserver("mapGetTileByName",&mapGetTileByNameL);

	registerclient("tileGetName",&mapGetNameByTileL);
	registerclient("mapName",&mapGetNameL);
}

