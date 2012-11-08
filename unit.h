#pragma once
struct scriptEvent
{
	int falgs;
	int scriptN;
};
struct hpData
{
	__int16 hp;
	__int16 maxHP;
	__int16 maxHP2MB;
	char gap_6[10];
	int field_10;
};
struct creatureAction
{
	int actionType;
	int arg0;
	int arg1;
	int arg2;
	char gap_10[4];
	int field_14;
};

struct ucCreature
{
	char gap_0[4];
	int field_4;
	char field_8[475];
	char actionSpellMB;
	void *unitDefPtr;
	int spellPtr;
	char gap_1EC[4];
	int time_1F0;
	char gap_1F4[16];
	int time_204;
	char gap_208[24];
	char actionStackSize;
	char gap_221[3];
	int time_224;
	creatureAction actionStack[22];
	char field_438[116];
	int field_4AC;
	char unkfield_4B0[24];
	scriptEvent eventSome;
	scriptEvent eventProp3SeeEnemy;
	scriptEvent eventProp6;
	scriptEvent eventProp7Hurt;
	scriptEvent eventProp8;
	scriptEvent eventProp5Death;
	scriptEvent eventProp9Collision;
	scriptEvent eventProp10;
	scriptEvent eventProp11;
	scriptEvent eventProp13TargetLost;
	char field_518[136];
	int flags_5A0;
	char field_5A4[628];
	float armorMB;
	char field_81C[20];
	int field_830;
	int field_834;
	char dialogType_838;
	char field_839[71];
	int targetUnit;
	int unitControllerMorphTo;
	char gap_888[3];
	char field_88B;
	int field_88C;
	char field_890;
};
struct ucPlayer
{
	int field_0;
	__int16 mana;
	__int16 manaOld;
	__int16 maxMana;
	char gap_a[2];
	hpData hpDataCopy;
	char gap_20[44];
	__int16 hpMB;
	__int16 field_4E;
	char gap_50[8];
	char animState_58;
	char gap_59[15];
	int equipedWeapon;
	char gap_6c[8];
	int field_74[4];
	int harpoon_84;
	char gap_88[76];
	char castIdxMB;
	char gap_d5[3];
	int mbTarg;
	float cursorXcopy;
	float cursorYcopy;
	int armorMB;
	char gap_e8[40];
	int field_110;
	struct playerInfoStruct *playerInfo;
	int field_118;
	char gap_11c[4];
	int spellTargMB_120;
	int morphUnitController;
	char gap_128[4];
	int field_12C;
	int field_130;
	char gap_134[240];
	int unkfield_224;
};

struct bigUnitStruct
{
	int ScriptNameMB;
	__int16 thingType;
	char field_6;
	char gap_7[1];
	int Class;
	int SubClass;
	int flags;
	int field_14;
	int field_18;
	float unitXP;
	char gap_20[4];
	int netCode;
	int mapExtentId;
	int globalID;
	void *field_30;
	int teamId;
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
	float heightFromFloor;
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
	bigUnitStruct *unitDamaged;
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
	scriptEvent field_2FC;
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
enum unitFlags
{
	ufBelow = 1,
	ufNoUpdate = 2,
	ufActive = 4,
	ufAllowOverlap = 8,
	ufShort = 0x10,
	ufDestroyed = 0x20,
	ufNoCollide = 0x40,
	ufMissileHit = 0x80,
	ufEquipped = 0x100,
	ufAirborne = 0x4000,
	ufDead = 0x8000,
	ufShadow = 0x10000,
	ufRespawn = 0x80000,
	ufTransient = 0x400000,
	ufEnabled = 0x1000000,
	ufPending = 0x2000000,
	ufMarked = 0x80000000,
};

extern bigUnitStruct *(__cdecl* objectCreateByName)(char const *ObjName);
extern bigUnitStruct *(__cdecl *unitDamageFindParent) (bigUnitStruct *Unit); // кто источник урона
extern void (__cdecl *noxUnitSetOwner) (bigUnitStruct *NewOwner,bigUnitStruct *Owner);
extern void (__cdecl *noxUnitDelete) (bigUnitStruct *Unit);
extern void (__cdecl *noxDeleteObject)(bigUnitStruct *Unit);