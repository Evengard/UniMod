#include "lua.hpp"
#include "unimod.h"
#include "console.h"
#include "memory.h"
#include "stdlib.h"
#include <string>
#include <vector>

int (__cdecl *nox_console_print)(int color,const wchar_t* text);

void print_to_console(const std::string& s, int color)
{
	//std::wstring buffer(s.begin(), s.end()); // по другому надо
	int size = s.size()+1;
	std::vector<wchar_t> ws(size, 0); // хз насколько безопасно
	mbstowcs(&ws[0], s.c_str(), size);
	nox_console_print(color, &ws[0]);
}


void __cdecl console_on_cmd()
{ // вызываетс€, когда неудалс€ ноксѕарсер

	const wchar_t* wcmd = *((wchar_t**)0x0069D728); // хз насколько безопасно
	int nsize = (wcslen(wcmd)+1) * 2;
	std::vector<char> vbuff(nsize, 0);
	wcstombs(&vbuff[0], wcmd, nsize);
	const char *cmd = &vbuff[0];
	

	lua_State *L = unimod_State.L;
	int top = lua_gettop(L);

	bool is_variable = true;
	if (strchr("_ABCDEFGHIJKLMNOPQRSTUVWXUZabcdefghijklmnopqrstuvwxyz", cmd[0]) == NULL) // провер€ем, не отдельна€ ли это переменна€
		is_variable = false;
	else
	{
		for (unsigned int i = 1; cmd[i] ; ++i) // все остальные еЄ символы
			if (strchr("_ABCDEFGHIJKLMNOPQRSTUVWXUZabcdefghijklmnopqrstuvwxyz1234567890", cmd[i]) == NULL)
			{
				is_variable = false;
				break;
			}
	}
	if (is_variable)
	{
		lua_getglobal(L, "tostring");
		lua_getglobal(L, cmd);
		lua_pcall(L, 1, 1, 0);
		print_to_console(lua_tostring(L, -1), 2);
		lua_settop(L, top);
		return;
	}


	if (luaL_loadstring(L, cmd)) // грузим в луа
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

void __declspec(naked) check_token_size()
{
	void (__cdecl *fn)(); // функци€ котора€ херит стек при  большом токене
	ASSIGN(fn, 0x00443BF0);
	__asm {
		push eax // птр на токен
		call wcslen
		add esp, 4
		cmp eax, 31 
		jg l_exit // если длина(токена) > 31 -> выходим
		call fn
		push  0x00443A76
		retn

	l_exit:
		add esp, 8 // чистим стек от fn
		mov eax, 0 // неудача
		push 0x00443BDA
		retn
	}
}

void console_init()
{
	ASSIGN(nox_console_print, 0x00450B90);
	inject_offs(0x00443E16, console_on_cmd); // место, где принтитьс€ сообщение о синтактической ошибке
	inject_jump(0x00443A71, check_token_size);// ‘икс. Ѕыл вылет, если длина токена превышала (31+1) символ.
}