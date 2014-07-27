#include <windows.h>
#include "lua.hpp"
#include "memory.h"
#include "unimod.h"
#include "console.h"
#pragma pack(1)

void patch_nox()
{
	BYTE OperatorJmps=0xEB;
	BYTE OperatorMovEax1[]={0xB8,0x01,0,0,0};
	BYTE OperatorNop[]={0x90,0x90};
	BYTE OperatorMovEax2[]={0xB8,0x02,0,0,0};
	DWORD bt=0x00553660;
	DWORD bt2=0x00401E06;
	DWORD bt3=0x00401114;
	DWORD bt4=0x0043E82B;
	DWORD bt5=0x0041353D;

	write_array(bt, &OperatorJmps, sizeof(OperatorJmps));  // это убираем серийник
	write_array(bt2, &OperatorJmps, sizeof(OperatorJmps)); // это на запуск 2 -ух ноксов
	write_array(bt3, OperatorMovEax1, sizeof(OperatorMovEax1)); // убиваем мутекс
	write_array(bt4, OperatorNop, sizeof(OperatorNop)); // чиним прожорливость Нокса и архитектурный изъян.
	write_array(bt5, OperatorMovEax2, sizeof(OperatorMovEax2)); // NoCD 
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	switch (reason) {
		case DLL_PROCESS_ATTACH:
		{
			patch_nox();
			Console::init();
		}
		case DLL_PROCESS_DETACH:
			break;
	}
	return 1;
}