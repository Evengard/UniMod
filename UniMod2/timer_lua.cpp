#include "unimod.h"
#include "lua_unimod.h"
#include "timer.h"
#include "lua.hpp"

namespace {
	const char* timer_meta = "UniMod.Timer";

	static int timer_new(lua_State* L)
	{
		luaL_checktype(L, 1, LUA_TFUNCTION);
		int delta = -1; // ��������, ���� ����� ������ �� ���������
		if (lua_gettop(L) > 1)
			delta = luaL_checkinteger(L, 2);

		Timer::Timer_instance* timer = (Timer::Timer_instance*)lua_newuserdata(L, sizeof(Timer::Timer_instance));
		luaL_getmetatable(L, timer_meta);
		lua_setmetatable(L, -2); // ������������ ����������� ��� �������

		timer->delta = delta; // ��������������
		timer->frame = Nox::frame_counter() + delta;

		lua_rawgeti(L, LUA_REGISTRYINDEX, Timer::timer_function_table); // ������ ������� ��� ������� �������
			lua_pushvalue(L, -2); // �������� �������
			lua_pushvalue(L, 1); // �������
			lua_settable(L, -3);
		lua_pop(L, 1); // ���������� ������� ��� ��������

		lua_rawgeti(L, LUA_REGISTRYINDEX, Timer::ptr_timer_table);
			lua_pushlightuserdata(L, (void*)timer);
			lua_pushvalue(L, -3); // �������� �������
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
	int timer_tostring(lua_State* L)
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
	int timer_function_table = -1;
	int ptr_timer_table = -1;

	int open_lib(lua_State *L)
	{
		/*
			� �������� ��� ���������� Timer::add_to_list ������ ������ �������.
			� ���� ������� ����� - �������� ��������
			�������� - �������, ������� ���� �������
			������� ������ ���� ������ �� �����, ������ ��� ���� ��� ������ ��������� � ����� �������
			*/
		luaU_newweaktable(L, "k");
		timer_function_table = luaL_ref(L, LUA_REGISTRYINDEX); // ������ �������, ��� ����������� ������ - �������

		luaU_newweaktable(L, "v");
		ptr_timer_table = luaL_ref(L, LUA_REGISTRYINDEX); // ������ �������, ��� ����������� ��������� - ������

		luaL_newmetatable(L, timer_meta); // ������� ����������� ��� ��������
			lua_pushvalue(L, -1);
			lua_setfield(L, -2, "__index");
		
		luaU_insert_fn(L, timer_meta_fn);
		lua_pop(L, 1); // ���������� �����������


		lua_pushcfunction(L, timer_new); //������ � ������� (�� ���� ������) ������� ��� �������� ��������
		lua_setfield(L, -2, "timer");
		return 1;
	}

}//Timer namespace