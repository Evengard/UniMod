#include <windows.h>
#include "lua.hpp"
#include "memory.h"
#pragma pack(1)
lua_State *globalL;

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
	write_array(bt, &OperatorJmps, sizeof(OperatorJmps));  // ��� ������� ��������
	write_array(bt2, &OperatorJmps, sizeof(OperatorJmps)); // ��� �� ������ 2 -�� ������
	write_array(bt3, OperatorMovEax1, sizeof(OperatorMovEax1)); // ������� ������
	write_array(bt4, OperatorNop, sizeof(OperatorNop)); // ����� ������������� ����� � ������������� �����.
	write_array(bt5, OperatorMovEax2, sizeof(OperatorMovEax2)); // NoCD 
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	switch (reason) {
		case DLL_PROCESS_ATTACH:
		{
			patch_nox();
			// ����
			globalL = luaL_newstate();

			if (luaL_dofile(globalL, "UniMod2.lua"))
				break;
			lua_getglobal(globalL, "prompt");
			if (!lua_isstring(globalL, -1))
			{
				lua_pop(globalL, 1);
				break;
			}
			const char *s = lua_tostring(globalL, -1);
			int size = strlen(s) + 1;
			wchar_t *ws = new wchar_t[size];
			
			mbstowcs(ws, s, size);

			MessageBox(
				NULL,
				ws,
				L"UniMod2",
				MB_OK
				);
			break;

			lua_pop(globalL, 1);
			delete ws;
		}
		case DLL_PROCESS_DETACH:
			lua_close(globalL);
			break;
	}
	return 1;
}