#include "unit.h"

struct EquipedItem
{
	int itemFlags;
	int modifierDescs[4];
	int field_14;
	char field_18;
};

struct playerInfoStruct
{
	int armorItemFlags;
	int weaponItemFlags;
	char field_8[2048];
	void *playerUnitPtr;
	int netCode;
	byte playerIdx;
	char field_811[3];
	int field_814;
	char field_818[24];
	char field_830[16];
	char gameSerial[8];
	char field_848[16];
	int field_858;
	int field_85C;
	int field_860;
	char field_864[37];
	char playerNameOwnW[24];
	char field_8A1[26];
	int stat_8BB;
	int statStrength;
	int statMaxMana;
	int statMaxHP;
	char playerClass;
	char MaleOrFamale;
	char colorR1;
	char colorG1;
	char colorB1;
	char colorR2;
	char colorG2;
	char colorB2;
	char colorR3;
	char colorG3;
	char colorB3;
	char colorR4;
	char colorG4;
	char colorB4;
	char colorR5;
	char colorG5;
	char colorB5;
	char colorR6;
	char colorG6;
	char colorB6;
	char gap_8DF[3];
	char nameNumberBuf;
	char gap_8E3[7];
	__int16 buffsMB;
	int cursorX;
	int cursorY;
	int color2;
	int color1;
	int color3;
	int color4;
	int color5;
	float material;
	char gap_90C[8];
	EquipedItem equipedWeapons[27];
	char field_BB7[597];
	int joinTime;
	char gap_E10[3];
	char unkfield_E13[25];
	int unit_E2C;
	int cameraX;
	int cameraY;
	__int16 unknown_E38;
	char field_E3A[18];
	int playerPolygonIdx;
	char gap_E50[4];
	int playerPolygonMiniMapGroup;
	char gap_E58[8];
	int field_E60;
	char statLvl[12];
	int spellLevels[136];
	char field_1090[344];
	int protectionCodeHP;
	int protectionCode_3_pi874;
	float protectionCodeMaxHP_MB;
	int protectionCodeMP;
	int protectionCodeMaxMP;
	int protectionCodeXP;
	int protectionCode_FloatucPlayer78;
	int protectionCodeBuffs;
	int protectionCodeClass;
	int protectionCode_ucPlayer8BB;
	int protectionCode_Strength;
	int protectionCode_Name;
	int protectionCode_1;
	int protectionCodeSpells;
	int protectionCode_4;
	int protectionCode_Level;
	char field_1228[56];
	char playerNameIngameW[24];
};


extern bigUnitStruct* (__cdecl *playerFirstUnit)(); ///возвращает первый юнит сетевого игрока
extern bigUnitStruct* (__cdecl *playerNextUnit)(void* Prev); /// Возвращает следующего сетевого игрока

extern bigUnitStruct* getPlayerUDataFromPlayerInfo(playerInfoStruct *addr);
extern bigUnitStruct* getPlayerUDataFromPlayerIdx(int idx);
extern bigUnitStruct* getPlayerUDataFromPlayerNetCode(int netCode);
extern playerInfoStruct *(__cdecl *playerGetDataFromIndex)(int index);