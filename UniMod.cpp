// UniMod.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

lua_State *L=0;
extern void initModLib1(HMODULE hModule);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    if (ul_reason_for_call== DLL_PROCESS_ATTACH)
	{
		initModLib1(hModule);
		L=luaL_newstate();
	}
	else if ( ul_reason_for_call== DLL_PROCESS_ATTACH)
	{
		lua_close(L);
	}
	return TRUE;
}

