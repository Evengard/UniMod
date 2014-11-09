#pragma once
#include "lua.hpp"
#include "unit_nox.h"

namespace Unit {
	struct Unit_memory {
		Nox::Unit* nox_unit;
	};
	int open_lib(lua_State *L); // luaopen
	void init();

	extern int memory_table;
}