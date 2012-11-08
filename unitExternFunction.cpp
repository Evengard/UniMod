#include "stdafx.h"
#include "unit.h"

bigUnitStruct*	(__cdecl* objectCreateByName)(char const *ObjName);
bigUnitStruct *(__cdecl *unitDamageFindParent) (bigUnitStruct *Unit); // кто источник урона
void (__cdecl *noxUnitSetOwner) (bigUnitStruct *NewOwner,bigUnitStruct *Owner);
void (__cdecl *noxUnitDelete) (bigUnitStruct *Unit);
void (__cdecl *noxDeleteObject)(bigUnitStruct *Unit);



extern void unitInit();
extern void unit2Init();
void unitFunctionInit()
{
	ASSIGN(noxUnitSetOwner,0x004EC290);
	ASSIGN(unitDamageFindParent,0x004EC580);
	ASSIGN(noxUnitDelete,0x004E5E80);
	ASSIGN(noxDeleteObject,0x004E5CC0);
	ASSIGN(objectCreateByName,0x004E3810);

	unitInit();
	unit2Init();
}
