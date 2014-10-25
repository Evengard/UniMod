#include "lua.hpp"
#include "unimod.h"
#include "console.h"
#include "memory.h"
#include "lua_unimod.h"
#include "config.h"

#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>

int Console::environment = -1;
Console::Color Console::current_color = Console::Grey;

NOX_FN(int, nox_console_print, 0x00450B90, int color, const wchar_t* text);

int Console::print(const std::string& s, Console::Color color)
{
	//std::wstring buffer(s.begin(), s.end()); // по другому надо
	int size = s.size()+1;
	std::vector<wchar_t> ws(size, 0); // хз насколько безопасно
	mbstowcs(&ws[0], s.c_str(), size);
	return nox_console_print(int(color), &ws[0]);
}
int Console::print(const std::wstring& s, Console::Color color)
{
	return nox_console_print(int(color), s.c_str());
}

namespace {
	bool is_variable(const char* s)
	{	// пременная начинается с буквы или _. Потом могут быть и цифры.
		// Так же она может состоять из нескольких кусков разделенных '.'
		if (!(*s))
			return false;

		if (std::strchr("_ABCDEFGHIJKLMNOPQRSTUVWXUZabcdefghijklmnopqrstuvwxyz", s[0]) == NULL)
			return false;

		for (unsigned int i = 1; s[i] ; ++i) 
			if (std::strchr("_ABCDEFGHIJKLMNOPQRSTUVWXUZabcdefghijklmnopqrstuvwxyz1234567890", s[i]) == NULL)
			{
				if (s[i] == '.')
					return is_variable(&s[i+1]);

				return false;
			}
		return true;
	}
	void get_var(lua_State* L, char* s)
	{ // возвращает значение переменной с подтаблицами
		int top = lua_gettop(L);
		char *token = std::strtok(s, ".");
		lua_rawgeti(L, LUA_REGISTRYINDEX, Console::environment);
		while (token != NULL)
		{
			char *next_token = std::strtok(NULL, ".");
			lua_getfield(L, -1, token);
			lua_remove(L, -2); // удаляем прошлое местоположение
			if (next_token == NULL)
				return;
			if (!lua_istable(L,-1))
			{
				lua_pop(L, 1);
				lua_pushnil(L);
				return;
			}
			token = next_token;
		}
		// return last value
	}


	void __cdecl console_on_cmd()
	{ // вызывается, когда неудался ноксПарсер

		const wchar_t* wcmd = *((wchar_t**)0x0069D728); // хз насколько безопасно
		int nsize = (wcslen(wcmd)+1) * 2;
		std::vector<char> vbuff(nsize, 0);
		wcstombs(&vbuff[0], wcmd, nsize);
		char *cmd = &vbuff[0];
	
		lua_State *L = unimod_State.L;
		int top = lua_gettop(L);

		if (is_variable(cmd))
		{
			get_var(L, cmd);
			luaU_tostring(L, -1);
			Console::print(lua_tostring(L, -1), Console::Grey);
			lua_settop(L, top);
			return;
		}

		if (luaL_loadstring(L, cmd)) // грузим в луа
		{
			Console::print(lua_tostring(L, -1), Console::Error);
			lua_settop(L, top);
			return;
		}

		lua_rawgeti(L, LUA_REGISTRYINDEX, Console::environment);
		lua_setfenv(L, -2);

		if (lua_pcall(L, 0, 0, 0)) // вызываем
		{
			Console::print(lua_tostring(L, -1), Console::Error);
			lua_settop(L, top);
			return;
		}

		lua_settop(L, top);
	}

	void __declspec(naked) check_token_size()
	{
		void (__cdecl *fn)(); // функция которая херит стек при  большом токене
		fn = (void (*)())0x00443BF0;
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

} // anonymous namespace

void Console::init()
{
	inject_offs(0x00443E16, console_on_cmd); // место, где принтиться сообщение о синтактической ошибке
	inject_jump(0x00443A71, check_token_size);// Фикс. Был вылет, если длина токена превышала (31+1) символ.

	lua_State *L = unimod_State.L; // таблица для консоли
	lua_pushinteger(L,1); // подефлоту таблица создается при ините игры
	Console::environment = luaL_ref(L, LUA_REGISTRYINDEX);

}