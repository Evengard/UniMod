#include <string>
#include "console.h"
#include "lua.hpp"
#include "lua_unimod.h"

namespace {
	static int luaU_print(lua_State *L) 
	{
	  int n = lua_gettop(L);  /* number of arguments */
	  int i;
	  std::string str;
	  for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, i);   /* value to print */
		luaU_tostring(L, -1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
		  return luaL_error(L, LUA_QL("tostring") " must return a string to "
							   LUA_QL("print"));
		if (i>1)	str += '\t';
		str += s;
		lua_pop(L, 1);  /* pop result */
	  }
	  str += '\n';
	  Console::print(str, Console::current_color);
	  return 0;
	}
	static int luaU_set_color(lua_State *L)
	{
		if (lua_gettop(L) == 0)
		{
			lua_pushinteger(L, int(Console::current_color));
			return 1;
		}
		int color = luaL_checkint(L, 1);
		Console::current_color = Console::Color(color);
		return 0;
	}

	const luaL_Reg console_fn[] = {
		{ "color", luaU_set_color },
		{ NULL, NULL }
	};
}

int Console::open_lib(lua_State *L)
{
	lua_pushcfunction(L, luaU_print);
	lua_setfield(L, -2, "print");
	luaU_register(L, "console", console_fn);
	return 1;
}
