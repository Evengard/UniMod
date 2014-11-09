#include "lua.hpp"
#include "unimod.h"
#include "lua_unimod.h"
#include "unit.h"
#include "unit_nox.h"

namespace {
	NOX_FN(Nox::Unit_def*, unit_def_by_name, 0x004E3830, const char*);
	NOX_FN(Nox::Unit_def*, unit_def_by_type, 0x004E3B70, int);
	NOX_FN(Nox::Unit*, unit_create_by_def, 0x004E3470, Nox::Unit_def*);
	NOX_FN(void, unit_create_at, 0x004DAA50, Nox::Unit* unit, Nox::Unit* parent, float x, float y);

	const int* unit_def_count = (const int*)0x005B82A8;
	static int unit_create_L(lua_State* L)
	{
		int type_id = luaL_checkint(L, 1);
		if (type_id < 0 || type_id > *unit_def_count)
			luaL_error(L, "Invalid type id!");
		float x = (float)luaL_checknumber(L, 2);
		float y = (float)luaL_checknumber(L, 3);

		Nox::Unit_def* def = unit_def_by_type(type_id);
	
		if ((def->u_class & Nox::Unit::Immobile) != 0)
			luaL_error(L, "Can't create this object!");

		Nox::Unit* unit = unit_create_by_def(def);
		if (!unit)
			luaL_error(L, "Nox can't create this object!");

		unit_create_at(unit, NULL, x, y);

		lua_pushlightuserdata(L, unit);
		return 1;
	};
	const luaL_Reg unit_fn[] = {
		{"create", unit_create_L},
		{NULL,				NULL}
	};
}//anonymous namespace

namespace Unit {
	int open_lib(lua_State* L)
	{
		luaU_register(L, "unit", unit_fn);
		return 1;
	}
}//Unit namespace