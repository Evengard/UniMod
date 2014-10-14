#pragma once
#include "lua.hpp"

namespace Events
{
	void init();
	int open_lib(lua_State *L);
}
