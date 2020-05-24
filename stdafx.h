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
	upSendPrintToCli,// сервер присылает консоли строку
	upVersionServerRq, //сервер запрашивает клиентскую версию юнимода
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

struct wallSecret_s
{
	int NextWallSecret_s;
	int X;
	int Y;
	int wallRec_Ptr;
	int timeToClose;
	BYTE timeToOpen;
	BYTE bitAuto;
	BYTE chetchickOpen;
	BYTE field_17;
	int ChetchickClose;
	int field_1c;
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

struct ServerData
{
	char mapName[8];
	char gap[1];
	char gameName[10];
	char gap_13[5];
	int field_18[5];
	DWORD weaponRestrictions;
	DWORD armorRestrictions;
	__int16 gameFlags;
	unsigned __int16 fragLimit;
	unsigned char timeLimitMB;
	char isNew;
};

#define ASSIGN(X,Y) *((DWORD*)&(X))=((DWORD)(Y));
DWORD uniCalcJump(DWORD From,void *To);

extern void printI(const char *S);

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

struct polygonAngle_s
{
	int numAngle;
	float posX;
	float posY;
	int isCreated;
};


struct polygon_s
{
	int filed_0;
	char Name[76];
	int polygonIdx;
	int field_54;
	int posX1;
	int posY1;
	int posX2;
	int posY2;
	int color;
	void *strucCoordinat;
	int scriptFunction;
	int field_74;
	int field_78;
	int field_7C;
	short kolvoUglov;
	short minimapGroup;
	int field_84;
	int field_88;
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
struct FxBuffer_t
{
	FxBuffer_t *Next;
	int Size;
	int FreeSize;
	int SelIdx;
	DWORD Data[];
	FxBuffer_t(int Sz):Next(0),Size(Sz){}
	~FxBuffer_t(){ if(Next) delete Next; }
	void getValues(int First,int Len); /// для запроса по сети
	void addItem(int Val){((int*)Data)[SelIdx++]=Val;}
	void addItemD(DWORD Val){Data[SelIdx++]=Val;}

	static FxBuffer_t *addBlock(int Size,int *Id); //начинаем добавлять элементы, возвращаем айди
	static bool delBlock(int Id); //удаляем блок
	static void drawBuffers();
};
extern const int *noxScreenX;
extern const int *noxScreenY;

#include <map>
typedef std::map <BYTE,netClientFn_s> ClientMap_s;
typedef std::map <BYTE,netServFn_s> ServerMap_s;


#define goto wtf юзать гоуту нельзя