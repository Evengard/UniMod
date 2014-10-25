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

		int env_index = lua_upvalueindex(1); // индекс, таблицы среды

		luaU_byukey(L, LUA_REGISTRYINDEX, &Timer::ptr_env_key);
			lua_pushlightuserdata(L, timer);
			lua_pushvalue(L, env_index); // пишем где искать таблицы для таймера
			lua_settable(L, -3);
		lua_pop(L, 1);

		luaU_byukey(L, env_index, &Timer::ptr_function_key); // достаём таблицу для функций таймера
			lua_pushlightuserdata(L, timer);
			lua_pushvalue(L, 1); // функция
			lua_settable(L, -3);
		lua_pop(L, 1); // выкидываем таблицу для таймеров

		luaU_byukey(L, env_index, &Timer::ptr_timer_key);
			lua_pushlightuserdata(L, timer);
			lua_pushvalue(L, -3); // юзердата таймера
			lua_settable(L, -3);
		lua_pop(L, 1);
		
		return 1;
	}
	static int timer_gc(lua_State* L)
	{
		Timer::Timer_instance* timer = (Timer::Timer_instance*)lua_touserdata(L, 1);
		Timer::erase_from_list(timer);
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
		lua_pushfstring(L, "Timer: %p\t delta: %d\t frame: %d", (void*)timer, timer->delta, timer->frame);
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
	int ptr_function_key = 0; // self key
	int ptr_timer_key = 0;
	int ptr_env_key = 0;

	int open_lib(lua_State *L)
	{
		
		lua_newtable(L); // таблица будет лежать в среде из котоорой исполняется команда
		luaU_makeukey(L, -2, &ptr_function_key); // делаем таблицу, для соотвествия указатель - функция
		
		luaU_newweaktable(L, "v");
		luaU_makeukey(L, -2, &ptr_timer_key); // делаем таблицу, для соотвествия указатель - таймер

		luaL_newmetatable(L, timer_meta); // создайм метатаблицу для таймеров
			lua_pushvalue(L, -1);
			lua_setfield(L, -2, "__index");
		luaU_insert_fn(L, timer_meta_fn);
		lua_pop(L, 1); // выкидываем метатаблицу

		lua_pushvalue(L, -1); // upvalue - 
		lua_pushcclosure(L, timer_new, 1); //регаем в таблицу (по идее эвентс) функцию для создания таймеров
		lua_setfield(L, -2, "timer");
		return 1;
	}

}//Timer namespace