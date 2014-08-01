#include "lua.hpp"
#include "unimod.h"
#include "config.h"

int Config::environment = -1;

void Config::init()
{
	lua_State *L = unimod_State.L;
	
	lua_newtable(L);
	Config::environment = luaL_ref(L, LUA_REGISTRYINDEX);

	if (luaL_loadfile(L, "UniMod.lua") == 0)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, Config::environment);
		lua_setfenv(L, -2);
		lua_pcall(L, 0, 0, 0);
	}
	else 
		lua_pop(L, 1); // error msg
}

int Config::check_flag(lua_State *L, Config::Flags flag) // ��������� �����
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, Config::environment);
	switch (flag)
	{
		case Config::fl_debug_mode:
			lua_getfield(L, -1, "debug");
			break;
		case Config::fl_more_noxs:
			lua_getfield(L, -1, "more_noxs");
			break;
		default:
			lua_pushboolean(L, int(false));
			break;
	}
	int res = lua_toboolean(L, -1);
	lua_pop(L, 2); // table and bool
	
	return res;
}