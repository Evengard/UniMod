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