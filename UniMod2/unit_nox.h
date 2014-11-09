#pragma once
namespace Nox {
	struct Unit {
		enum Class {
			Missile			= 1<<	0,
			Monster			= 1<<	1,
			Player			= 1<<	2,
			Obstacle		= 1<<	3,
			Short_t			= 1<<	4,
			Exit			= 1<<	5,
			Key				= 1<<	6,
			Door			= 1<<	7,
			InfoBook		= 1<<	8,
			Trigger			= 1<<	9,
			Hole			= 1<<	11,
			Wand			= 1<<	12,
			unitClass_2000	= 1<<	13,
			unitClass_4000	= 1<<	14,
			unitClass_8000	= 1<<	15,
			unitClass_10000	= 1<<	16,
			MonsterGen		= 1<<	17,
			unitClass_40000	= 1<<	18,
			Light			= 1<<	19,
			Simple			= 1<<	20,
			Complex			= 1<<	21,
			Immobile		= 1<<	22,
			VisibleEnable	= 1<<	23,
			Weapon			= 1<<	24,
			Armor			= 1<<	25,
			Flag			= 1<<	28,
			ClientPersist	= 1<<	29
		};
		enum Flags {
			Below			= 1<<	0,
			No_update		= 1<<	1,
			Active			= 1<<	2,
			Allow_overlap	= 1<<	3,
			Short			= 1<<	4,
			Destroyed		= 1<<	5,
			No_collide		= 1<<	6,
			Missile_hit		= 1<<	7,
			Equipped		= 1<<	8,
			Airborne		= 1<<	14,
			Dead			= 1<<	15,
			Shadow			= 1<<	16,
			Respawn			= 1<<	19,
			Transient		= 1<<	22,
			Enabled			= 1<<	24,
			Pending			= 1<<	25
		};
		char* ScriptNameMB; //  offset (00000000)
		__int16 thingType;
		__int16 field_6;
		Class u_class; //  enum unitClass
		__int32 SubClass;
		Flags flags; //  enum unitFlags
		__int32 flags2; //  ������� ����� 0x40 - ���������
		__int32 field_18; //  � ������ ������������� ��������
		__int32 unitXP;
		__int32 worth; //  ��������� ������
		__int32 netCode; //  unitN mb
		__int32 mapExtentId; //  ������ �� ����� ������� � �����
		__int32 globalID; //  ������ - ��� ������������ � ���� �������
		__int32 field_30; //  ����� �� ������ �� ������� � sprite_s+18h
		__int32 teamId;
		__int32 unitX;
		__int32 unitY;
		__int32 someX_2; //  �������� ������������ ��������
		__int32 someY_2;
		__int32 someX_1;
		__int32 someY_1;
		__int32 velX;
		__int32 velY;
		__int32 field_58; //  �������� X?
		__int32 field_5C; //  �������� Y?
		__int32 field_60;
		__int32 field_64;
		__int32 heightFromFloor;
		__int32 field_6C;
		__int32 field_70;
		__int32 field_74; //  some float,used by Fist
		__int32 field_78;
		__int16 unitLookAt;
		__int16 direction1;
		__int32 time_80; //  ��� ��������� - ����� ���������
		__int32 field_84;
		__int32 createTime; //  ��� ��������� - ��� ������ ������� ������ �����
		__int32 updatedPlayers1; //  set by bits of GiantStruct
		__int32 field_90; //  set by bits of GiantStruct
		__int32 updatedPlayers3; //  �������� ��������� - �.�. ������
		__int32 unitTargetORupdatedPlayers2; //  ����� �������� ��������� ������������� ��� �������?
		__int32 someX_3;
		__int32 someY_3;
		__int32 fallingX; //  ���� ������
		__int32 fallingY;
		__int32 moveClassMB; //  4 - ��������� ������
		__int32 unitRadius;
		__int32 unkfield_B4;
		__int32 field_B8; //  someX
		__int32 field_BC; //  someY
		__int32 field_C0; //  someFloat
		__int32 someY_4;
		__int32 someX_4;
		__int32 field_CC; //  some Float
		__int32 someX_6;
		__int32 field_D4; //  float...
		__int32 field_D8; //  float..
		__int32 someY_6;
		__int32 field_E0;
		__int32 field_E4;
		__int32 someX_5; //  �������� ������� �� ����. �����
		__int32 someY_5;
		__int32 someX_7; //  �������� ������� �� ����. ����� 2
		__int32 someY_7;
		__int32 field_F8;
		__int32 field_FC;
		__int16 shortX;
		__int16 shortY;
		__int32 nextBlockStruct; //  ��������� �� ��������� ��������� +100 � �����
		__int32 prevBlockStruct; //  offset (00000000)
		__int32 objectInBlock; //  ��������� �� ��� ������ � ���� �����
		char unkfield_110[68];
		__int32 buffFlagsMB;
		__int16 buffTimers[32];
		char buffSomeByte[32]; //  ������ ����������
		__int32 enchantFlags; //  ��������������� ����������                                
		Unit* next_unit;
		Unit* prev_unit; 
		__int32 nextDeletedThisFrameUnitMB;
		__int32 deleteTime;
		__int32 field_1CC;
		__int32 field_1D0;
		__int32 nextDecayPtr;
		__int32 field_1D8;
		Unit* nextUpdatableUnit;
		Unit* prevUpdatableUnit; //  ����� ������
		__int32 isInUpdatableList;
		char unitWeight;
		char field_1E9;
		__int16 unitCapacity; //  ����� ��� ����� �������
		__int32 inventoryOwner; //  ���� ������� ��� �������� ���������
		__int32 nextInventoryObj; //  offset (00000000)
		__int32 prevSome; //  ������������ ��������
		Unit* firstInventoryObj; //  mbInventory
		Unit* parent_unit; //  �����-�� ��������� �� ���� - � �������� - ��������
		Unit* next_slave; //  ���� ����� ������������ ������� ��� ��� ���������
		Unit* first_slave; //  offset (00000000)
		__int32 field_208; //  ��������� �� ����� ���� ������
		__int32 lastDamageTypeMB;
		__int32 damageX; //  ������ �����
		__int32 damageY;
		__int32 timeLastDamageMB_218;
		char decelTime;
		char accelTime; //  ������� ��� ��������� �������� � ������������ ������ ����
		__int16 movement_21E;
		__int32 unitSpeed;
		__int32 unitAccelMB;
		__int32 field_228; //  speed Enchant modify this
		void* hpData; //  somePtr accel+decel use it, size 14h
		__int32 field_230[32]; //  used by anim
		__int32 unkFnPtr2B0; //  Init Fn?
		__int32 unkFn2B0DataPtr; //  �������� mb####
		__int32 collideFn; //  ��� �������� ��� ������ ������
		__int32 collideData; //  ��� ��� � ���� �������� ���������
		__int32 XFerFnPtr2C0; //  Xfer - �������
		__int32 pickupFnPtr; //  ���������� ��� ������� ����� �������� ������� ��� ��������
		__int32 dropFnPtr; //  offset (00000000)
		__int32 damageFnPtr; //  �� ����, ����� �� ����
		__int32 soundDamageFnPtr; //  offset (00000000)
		__int32 dieFnPtr; //  offset (00000000)
		__int32 spawnAtDeath; //  for dieCreateObject things
		__int32 useFnPtr; //  ��� �������������
		__int32 useFnData; //  � ����� ���
		__int32 field_2E4;
		__int32 onUpdateFn; //  offset (00000000)
		void* unitController; //  offset (00000000)
		__int32 field_2F0;
		__int32 field_2F4; //  ��������� �� ������ pickup?
		__int32 field_2F8;
		__int32 script_flags;
		__int32 script_n;
	};
	struct Unit_def { //������ ����!
		__int16 type_id; //  thing type

