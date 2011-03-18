// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <string>

typedef unsigned char byte;
typedef unsigned short WORD;
#pragma pack(1)

extern "C"
{
#include "lauxlib.h"
#include "lualib.h"
}

extern lua_State *L;
typedef std::string TString;
typedef unsigned char byte;

enum uniPacket_e
{
	upVersionRq=1,
	upVersionResp,

	upWallChanged=5,
	upStaticCreated,
	upLuaRq,
	upLuaResp,
	upNewStatic,
	upDelStatic,
	upMoveStatic,
	upUpdateDef,	/// сервер хочет изменить свойства unitDef
	upTryUnitUse,	/// клиент хочет ИСПОЛЬЗОВАТЬ предмет
	upNotifyUnitUse,/// сервер сообщает, что кто-то использует предмет
	upSendArchive,  /// сервер сообщает, что вместе с картой придет еще и архив
	upChangeTile, /// сервер меняет тайл
	upSpellStart,/// сервер сообщает старт спелла
	upSpellSync, /// сервер сообщаем всем свежий список спелов
	upSendBubble,/// сервер хочет создать чат
};
// приписать к буферу наш заголовок для отправки
void netUniPacket(uniPacket_e Code,BYTE *&Data,int Size);
#define UNIPACKET_HEAD (3)

#define ABIL_TO_SPELL (0x89)

struct noxPoint
{
	float X;
	float Y;
	noxPoint(float x,float y):X(x),Y(y){}
};
struct noxRect
{
	float X1;
	float Y1;
	float X2;
	float Y2;
	noxRect():X1(0),Y1(0),X2(0),Y2(0){}
	noxRect(float x1,float y1, float x2,float y2):X1(x1),Y1(y1),X2(x2),Y2(y2){}
};
struct noxRectInt
{
	int X1;
	int Y1;
	int X2;
	int Y2;
	noxRectInt(int x1,int y1, int x2,int y2):X1(x1),Y1(y1),X2(x2),Y2(y2){}
};

struct wallRec
{
  BYTE Dir;
  BYTE tileName;
  BYTE variation;
  BYTE unk3;
  BYTE wallFlags;
  BYTE posX;
  BYTE posY;
  BYTE HP;
  BYTE field_8; /// ?
  BYTE gap_9[1];/// ?
  USHORT wallId;
  BYTE gap_c[4]; /// ?
  int wallMapPtr; /// указатель на сегмент карты - НЕ ТРОГАТЬ
  int nextWall; /// указатель на след стену - НЕ ТРОГАТЬ
  int mapYPtr; /// указатель на следующую вертикальную стену - НЕ ТРОГАТЬ
  int doorPtr; /// ? кажется здесь указатель на прилепленую дверь
  int field_20;
};

struct waypoint_s
{
	int Id;
	int flag;
	float X;
	float Y;
	char Name[256];

};

struct SpellTargetBlock
{
	void *target;
	float targX,targY;
};
typedef void * unitBigStructPtr;

struct creatureAction
{
  int actionTypeMB;
  int arg0;
  int arg1;
  int arg3;
  char gap_10[4];
  int field_14;
};

