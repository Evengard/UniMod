#pragma once
#include "lua.hpp"

void luaU_tostring(lua_State *L, int idx); // кладет наверх строку описывающую объект (аля tostring)
void luaU_insert_fn(lua_State *L, const luaL_Reg *fns); // вставляет в таблицу на топе фн из фнс
void luaU_register(lua_State* L, const char* name, const luaL_Reg *fns); // регает в таблицу на топе
void luaU_newweaktable(lua_State* L, const char* mode);
void luaU_makeukey(lua_State* L, int index, void* key); // в таблице  index, под лайтюзердатой key пишет то, что на топе
void luaU_byukey(lua_State* L, int index, void* key); // из таблице index, достайт значение под key
void luaU_crenvtable(lua_State* L, int ref); // создаёт в регистре под ref таблицу и открывает в ней либы
void luaU_initlib(lua_State* L); // nothing leave on stack

