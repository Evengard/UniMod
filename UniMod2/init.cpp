#include <windows.h>
#include "lua.hpp"
#include "memory.h"
#include "unimod.h"
#include "console.h"
#include "config.h"
#include "map.h"
#include "lua_unimod.h"
#include "events.h"
#pragma pack(1)

namespace {
	void __cdecl init_game(int arg1, int arg2) // когда хостится игра, или идёт коннект
	{
		luaU_initlib(unimod_State);
		Console::print(L"Game inited", Console::Blue);
		luaU_crenvtable(unimod_State, Console::environment);

		static NOX_FN(int, console_parse, 0x00443C80, int, int);
		console_parse(arg1, arg2);
	}

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

		if (Config::check_flag(Config::fl_more_noxs))
		{
			write_array(bt2, &OperatorJmps, sizeof(OperatorJmps)); // это на запуск 2 -ух ноксов
			write_array(bt3, OperatorMovEax1, sizeof(OperatorMovEax1)); // убиваем мутекс
		}

		write_array(bt4, OperatorNop, sizeof(OperatorNop)); // чиним прожорливость Нокса и архитектурный изъян.
		write_array(bt5, OperatorMovEax2, sizeof(OperatorMovEax2)); // NoCD 

		inject_offs(0x00435E93, init_game); // перед первым парсингом команд
	}
} // anonymous namespace


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	switch (reason) {
		case DLL_PROCESS_ATTACH:
		{
			Config::init(); // прежде всего

			patch_nox();
			Console::init();
			Map::init();
			Events::init();
		}
		case DLL_PROCESS_DETACH:
			break;
	}
	return 1;
}