		char field_2[2]; //  string(C)
		const char* name; 
		__int32 field_8; //  ��� �������� �������� � 0
		__int32 menuIcon; //  � ������� ���� ������������ 2 ��� �������� ������
		__int32 field_10; //  �������� �

		__int16 thing_type_copy;
		char field_16[2]; //  string(C)
		Unit::Class u_class;
		__int32 u_subclass;
		Unit::Flags flags; //  ������� ����� �����
		char field_24[4]; //  string(C)
		__int32 materialBits;
		__int32 unitXP;
		__int32 worth; //  ���� � ��� �������
		char field_34[4]; //  ���������������
		__int32 unitMass; //  ����������
		__int32 extentType; //  0=NULL
		__int32 extentCircleRadius; //  mb diameter
		__int32 extentCircleSquare; //  ������� �������
		__int32 extentBoxX;
		__int32 extentBoxY;
		char field_50[32]; //  string(C)
		__int32 sizeZ_1; //  ������ ������ �����
		__int32 sizeZ_2; //  ������ ������ �����
		char unitWeight; //  ��� �������
		char field_79; //  string(C)
		__int16 unitCapacity; //  ����������� �������?
		__int32 unitSpeed;
		__int32 unitAccel;
		__int32 field_84; //  set to 0 in parseSpeed
		__int32 unitHPDataField22C; //  ���������� - ���������-�� field22C
		__int32 collideFn; //  offset (00000000)
		void* collideDataPtr;
		__int32 collideDataSize; //  base 16
		__int32 damageFn; //  offset (00000000)
		__int32 damageSoundFnPtr2D0; //  offset (00000000)
		__int32 dieFnPtr;
		__int32 spawnAtDeath; //  ��� dieCreateObj - �������� ��������� �� �������2
		__int32 dropFn;
		__int32 initFnPtr2B0; //  offset (00000000)
		void* initFnDataPtr; //  offset (00000000)
		__int32 initFnDataSize;
		__int32 onPickupFn; //  offset (00000000)
		__int32 onUpdateFn; //  offset (00000000)
		__int32 unitControllerInitData; //  offset (00000000)
		__int32 unitControllerSize;
		__int32 useFnPtr;
		__int32 useFnDataInit;
		__int32 useFnDataSize;
		__int32 XFerFnPtr2C0; //  Xfer - �������
		__int32 CreatePtr; //  ���������� ��� �������� �������
		Unit_def* nextUnitDef;
	};
}