struct bigUnitStruct
{
  int ScriptNameMB;
  __int16 thingType;
  char field_6;
  char gap_7[1];
  int Class;
  int SubClass;
  int flags0;
  int field_14;
  int field_18;
  float unitXP;
  char gap_20[4];
  int netCode;
  int mapExtentId;
  int globalID;
  void *field_30;
  int flags2;
  int unitX;
  int unitY;
  int someX_2;
  int someY_2;
  int someX_1;
  float someY_1;
  int velX;
  int velY;
  float field_58;
  float field_5C;
  char gap_60[8];
  float field_68;
  float field_6C;
  char gap_70[4];
  float field_74;
  char gap_78[4];
  __int16 unitLookAt;
  __int16 direction1;
  int time_80;
  char gap_84[4];
  int createTime;
  int field_8C;
  int field_90;
  int field_94;
  int unitTargetMB;
  float someX_3;
  float someY_3;
  float fallingX;
  float fallingY;
  int moveClassMB;
  int unitRadius;
  float unkfield_B4;
  float field_B8;
  float field_BC;
  float field_C0;
  float someY_4;
  float someX_4;
  int field_CC;
  float someX_6;
  float field_D4;
  int field_D8;
  float someY_6;
  float field_E0;
  float field_E4;
  float someX_5;
  float someY_5;
  float someX_7;
  float someY_7;
  char gap_f8[8];
  __int16 shortX;
  __int16 shortY;
  void *nextBlockStruct;
  void *prevBlockStruct;
  void *objectInBlock;
  char unkfield_110[68];
  int buffMB;
  __int16 buffTimers[32];
  char buffSomeByte[32];
  char gap_1b8[4];
  void *nextUnit;
  bigUnitStruct *prevUnit;
  int nextDeletedThisFrameUnitMB;
  int deleteTime;
  char gap_1cc[8];
  int nextDecayPtrMB;
  char gap_1d8[4];
  int nextUpdatableUnit;
  int prevUpdatableUnit;
  int isInUpdatableList;
  char unitWeight;
  char gap_1e9[1];
  __int16 unitCapacity;
  void *prevInventoryObj;
  void *nextInventoryObj;
  void *prevSome;
  void *firstInventoryObj;
  void *parentUnit;
  void *nextSlave;
  void *firstSlave;
  int field_208;
  char gap_20c[4];
  int damageX;
  int damageY;
  int time_218;
  char decelTimeMB;
  char accelTimeMB;
  __int16 movement_21E;
  float unitSpeed;
  float unitAccelMB;
  int field_228;
  void *field_22C;
  int field_230[32];
  void *unkFnPtr2B0;
  void *unkFn2B0DataPtr;
  void *collideFn;
  void *collideDataMB;
  void *XFerFnPtr2C0;
  void *pickupFnPtr;
  void *dropFnPtr;
  void *damageFnPtr;
  void *soundDamageFnPtr;
  void *dieFnPtr;
  void *spawnAtDeath;
  void *useFnPtr;
  void *useFnData;
  char gap_2e4[4];
  void *onUpdateFn;
  void *unitController;
  char gap_2f0[4];
  int field_2F4;
  char gap_2f8[4];
//  scriptEvent_s field_2FC;
};


#define ASSIGN(X,Y) *((DWORD*)&(X))=((DWORD)(Y));
DWORD uniCalcJump(DWORD From,void *To);

extern void printI(const char *S);
extern bigUnitStruct* (__cdecl *playerFirstUnit)(); ///возвращает первый юнит сетевого игрока
extern bigUnitStruct* (__cdecl *playerNextUnit)(void* Prev); /// Возвращает следующего сетевого игрока

extern void conSetNextColor(int C); // устанавливает цвет след. сообщения консоли
extern void conPrintI(const char *S);
extern DWORD *GameFlags;

void netSendAll(void *Buf,int BufSize); /// посылает всем клиентам
void netSendNotOne(void *Buf,int BufSize,void *One); /// посылает всем клиентам  кроме одного
void netSendServ(void *Buf,int BufSize);/// посылает серверу

USHORT inline toShort(BYTE *X){return *((USHORT*)X);}
void inline fromShort(BYTE *&P,USHORT X){*((USHORT *)P)=X;P+=2;}

struct spellPacket
{
	byte Pckt;
	DWORD Spells[5];
	byte Dir;// 0= обычный 1= на себя
};
struct FloatRect
{
	float left,top,right,bottom;
	FloatRect(float l,float t,float r,float b):left(l),top(t),right(r),bottom(b){}
};
void lua_error_(lua_State *L);

struct ParseAttrib;
/// функция для доступа к данным архивов - в последствии через нее надо провести все
extern bool fsRead(const char *File,void *&Data, size_t &Size);

/// Функции выделения ноксо-памяти
extern void *(__cdecl *noxAlloc)(int Size);
extern void (__cdecl *noxFree)(void *Ptr);

