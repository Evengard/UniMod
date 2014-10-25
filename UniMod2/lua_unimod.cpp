// aux fn
#include "lua.hpp"
#include "lua_unimod.h"
#include "config.h"

#include "console.h"
#include "events.h"

namespace {
	char* lib_environment = "UniMod.Lib_Environment";

	static const luaL_Reg unimod_libs[] = {
	  {"console", Console::open_lib},
	  {"events", Events::open_lib},
	  {NULL, NULL}
	};
} // anonymous namespace
void luaU_initlib(lua_State* L) // nothing leave on stack
{ // открывает в специальное место стандартную библиотеку луа
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setfield(L, LUA_REGISTRYINDEX, lib_environment);

	luaL_openlibs(L);

	if (Config::check_flag(Config::fl_debug_mode))
	{
		lua_pushcfunction(L, luaopen_debug);
		lua_pushvalue(L, -2);
		lua_call(L, 1, 0);
	}

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index"); // self
	lua_pop(L, 1);
}
void luaU_crenvtable(lua_State* L, int ref)
{ // Создаёт енв таблицу, открыавет в неё Юнимод либу и наследует функции от стандартной либы
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_rawseti(L, LUA_REGISTRYINDEX, ref);

	lua_getfield(L, LUA_REGISTRYINDEX, lib_environment);
	lua_setmetatable(L, -2);
		
	const luaL_Reg *lib = unimod_libs;
	for (; lib->func; lib++) 
	{
		lua_pushcfunction(L, lib->func);
		lua_pushvalue(L, -2); // table for inserting
		lua_call(L, 1, 0);
	}

	lua_pop(L, 1);
}


void luaU_tostring(lua_State *L, int idx)
{
	lua_pushvalue(L, idx);
	if (luaL_callmeta(L, -1, "__tostring"))  /* is there a metafield? */
		return;
	else
		lua_pop(L,1);
	
  switch (lua_type(L, idx)) {
    case LUA_TNUMBER:
      lua_pushstring(L, lua_tostring(L, idx));
      return;
    case LUA_TSTRING:
      lua_pushvalue(L, idx);
      return;
	case LUA_TBOOLEAN:
      lua_pushstring(L, (lua_toboolean(L, idx) ? "true" : "false"));
      return;
    case LUA_TNIL:
      lua_pushliteral(L, "nil");
      return;
    default:
      lua_pushfstring(L, "%s: %p", luaL_typename(L, idx), lua_topointer(L, idx));
      return;
  }
}

void luaU_insert_fn(lua_State *L, const luaL_Reg *fns)
{ // вставляет в таблицу на топе фнки из fns
	if (!lua_istable(L, -1))
		luaL_error(L, "luaU_insert_fn: on top must be table");
	for (; fns->func; fns++)
	{
		lua_pushcfunction(L, fns->func);
		lua_setfield(L, -2, fns->name);
	}
}
void luaU_register(lua_State* L, const char* name, const luaL_Reg *fns)
{ // регает в таблицу на топе
	if (!lua_istable(L, -1))
		luaL_error(L, "luaU_register: on top must be table");
	lua_getfield(L, -1, name);
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1); // remove what we pick up
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, name);
	}
	luaU_insert_fn(L, fns); // инсертим фнки в таблицу, что лежит в поле name
	lua_pop(L, 1);
}
void luaU_newweaktable(lua_State* L, const char* mode)
{
	lua_newtable(L);
		lua_newtable(L);
		lua_pushstring(L, mode);
		lua_setfield(L, -2, "__mode");
	lua_setmetatable(L, -2);
}

void luaU_makeukey(lua_State* L, int index, void* key)
{
	int abs = index > 0 || index <= LUA_REGISTRYINDEX ? index : lua_gettop(L) + index + 1;
	lua_pushlightuserdata(L, key);
	lua_insert(L, -2); // меняем значение и ключ
	lua_settable(L, abs);
}
void luaU_byukey(lua_State* L, int index, void* key)
{
	int abs = index > 0 || index <= LUA_REGISTRYINDEX ? index : lua_gettop(L) + index + 1;
	lua_pushlightuserdata(L, key);
	lua_gettable(L, abs);
}

