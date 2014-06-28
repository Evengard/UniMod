#include <windows.h>
#include "lua.hpp"
#pragma pack(1)
lua_State *globalL;


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	switch (reason) {
		case DLL_PROCESS_ATTACH:
		{
			// тест
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