/// Функция для регистрации пакетов клиенту и серверу
typedef void (*netClientFn_s) (BYTE *Packet);
typedef void (*netServFn_s) (BYTE *Packet,BYTE *MyPlayer,BYTE *MyUc);
extern void netRegClientPacket(uniPacket_e Event,netClientFn_s Fn);
extern void netRegServPacket(uniPacket_e Event,netServFn_s Fn);
extern void parseAttr(lua_State *L,int KeyIdx,int ValIdx,void *Struct,const ParseAttrib *Attrs);

inline void registerclient(const char *FnName,lua_CFunction Fn,int UpSize=0)
{
	lua_pushcclosure(L, Fn,UpSize);
	lua_getfield(L,LUA_REGISTRYINDEX,"client");
	lua_pushvalue(L,-2);
	lua_setfield(L,-2,FnName);
	lua_pop(L,2);// копию замыкания и таблицу
}
inline void registerserver(const char *FnName,lua_CFunction Fn,int UpSize=0)
{
	lua_pushcclosure(L, Fn,UpSize);
	lua_getfield(L,LUA_REGISTRYINDEX,"server");
	lua_pushvalue(L,-2);
	lua_setfield(L,-2,FnName);
	lua_pop(L,2);// копию замыкания и таблицу
}
inline void registerServerVar(const char *VarName)
{
	lua_getfield(L,LUA_REGISTRYINDEX,"server");
	lua_pushvalue(L,-2);
	lua_setfield(L,-2,VarName);
	lua_pop(L,2);// копию переменной и таблицу
}
inline void registerClientVar(const char *VarName)
{
	lua_getfield(L,LUA_REGISTRYINDEX,"client");
	lua_pushvalue(L,-2);
	lua_setfield(L,-2,VarName);
	lua_pop(L,2);// копию переменной и таблицу
}
inline void getServerVar(const char *VarName)
{
	lua_getfield(L,LUA_REGISTRYINDEX,"server");
	lua_getfield(L,-1,VarName);
	lua_remove(L,-2);
}
inline void getClientVar(const char *VarName)
{
	lua_getfield(L,LUA_REGISTRYINDEX,"client");
	lua_getfield(L,-1,VarName);
	lua_remove(L,-2);
}
struct sprite_s
{
	void *x;
};
struct keyState
{
  int Spell;
  int Direction;
};
struct keyPack
{
  keyState Spells[25];
  int selectedRow;
  keyState *keyRowPtr;
  void *someWnd_D0;
  void *spellWndItemMB[5];
  void *spellIconMB[5];
  int field_FC;
};

enum unitClass
{
  clMissile = 0x1,
  clMonster = 0x2,
  clPlayer = 0x4,
  clObstacle = 0x8,
  clShort = 0x10,
  clKey = 0x40,
  clDoor = 0x80,
  clInfoBook = 0x100,
  clTrigger = 0x200,
  clWand = 0x1000,
  clMonsterGen = 0x20000,
  clLight = 0x80000,
  clSimple = 0x100000,
  clComplex = 0x200000,
  clImmobile = 0x400000,
  clVisibleEnable = 0x800000,
  clWeapon = 0x1000000,
  clArmor = 0x2000000,
  clFlag = 0x10000000,
  clClientPersist = 0x20000000,
  clPickup = 0x80000000,
};
enum spellFlags
{
	sfNeedFly        = 4,
	sfRemoveShieldEtc  = 0x20,
	sfCreature=0x0
};

struct tileDef_s
{
	char Name[32];
	void *ImgPtr;
	DWORD Flags;
	DWORD doDamage;
	short sizeMB;
	short param2E;
	DWORD colorMB;
	byte tileSizeX;
	byte tileSizeY;
	char More[6];
};

class SendBuffer /// Предназначен для передачи по сети больших кусков
{
public:
	SendBuffer();
	
	byte *RecvComplete(size_t &Len);// Вернет не NULL если приехали данные
	
	void Write(byte *Data,size_t &Len); // отправит буфер по кускам
	void WriteLuaVar(lua_State *L,bool Float=false);
};
struct ParseAttrib /// для всяких парсеров всяких структур
{
	const char *name;
	int ofs;
	int type;
	/*
	1 int
	+2 string
	+3 color
	+4 wstring
	5 bitfield (A=TablePtr)
	*/
	int argA;
	void *argB;
};
#define goto wtf юзать гоуту нельзя