#include "stdafx.h"

void (__cdecl *sub_43B4D0) (); //для хоста 
void (__cdecl *parseGamedataBinPre) ();
int (__cdecl *noxCheckGameFlags) (int);
void (__cdecl *noxSetGameFlags) (int);
void (__cdecl *noxClearGameFlags) (int);

void (__cdecl *sub_461440)(int);
void (__cdecl *sub_4D6F40)(int);
void (__cdecl *sub_4D6F90)(int);
void (__cdecl *sub_4D6F60)(int);
void (__cdecl *sub_4D6F80)(int);
void (__cdecl *sub_473670)();
void (__cdecl *sub_472520)(int);
void (__cdecl *sub_43AF50)(int);

void *dword_748260;

int *dword_69B55C=(int*)0x0069B55C;
int *noxGameFlags;

extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);

namespace
{
	void __declspec(naked) asmHostWhenStart() // хостим сервер когда стартует игра
	{
		__asm
		{
			mov eax,dword_69B55C
			mov [eax+0],1
			push ecx
			mov ecx,noxGameFlags
			mov eax,[ecx+0]
			and eax,0FFFFFFDFh
			or eax,10h
			mov [ecx],eax
			pop ecx
			push 2000h
			call noxSetGameFlags
			push 10000h
			call noxSetGameFlags
			push 800h
		//	call noxClearGameFlags
		//	push 1000000h
			call noxClearGameFlags
			add esp,0Ch
			push 0
			call sub_461440
			push 0
			call sub_4D6F90
			push 0
			call sub_4D6F60
			push 0
			call sub_4D6F80
			call sub_473670
			push 8FCh
			call sub_472520
			push 0
			call sub_43AF50
			add esp,18h
			call parseGamedataBinPre
			call sub_43B4D0
			mov eax,1
			push 004AB14Ch
			ret
		}
	}

	void __cdecl checkAutoSrvFlag(char *str)
	{
		if (strcmp(str,"-autosrv")==0)
		{
			InjectJumpTo(0x004AB147,&asmHostWhenStart);
			byte OperatorJmps=0xEB;
			byte *bt=(byte*)(0x004AB105);
			DWORD OldProtect;
			VirtualProtect(bt,1,PAGE_EXECUTE_READWRITE,&OldProtect);
			memcpy((byte*)bt,&OperatorJmps,1); // убираем заставочки
			VirtualProtect(bt,1,OldProtect,&OldProtect);
		}
		if (strcmp(str, "-debug") == 0)
		{
			*noxGameFlags = *noxGameFlags | 0x1000000;
		}
	}

	void __declspec(naked) asmCheckAutoSrvFlag() // флаг проверяем
	{
		__asm
		{
			push ebp
			call checkAutoSrvFlag
			add esp,4
			sbb eax,eax
			sbb eax,0FFFFFFFFh
			push 004012B8h
			ret
		}
	}



}


void autoServer()
{

	ASSIGN(sub_43B4D0,0x43B4D0);

	ASSIGN(sub_461440,0x00461440);
	ASSIGN(sub_4D6F40,0x4D6F40);
	ASSIGN(sub_4D6F90,0x4D6F90);
	ASSIGN(sub_4D6F60,0x4D6F60);
	ASSIGN(sub_4D6F80,0x4D6F80);
	ASSIGN(sub_473670,0x473670);
	ASSIGN(sub_472520,0x472520);
	ASSIGN(sub_43AF50,0x0043AF50); // влияет на видимость игры

	ASSIGN(dword_748260,0x00748260);

	ASSIGN(parseGamedataBinPre,0x004D1630);
	
	ASSIGN(noxGameFlags,0x0085B7A0);
	ASSIGN(noxCheckGameFlags,0x0040A5C0);
	ASSIGN(noxSetGameFlags,0x0040A4D0);
	ASSIGN(noxClearGameFlags,0x0040A540);


	InjectJumpTo(0x004012B3,&asmCheckAutoSrvFlag);

	
}