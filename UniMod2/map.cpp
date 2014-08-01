#include "console.h"
#include "memory.h"
#include "unimod.h"
#include "map.h"


namespace {
	bool map_loaded = false;

	void __cdecl map_on_load(int nox_arg)
	{
		static NOX_FN(void, cursor_set_type, 0x00477610, int);
		cursor_set_type(nox_arg);

		if (Nox::check_server_flags(Nox::sf_is_server))
		{
			map_loaded = true;
			Console::print(L"Map load", Console::Yellow);
		}

	}
	void __cdecl map_on_exit(int nox_arg)
	{
		if (map_loaded && Nox::check_server_flags(Nox::sf_is_server))
		{
			Console::print(L"Map exit", Console::Yellow);
			map_loaded = false;
		}

		static NOX_FN(void, some_nox_fn, 0x004460A0, int);
		some_nox_fn(nox_arg);
	}
} // anonymus namespace

void Map::init()
{
	inject_offs(0x0043DED0, map_on_load); // и клиент и сервер
	inject_offs(0x00467E06, map_on_exit); // сервер и клиень

}