#pragma once
#include "lua.hpp"

void luaU_tostring(lua_State *L, int idx); // кладет наверх строку описывающую объект (аля tostring)
void luaU_insert_fn(lua_State *L, const luaL_Reg *fns); // вставляет в таблицу на топе фн из фнс
void luaU_register(lua_State* L, const char* name, const luaL_Reg *fns); // регает в таблицу на топе