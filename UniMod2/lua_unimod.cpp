// aux fn
#include "lua.hpp"
#include "lua_unimod.h"

void luaU_tostring(lua_State *L, int idx)
{
  if (lua_getmetatable(L, idx))
  {
	  lua_getfield(L, -1, "__tostring");
	  if (lua_isfunction(L, -1))
	  {
		  lua_pushvalue(L, idx);
		  lua_pcall(L, 1, 1, 0);
		  lua_replace(L, -2); // remove metatable
		  return;
	  }
	  lua_pop(L, 2);
  }

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




