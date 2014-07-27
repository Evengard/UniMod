#pragma once
#include "lua.hpp"

void luaU_tostring(lua_State *L, int idx); // ������ ������ ������ ����������� ������ (��� tostring)
void luaU_insert_fn(lua_State *L, const luaL_Reg *fns); // ��������� � ������� �� ���� �� �� ���
void luaU_register(lua_State* L, const char* name, const luaL_Reg *fns); // ������ � ������� �� ����