#include <windows.h>
#include "lua.hpp"
#include "memory.h"
#include "unimod.h"
#include "console.h"
#include "config.h"
#include "lua_unimod.h"
#pragma pack(1)

namespace {
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
} // anonymus namespace


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	switch (reason) {
		case DLL_PROCESS_ATTACH:
		{
			patch_nox();
			Console::init();
			Config::init();

			lua_State* L = unimod_State.L;

			lua_rawgeti(L, LUA_REGISTRYINDEX, Console::environment);
			luaL_openlibs(L);
	
			lua_rawgeti(L, LUA_REGISTRYINDEX, Config::environment); // смотрим наличие дебаг-мода
			lua_getfield(L, -1, "debug");
			int debug_mode = lua_toboolean(L, -1);
			lua_pop(L, 2);

			if (debug_mode)
			{
				lua_pushcfunction(L, luaopen_debug);
				lua_pushvalue(L, -2);
				lua_call(L, 1, 0);
			}
			lua_pop(L, 1);
		}
		case DLL_PROCESS_DETACH:
			break;
	}
	return 1;
}