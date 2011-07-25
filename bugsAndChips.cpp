#include "stdafx.h"

extern void (__cdecl *noxUnitDelete) (void *Unit);
extern void (__cdecl *noxUnitSetOwner) (void *NewOwner,void *Owner);
extern void (__cdecl *noxDeleteObject)(void *Unit);

extern DWORD *frameCounter;

namespace
{
	void __declspec(naked) asmDeathBallBugGs() // для GreatSword
	{
		__asm
		{
			call noxUnitSetOwner
			add esp,0Ch
			cmp [ebx+4],02C2h
			jnz exitL
			mov eax,[ebx+2DCh] // теперь это время последнего коллайда
			mov ecx,frameCounter
			mov ecx,[ecx+0]
			test eax,eax // если у нас там 0, то пишем время и выходим
			jnz l1
			obnul:
			mov [ebx+2DCh],ecx
			mov [ebx+2E0h],0
			jmp exitL
			l1: // если не ноль то смотрим что и действуем
			sub ecx,eax
			cmp ecx,4
			ja obnul // если прошло больше 4 кадров
			mov ecx,[ebx+2E0h]
			inc ecx
			mov [ebx+2E0h],ecx
			cmp ecx,3
			jna exitL
			push ebx
			call noxDeleteObject //удаляем наконец
			add esp,4

			exitL:
			push 004E1C92h
			ret
		}
	}

	void __declspec(naked) asmDeathBallBugSh() // для Shield
	{
		__asm
		{
			call noxUnitSetOwner
			add esp,0Ch
			cmp [ebx+4],02C2h
			jnz exitL
			mov eax,[ebx+2DCh] // теперь это время последнего коллайда
			mov ecx,frameCounter
			mov ecx,[ecx+0]
			test eax,eax // если у нас там 0, то пишем время и выходим
			jnz l1
			obnul:
			mov [ebx+2DCh],ecx
			mov [ebx+2E0h],0
			jmp exitL
			l1: // если не ноль то смотрим что и действуем
			sub ecx,eax
			cmp ecx,4
			ja obnul // если прошло больше 4 кадров
			mov ecx,[ebx+2E0h]
			inc ecx
			mov [ebx+2E0h],ecx
			cmp ecx,3
			jna exitL
			push ebx
			call noxDeleteObject //удаляем наконец
			add esp,4

			exitL:
			push 004E1BDEh
			ret
		}
	}


}


extern void InjectJumpTo(DWORD Addr,void *Fn);
void bugsInit()
{
	InjectJumpTo(0x004E1C8A,&asmDeathBallBugGs);
	InjectJumpTo(0x004E1BD6,&asmDeathBallBugSh);


}