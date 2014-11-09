#include "lua.hpp"
#include "unimod.h"
#include "lua_unimod.h"
#include "memory.h"
#include "unit.h"
#include "unit_nox.h"

#include "console.h"

namespace {
	NOX_FN(Nox::Unit_def*, nox_unit_def_by_name, 0x004E3830, const char*);
	NOX_FN(Nox::Unit_def*, nox_unit_def_by_type, 0x004E3B70, int);
	NOX_FN(Nox::Unit*, nox_unit_create_by_def, 0x004E3470, Nox::Unit_def*);
	NOX_FN(void, nox_unit_create_at, 0x004DAA50, Nox::Unit* unit, Nox::Unit* parent, float x, float y);

	const char* unit_meta = "UniMod.Unit";
	const int* unit_def_count = (const int*)0x005B82A8;

	//-----------------------------------------------[[
	void __cdecl nox_on_unit_delete(Nox::Unit* unit)
	{
		lua_State* L = unimod_State;
		lua_rawgeti(L, LUA_REGISTRYINDEX, Unit::memory_table);
		lua_pushlightuserdata(L, unit);
		lua_rawget(L, -2);
		if (!lua_isnil(L, -1))
		{
			Unit::Unit_memory* unit_m = (Unit::Unit_memory*)lua_touserdata(L, -1);
			unit_m->nox_unit = NULL;

			lua_pushlightuserdata(L, unit);
			lua_pushnil(L);
			lua_rawset(L, -4);

			char buff[256];
			sprintf(buff, "0x%x", (int) unit);
			Console::print(buff, Console::Yellow);
		}
		lua_pop(L,2);


		NOX_FN(void, some_fn, 0x004EC4B0, Nox::Unit*);
		some_fn(unit);
	}
	bool unimod_new_unit(lua_State* L, Nox::Unit* unit) // оставляет на стеке userdata юнита
	{ // false - если пришло создавать, true - если уже был
		lua_rawgeti(L, LUA_REGISTRYINDEX, Unit::memory_table);
		lua_pushlightuserdata(L, unit);
		lua_rawget(L, -2);
		if (lua_isnil(L, -1))
		{
			lua_pop(L,1); // pop nil
			Unit::Unit_memory* u_unit = (Unit::Unit_memory*)lua_newuserdata(L, sizeof(Unit::Unit_memory));
			u_unit->nox_unit = unit;
			luaL_getmetatable(L, unit_meta);
			lua_setmetatable(L, -2); // устанавливем метатаблицу для таймера

			lua_pushlightuserdata(L, unit);
			lua_pushvalue(L, -2);
			lua_rawset(L, -4);

			lua_remove(L, -2); // memory_table
			return false;
		}
		lua_remove(L, -2);
		return true;
	}
	//-----------------------------------------------]]
	Unit::Unit_memory* get_unit_by_idx(lua_State* L, int idx)
	{
		luaL_checkudata(L, idx, unit_meta);
		Unit::Unit_memory* unit_m = (Unit::Unit_memory*)lua_touserdata(L, idx);
		if (unit_m->nox_unit == NULL)
			luaL_error(L, "Error! Unit already deleted!");
		return unit_m;
	}

	static int unit_get_type_L(lua_State* L)
	{
		Nox::Unit* unit = get_unit_by_idx(L, 1)->nox_unit;
		lua_pushinteger(L, unit->type_id);
		return 1;
	}
	static int unit_to_string_L(lua_State* L)
	{
		luaL_checkudata(L, 1, unit_meta);
		Unit::Unit_memory* unit_m = (Unit::Unit_memory*)lua_touserdata(L, 1);
		Nox::Unit* unit = unit_m->nox_unit;
		if (unit_m->nox_unit == NULL)
			lua_pushstring(L, "Unit already deleted!");
		else
		{
			int type_id = unit->type_id;
			Nox::Unit_def* unit_d = nox_unit_def_by_type(type_id);
			lua_pushfstring(L, "N_p: %p\tU_p: %p\tType_id: %d\tName %s", (void*)unit, (void*)unit_m, type_id, unit_d->name);
		}
		return 1;
	}
	//-----------------
	static int get_defs_count_L(lua_State* L)
	{
		lua_pushinteger(L, *unit_def_count);
		return 1;
	}
	static int type_by_name_L(lua_State* L)
	{
		const char* name = luaL_checkstring(L, 1);
		Nox::Unit_def* def = nox_unit_def_by_name(name);
		if (!def)
			luaL_error(L, "Invalid name: %s", name);

		lua_pushinteger(L, def->type_id);
		return 1;
	}
	static int name_by_type_L(lua_State* L)
	{
		int type = luaL_checkint(L, 1);
		if (type < 0 || type > *unit_def_count)
			luaL_error(L, "Invalid type id!");
		Nox::Unit_def* def = nox_unit_def_by_type(type);
		lua_pushstring(L, def->name);
		return 1;
	}

	static int unit_create_L(lua_State* L)
	{
		int type_id = luaL_checkint(L, 1);
		if (type_id < 0 || type_id > *unit_def_count)
			luaL_error(L, "Invalid type id!");
		float x = (float)luaL_checknumber(L, 2);
		float y = (float)luaL_checknumber(L, 3);

		Nox::Unit_def* def = nox_unit_def_by_type(type_id);
	
		if ((def->u_class & (Nox::Unit::Immobile | Nox::Unit::Player)) != 0)
			luaL_error(L, "Can't create this object!");

		Nox::Unit* unit = nox_unit_create_by_def(def);
		if (!unit)
			luaL_error(L, "Nox can't create this object!");

		nox_unit_create_at(unit, NULL, x, y);

		unimod_new_unit(L, unit); // leave userdata
		return 1;
	};

	const luaL_Reg unit_fn[] = {
		{"create", unit_create_L},
		{"defs_count", get_defs_count_L},
		{"gtbn", type_by_name_L},
		{"gnbt", name_by_type_L},
		{NULL,				NULL}
	};
	const luaL_Reg unit_meta_fn[] = {
		{"type", unit_get_type_L},
		{"__tostring", unit_to_string_L},
		{NULL,				NULL}
	};
}//anonymous namespace

namespace Unit {
	int memory_table = -1;
	int open_lib(lua_State* L)
	{
		luaU_register(L, "unit", unit_fn);

		if (luaL_newmetatable(L, unit_meta)) // создайм метатаблицу для .юнитов
		{
			lua_pushvalue(L, -1);
			lua_setfield(L, -2, "__index");
			luaU_insert_fn(L, unit_meta_fn);
		}
		lua_pop(L, 1); // выкидываем метатаблицу

		return 1;
	}
	void init()
	{
		inject_offs(0x004E5E86, nox_on_unit_delete);

		lua_State* L = unimod_State;
		lua_newtable(L); // эта таблица нужна, чтобы помнить всех юнитов, которые мы использовали в юнимоде
		Unit::memory_table = lua_ref(L, LUA_REGISTRYINDEX);
	}
}//Unit namespace