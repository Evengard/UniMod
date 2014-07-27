/*
** $Id: linit.c,v 1.14.1.1 2007/12/27 13:02:25 roberto Exp $
** Initialization of libraries for lua.c
** See Copyright Notice in lua.h
*/


#define linit_c
#define LUA_LIB

#include "lua.hpp"


static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  //{LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};


LUALIB_API void luaL_openlibs (lua_State *L) 
{ // инсертит стандартные библиотеки в таблицу на верху стека
	if (!lua_istable(L, -1))
		luaL_error(L, "luaL_openlibs: on top must be table");
	const luaL_Reg *lib = lualibs;
	for (; lib->func; lib++) 
	{
		lua_pushcfunction(L, lib->func);
		lua_pushvalue(L, -2); // table for inserting
		lua_call(L, 1, 0);
	}
}

