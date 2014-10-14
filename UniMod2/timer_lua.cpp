#include "unimod.h"
#include "lua_unimod.h"
#include "timer.h"
#include "lua.hpp"

namespace {
	const char* timer_meta = "UniMod.Timer";

	static int timer_new(lua_State* L)
	{
		luaL_checktype(L, 1, LUA_TFUNCTION);
		int delta = -1; // заведомо, этот такой таймер не сработает
		if (lua_gettop(L) > 1)
			delta = luaL_checkinteger(L, 2);

		Timer::Timer_instance* timer = (Timer::Timer_instance*)lua_newuserdata(L, sizeof(Timer::Timer_instance));
		luaL_getmetatable(L, timer_meta);
		lua_setmetatable(L, -2); // устанавливем метатаблицу для таймера

		timer->delta = delta; // инициализируем
		timer->frame = Nox::frame_counter() + delta;

		lua_pushlightuserdata(L, Timer::add_to_list);
		lua_gettable(L, LUA_REGISTRYINDEX); // достаём таблицу для функций таймера
			lua_pushvalue(L, -2); // юзердата таймера
			lua_pushvalue(L, 1); // функция
			lua_settable(L, -3);

			lua_pushlightuserdata(L, (void*)timer);
			lua_pushvalue(L, -3); // юзердата таймера
			lua_settable(L, -3);
		lua_pop(L, 1); // выкидываем таблицу для таймеров

		if (delta >= 0)
			Timer::add_to_list(timer);

		return 1;
	}
	static int timer_gc(lua_State* L)
	{
		Timer::Timer_instance* timer = (Timer::Timer_instance*)lua_touserdata(L, 1);
		Timer::erase_from_list(timer);

		lua_pushlightuserdata(L, Timer::add_to_list);
		lua_gettable(L, LUA_REGISTRYINDEX); // достаём таблицу для функций таймера
			lua_pushlightuserdata(L, (void*)timer);
			lua_pushnil(L);
			lua_settable(L, -3); // убираем юзердату по указателю
		lua_pop(L, 1);

		return 0;
	}
	static int timer_start(lua_State* L)
	{
		luaL_checkudata(L, 1, timer_meta);
		Timer::Timer_instance* timer = (Timer::Timer_instance*)lua_touserdata(L, 1);
		if (Timer::is_in_list(timer))
			luaL_error(L, "Error: timer is already started");
		if (timer->delta < 0)
			luaL_error(L, "Error: timer's delta < 0");
		timer->frame = Nox::frame_counter() + timer->delta;
		Timer::add_to_list(timer);
		return 0;
	}
	static int timer_restart(lua_State* L)
	{
		luaL_checkudata(L, 1, timer_meta);
		Timer::Timer_instance* timer = (Timer::Timer_instance*)lua_touserdata(L, 1);
		if (timer->delta < 0)
			luaL_error(L, "Error: timer's delta < 0");

		Timer::erase_from_list(timer);
		timer->frame = Nox::frame_counter() + timer->delta;
		Timer::add_to_list(timer);
		return 0;
	}
	static int timer_stop(lua_State* L)
	{
		luaL_checkudata(L, 1, timer_meta);
		Timer::Timer_instance* timer = (Timer::Timer_instance*)lua_touserdata(L, 1);
		Timer::erase_from_list(timer);
		return 0;
	}
	static int timer_tostring(lua_State* L)
	{
		luaL_checkudata(L, 1, timer_meta);
		Timer::Timer_instance* timer = (Timer::Timer_instance*)lua_touserdata(L, 1);
		lua_pushfstring(L, "Timer: %x\t delta: %i\t frame: %i", (int)timer, timer->delta, timer->frame);
		return 1;
	}


	const luaL_Reg timer_meta_fn[] = {
		{ "__gc",		timer_gc		},
		{ "__tostring",	timer_tostring	},
		{ "start",		timer_start		},
		{ "restart",	timer_restart	},
		{ "stop",		timer_stop		},
		{ NULL,			NULL			}
	};

}//anonymous namespace
namespace Timer {
	int open_lib(lua_State *L)
	{
		/*
			в регистре под указателем Timer::add_to_list храним слабую таблицу.
			В этой таблицу ключи - юзерадты таймеров
			Значения - функции, которые надо вызвать
			Плюс используем её для хранения юзердат по указателям
			Таблица должна быть слабой по ключу, потому что юзер сам должен забоиться о жизни таймера
			*/
		lua_pushlightuserdata(L, Timer::add_to_list);
		lua_newtable(L); // делаем таблицу, куда будем складывать таймеры
			lua_newtable(L); // метатблица, что бы сделать таблицу слабой на значения
				lua_pushstring(L, "k");
				lua_setfield(L, -2, "__mode"); 
			lua_setmetatable(L, -2);
		lua_settable(L, LUA_REGISTRYINDEX); // делаем таблицу, куда будем складывать таймеры


		luaL_newmetatable(L, timer_meta); // создайм метатаблицу для таймеров
			lua_pushvalue(L, -1);
			lua_setfield(L, -2, "__index");
		
		luaU_insert_fn(L, timer_meta_fn);
		lua_pop(L, 1); // выкидываем метатаблицу


		lua_pushcfunction(L, timer_new); //регаем в таблицу (по идее эвенты) функцию для создания таймеров
		lua_setfield(L, -1, "timer");
		return 1;
	}

}//Timer namespace