#include "stdafx.h"

extern void (__cdecl *noxUnitDelete) (void *Unit);
extern void (__cdecl *noxUnitSetOwner) (void *NewOwner,void *Owner);
extern void (__cdecl *noxDeleteObject)(void *Unit);

extern DWORD *frameCounter;

namespace
{
	void __declspec(naked) asmDeathBallBugGs() // ��� GreatSword
	{
		__asm
		{
			call noxUnitSetOwner
			add esp,0Ch
			cmp [ebx+4],02C2h
			jnz exitL
			mov eax,[ebx+2DCh] // ������ ��� ����� ���������� ��������
			mov ecx,frameCounter
			mov ecx,[ecx+0]
			test eax,eax // ���� � ��� ��� 0, �� ����� ����� � �������
			jnz l1
			obnul:
			mov [ebx+2DCh],ecx
			mov [ebx+2E0h],0
			jmp exitL
			l1: // ���� �� ���� �� ������� ��� � ���������
			sub ecx,eax
			cmp ecx,4
			ja obnul // ���� ������ ������ 4 ������
			mov ecx,[ebx+2E0h]
			inc ecx
			mov [ebx+2E0h],ecx
			cmp ecx,3
			jna exitL
			push ebx
			call noxDeleteObject //������� �������
			add esp,4

			exitL:
			push 004E1C92h
			ret
		}
	}

	void __declspec(naked) asmDeathBallBugSh() // ��� Shield
	{
		__asm
		{
			call noxUnitSetOwner
			add esp,0Ch
			cmp [ebx+4],02C2h
			jnz exitL
			mov eax,[ebx+2DCh] // ������ ��� ����� ���������� ��������
			mov ecx,frameCounter
			mov ecx,[ecx+0]
			test eax,eax // ���� � ��� ��� 0, �� ����� ����� � �������
			jnz l1
			obnul:
			mov [ebx+2DCh],ecx
			mov [ebx+2E0h],0
			jmp exitL
			l1: // ���� �� ���� �� ������� ��� � ���������
			sub ecx,eax
			cmp ecx,4
			ja obnul // ���� ������ ������ 4 ������
			mov ecx,[ebx+2E0h]
			inc ecx
			mov [ebx+2E0h],ecx
			cmp ecx,3
			jna exitL
			push ebx
			call noxDeleteObject //������� �������
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