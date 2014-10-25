#pragma once
#include "lua.hpp"

void luaU_tostring(lua_State *L, int idx); // ������ ������ ������ ����������� ������ (��� tostring)
void luaU_insert_fn(lua_State *L, const luaL_Reg *fns); // ��������� � ������� �� ���� �� �� ���
void luaU_register(lua_State* L, const char* name, const luaL_Reg *fns); // ������ � ������� �� ����
void luaU_newweaktable(lua_State* L, const char* mode);
void luaU_makeukey(lua_State* L, int index, void* key); // � �������  index, ��� ������������� key ����� ��, ��� �� ����
void luaU_byukey(lua_State* L, int index, void* key); // �� ������� index, ������� �������� ��� key
void luaU_crenvtable(lua_State* L, int ref); // ������ � �������� ��� ref ������� � ��������� � ��� ����
void luaU_initlib(lua_State* L); // nothing leave on stack

