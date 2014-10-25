#pragma once
#include <string>
#include "lua.hpp"

namespace Console {
	enum Color {
		Black = 1, Grey, Light_grey, White, Dark_red, Red, Light_red, Dark_green, Green, Light_green,
		Dark_blue, Blue, Light_blue, Dark_yellow, Yellow, Light_yellow, Error = Light_red };

	int print(const std::string& s, Console::Color color); // печатает в консоль
	int print(const std::wstring& s, Console::Color color); // печатает в консоль
	void init();

	int open_lib(lua_State *L); // luaopen

	extern int environment; // reference for LUA_REGISTRY
	extern Color current_color; // for print from lua
}