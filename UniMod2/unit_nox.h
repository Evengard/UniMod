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
		__int32 flags2; //  какието флаги 0x40 - сверкание
		__int32 field_18; //  у мячика максиамальная скорость
		__int32 unitXP;
		__int32 worth; //  стоимость товара
		__int32 netCode; //  unitN mb
		__int32 mapExtentId; //  похоже на номер объекта в карте
		__int32 globalID; //  Индекс - это отправляется в стэк скрипта
		__int32 field_30; //  такие же данные на клиенте в sprite_s+18h
		__int32 teamId;
		__int32 unitX;
		__int32 unitY;
		__int32 someX_2; //  возможно составляющая скорости
		__int32 someY_2;
		__int32 someX_1;
		__int32 someY_1;
		__int32 velX;
		__int32 velY;
		__int32 field_58; //  скорость X?
		__int32 field_5C; //  скорость Y?
		__int32 field_60;
		__int32 field_64;
		__int32 heightFromFloor;
		__int32 field_6C;
		__int32 field_70;
		__int32 field_74; //  some float,used by Fist
		__int32 field_78;
		__int16 unitLookAt;
		__int16 direction1;
		__int32 time_80; //  для выстрелов - когда запустили
		__int32 field_84;
		__int32 createTime; //  для выстрелов - при оценки частоты поиска целей
		__int32 updatedPlayers1; //  set by bits of GiantStruct
		__int32 field_90; //  set by bits of GiantStruct
		__int32 updatedPlayers3; //  возможно видимость - т.к. каждый
		__int32 unitTargetORupdatedPlayers2; //  также возможно состояние обновленности для плэеров?
		__int32 someX_3;
		__int32 someY_3;
		__int32 fallingX; //  Куда падает
		__int32 fallingY;
		__int32 moveClassMB; //  4 - учитываем радиус
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
		__int32 someX_5; //  возможно позиция на след. кадре
		__int32 someY_5;
		__int32 someX_7; //  возможно позиция на след. кадре 2
		__int32 someY_7;
		__int32 field_F8;
		__int32 field_FC;
		__int16 shortX;
		__int16 shortY;
		__int32 nextBlockStruct; //  указывает на следующую структуру +100 в блоке
		__int32 prevBlockStruct; //  offset (00000000)
		__int32 objectInBlock; //  указывает на сам объект в этом блоке
		char unkfield_110[68];
		__int32 buffFlagsMB;
		__int16 buffTimers[32];
		char buffSomeByte[32]; //  похоже спеллпауэр
		__int32 enchantFlags; //  устанавливаются енчаннтами                                
		Unit* next_unit;
		Unit* prev_unit; 
		__int32 nextDeletedThisFrameUnitMB;
		__int32 deleteTime;
		__int32 field_1CC;
		__int32 field_1D0;
		__int32 nextDecayPtr;
		__int32 field_1D8;
		Unit* nextUpdatableUnit;
		Unit* prevUpdatableUnit; //  почти уверен
		__int32 isInUpdatableList;
		char unitWeight;
		char field_1E9;
		__int16 unitCapacity; //  какой вес может поднять
		__int32 inventoryOwner; //  есть вариант что владелец инвентаря
		__int32 nextInventoryObj; //  offset (00000000)
		__int32 prevSome; //  используется турелями
		Unit* firstInventoryObj; //  mbInventory
		Unit* parent_unit; //  какой-то указатель на юнит - у выстрела - родитель
		Unit* next_slave; //  сюда юниту записывается прошлый раб его владельца
		Unit* first_slave; //  offset (00000000)
		__int32 field_208; //  указывает на некий юнит похоже
		__int32 lastDamageTypeMB;
		__int32 damageX; //  откуда дамак
		__int32 damageY;
		__int32 timeLastDamageMB_218;
		char decelTime;
		char accelTime; //  счетчик для плавности движения у величивается каждый кадр
		__int16 movement_21E;
		__int32 unitSpeed;
		__int32 unitAccelMB;
		__int32 field_228; //  speed Enchant modify this
		void* hpData; //  somePtr accel+decel use it, size 14h
		__int32 field_230[32]; //  used by anim
		__int32 unkFnPtr2B0; //  Init Fn?
		__int32 unkFn2B0DataPtr; //  модбоксы mb####
		__int32 collideFn; //  для фаербола это функия взрыва
		__int32 collideData; //  Для дыр в полу содержит описалово
		__int32 XFerFnPtr2C0; //  Xfer - функция
		__int32 pickupFnPtr; //  вызывается при подъеме этого предмета плеером или монстром
		__int32 dropFnPtr; //  offset (00000000)
		__int32 damageFnPtr; //  не знаю, зачем но есть
		__int32 soundDamageFnPtr; //  offset (00000000)
		__int32 dieFnPtr; //  offset (00000000)
		__int32 spawnAtDeath; //  for dieCreateObject things
		__int32 useFnPtr; //  при использовании
		__int32 useFnData; //  у палок там
		__int32 field_2E4;
		__int32 onUpdateFn; //  offset (00000000)
		void* unitController; //  offset (00000000)
		__int32 field_2F0;
		__int32 field_2F4; //  указатель на скрипт pickup?
		__int32 field_2F8;
		__int32 script_flags;
		__int32 script_n;
	};
	struct Unit_def { //именно дефс!
		__int16 type_id; //  thing type

		char field_2[2]; //  string(C)
		const char* name; 
		__int32 field_8; //  при создании задается в 0
		__int32 menuIcon; //  В старший байт записывается 2 при загрузке цветов
		__int32 field_10; //  задается в

		__int16 thing_type_copy;
		char field_16[2]; //  string(C)
		Unit::Class u_class;
		__int32 u_subclass;
		Unit::Flags flags; //  обычные флаги юнита
		char field_24[4]; //  string(C)
		__int32 materialBits;
		__int32 unitXP;
		__int32 worth; //  цена я так полагаю
		char field_34[4]; //  устанавливается
		__int32 unitMass; //  физическая
		__int32 extentType; //  0=NULL
		__int32 extentCircleRadius; //  mb diameter
		__int32 extentCircleSquare; //  квадрат радиуса
		__int32 extentBoxX;
		__int32 extentBoxY;
		char field_50[32]; //  string(C)
		__int32 sizeZ_1; //  похоже ширина круга
		__int32 sizeZ_2; //  похоже высота круга
		char unitWeight; //  для кармана
		char field_79; //  string(C)
		__int16 unitCapacity; //  вместимость кармана?
		__int32 unitSpeed;
		__int32 unitAccel;
		__int32 field_84; //  set to 0 in parseSpeed
		__int32 unitHPDataField22C; //  определяет - создавать-ли field22C
		__int32 collideFn; //  offset (00000000)
		void* collideDataPtr;
		__int32 collideDataSize; //  base 16
		__int32 damageFn; //  offset (00000000)
		__int32 damageSoundFnPtr2D0; //  offset (00000000)
		__int32 dieFnPtr;
		__int32 spawnAtDeath; //  для dieCreateObj - содержит указатель на юнитдеф2
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
		__int32 XFerFnPtr2C0; //  Xfer - функция
		__int32 CreatePtr; //  вызывается при создании объекта
		Unit_def* nextUnitDef;
	};
}