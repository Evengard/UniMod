#include "stdafx.h"

// Include converted lua files
#include "Libs/luasocket/src/socket_main.h"
#include "Libs/json.h"
#include "lua/funcs.h"
#include "lua/require.h"

// Reference Lua C functions
extern "C" int luaopen_socket_core(lua_State *L);

// Handy functions
void registerLuaModule(const lua_CFunction cfunction, const char* modulename)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, cfunction);
	lua_setfield(L, -2, modulename);
}

void registerLuaModule(const unsigned char* buffer, size_t length, const char* modulename)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "package");
	lua_getfield(L, -1, "preload");
	int error = luaL_loadbuffer(L, (const char*)buffer, length, modulename);
	if (error != 0)
	{
		const char* errorMsg = lua_tostring(L, -1);
		MessageBoxA(NULL, errorMsg, "LUA load error! The module won't be loaded!", MB_OK);
		return;
	}
	lua_setfield(L, -2, modulename);
}

void runEmbeddedLuaFile(const unsigned char* buff, size_t length, const char* name)
{
	int err = luaL_loadbuffer(L, (const char*)buff, length, name);
	if (err == 0)
	{
		err = lua_pcall(L, 0, 0, 0);
		if (err != 0)
		{
			const char* errorMsg = lua_tostring(L, -1);
			MessageBoxA(NULL, errorMsg, "LUA load error! The file won't be loaded!", MB_OK);
			return;
		}
	}
	else
	{
		const char* errorMsg = lua_tostring(L, -1);
		MessageBoxA(NULL, errorMsg, "LUA load error! The file won't be loaded!", MB_OK);
		return;
	}
}

// Init addons
void luaAddonsLoad()
{
	// Init useful functions globally
	runEmbeddedLuaFile(funcs_lua, funcs_lua_len, "funcs.lua");

	// Load the patch of the require function so it sets the current environement correctly
	// -1
	int err = luaL_loadbuffer(L, (const char*)require_lua, require_lua_len, "require.lua");
	if (err != 0)
	{
		const char* errorMsg = lua_tostring(L, -1);
		MessageBoxA(NULL, errorMsg, "LUA load error! The file won't be loaded!", MB_OK);
		return;
	}

	lua_pushstring(L, "require.lua");
	lua_pushvalue(L, -2);
	lua_settable(L, LUA_REGISTRYINDEX);

	// Init json engine
	registerLuaModule(json_lua, json_lua_len, "json");

	// Init the LuaSocket
	registerLuaModule(luaopen_socket_core, "socket.core");
	registerLuaModule(socket_lua, socket_lua_len, "socket");
}