#include "lua.hpp"
#include "unimod.h"
#include "console.h"
#include "memory.h"
#include "stdlib.h"
#include <string>

int (__cdecl *nox_console_print)(int color,const wchar_t *text);
void print_to_console(const std::string& s, int color)
{
	std::wstring buffer(s.begin(), s.end()); // по другому надо
	nox_console_print(color, buffer.c_str());
}


void __cdecl console_on_cmd()
{ // вызываетс€, когда неудалс€ ноксѕарсер
	const std::wstring console_buffer(*((wchar_t**)0x0069D728));
	std::string cmd(console_buffer.begin(), console_buffer.end()); // странный конверт, но вроде работает

	lua_State *L = unimod_State.L;
	int top = lua_gettop(L);

	bool is_variable = true;
	for (unsigned int i = 0; i < cmd.size(); ++i) // провер€ем, не отдельна€ ли это переменна€
		if (strchr("_ABCDEFGHIJKLMNOPQRSTUVWXUZabcdefghijklmnopqrstuvwxyz1234567890", cmd[i]) == NULL)
		{
			is_variable = false;
			break;
		}
	if (is_variable)
	{
		lua_getglobal(L, "tostring");
		lua_getglobal(L, cmd.c_str());
		lua_pcall(L, 1, 1, 0);
		print_to_console(lua_tostring(L, -1), 2);
		lua_settop(L, top);
		return;
	}


	if (luaL_loadstring(L, cmd.c_str())) // грузим в луа
	{
		print_to_console(lua_tostring(L, -1), 2);
		lua_settop(L, top);
		return;
	}
	if (lua_pcall(L, 0, 0, 0)) // вызываем
	{
		print_to_console(lua_tostring(L, -1), 2);
		lua_settop(L, top);
		return;
	}
	lua_settop(L, top);

}

void console_init()
{
	ASSIGN(nox_console_print, 0x00450B90);
	inject_offs(0x00443E16, console_on_cmd); // место, где принтитьс€ сообщение о синтактической ошибке
}