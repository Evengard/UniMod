#include "console.h"
#include "memory.h"
#include "unimod.h"
#include "map.h"
#include "file_system.h"

int Map::environment = -1;

const char* Map::current_map_name()
{
	return (char*)0x0085B420;
}
namespace {
	bool map_loaded = false;

	int __cdecl map_on_load()
	try
	{
		map_loaded = true;
		Console::print(L"Map load", Console::Yellow);

		lua_State *L = unimod_State.L;
		int ret_status = Fsystem::open_file(L, "server.lua");
		if (lua_isfunction(L, -1))
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, Console::environment);
			lua_setfenv(L, -2);
			int ret = lua_pcall(L, 0, 0, 0);
			if (ret != 0)
			{
				Console::print(lua_tostring(L, -1), Console::Light_red);
				lua_pop(L, 1);
			}
		}
		else if (ret_status == LUA_ERRSYNTAX)
		{
			Console::print(lua_tostring(L, -1), Console::Light_red);
			lua_pop(L, 1);
		}
		static NOX_FN(int, some_fn, 0x004DA7C0);
		return some_fn();
	}
	catch (Fsystem::Invalid& e)
	{
		Console::print(e.what, Console::Light_red);
		static NOX_FN(int, some_fn, 0x004DA7C0);
		return some_fn();
	}

	void __cdecl map_on_exit(int nox_arg)
	{
		if (map_loaded)
		{
			Console::print(L"Map exit", Console::Yellow);
			map_loaded = false;
		}

		static NOX_FN(void, some_nox_fn, 0x0040A4D0, int);
		some_nox_fn(nox_arg);
	}
} // anonymous namespace

void Map::init()
{
	inject_offs(0x004FC599, map_on_load); // сервер. Перед поиском скрипта инициализации
	inject_offs(0x004D12F9, map_on_exit); // сервер. Перед дестроем юнитов

	lua_State *L = unimod_State.L; 
	lua_pushnil(L); 
	Map::environment = luaL_ref(L, LUA_REGISTRYINDEX);
}