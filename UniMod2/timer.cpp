#include <list>
#include "unimod.h"
#include "memory.h"
#include "console.h"
#include "events.h"
#include "timer.h"

namespace {
	std::list<Timer::Timer_instance*> timer_list; // упорядоченный список фраймов для таймеров

	void __cdecl on_each_frame()
	{
		lua_State* L = unimod_State.L;

		lua_pushlightuserdata(L, Timer::add_to_list);
		lua_gettable(L, LUA_REGISTRYINDEX); // достаём таблицу для функций таймера

		unsigned __int32 current_time = Nox::frame_counter();
		for (std::list<Timer::Timer_instance*>::iterator iter = timer_list.begin(); iter != timer_list.end();)
		{
			if (current_time < (*iter)->frame)
				break;
			
			Timer::Timer_instance* timer = *iter;
			iter = timer_list.erase(iter); // для того чтобы его можно было ещё раз повесить, в том же кадре

			if (current_time == timer->frame)
			{
				lua_pushlightuserdata(L, timer);
				lua_gettable(L, -2); // достаём юзердату
				lua_pushvalue(L, -1); // дублируем, потом для вызова пригодиться
				lua_gettable(L, -2); // достаём функцию
				if (lua_isfunction(L, -1))
				{
					if (lua_pcall(L, 1, 0, 0))
					{
						Console::print(lua_tostring(L, -1), Console::Grey);
						lua_pop(L, 1);
					}
				}
				else
					lua_pop(L, 2); // функцию и юзердату
			}
			
		}
		lua_pop(L, 1); // таблицу таймеров

		static NOX_FN(void, sub51ADF0, 0x51adf0);
		sub51ADF0();
	}
}//anonymous namespace

namespace Timer {

	void add_to_list(Timer_instance* timer)
	{
		std::list<Timer_instance*>::iterator iter = timer_list.begin();
		for (; iter != timer_list.end();)
			if ((*iter)->frame > timer->frame)
				break;
		timer_list.insert(iter, timer);
	}
	void erase_from_list(Timer_instance* timer)
	{
		for (std::list<Timer_instance*>::iterator iter = timer_list.begin(); iter != timer_list.end(); ++iter)
			if (*iter == timer)
			{
				timer_list.erase(iter);
				return;
			}
	}
	bool is_in_list(Timer_instance* timer)
	{
		for (std::list<Timer_instance*>::iterator iter = timer_list.begin(); iter != timer_list.end(); ++iter)
			if (*iter == timer)
				return true;
		return false;
	}

	void init()
	{
		inject_offs(0x4D2AB4, on_each_frame);
	}

}//Timer namespace