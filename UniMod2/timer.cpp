#include <list>
#include "unimod.h"
#include "lua_unimod.h"
#include "memory.h"
#include "console.h"
#include "events.h"
#include "timer.h"

namespace {
	std::list<Timer::Timer_instance*> timer_list; // упорядоченный список фраймов для таймеров

	void __cdecl on_each_frame()
	{
		lua_State* L = unimod_State.L;
		unsigned __int32 current_time = Nox::frame_counter();
		for (std::list<Timer::Timer_instance*>::iterator iter = timer_list.begin(); iter != timer_list.end();)
		{
			if (current_time < (*iter)->frame)
				break;
			
			Timer::Timer_instance* timer = *iter;
			iter = timer_list.erase(iter); // для того чтобы его можно было ещё раз повесить, в том же кадре

			if (current_time == timer->frame)
			{
				luaU_byukey(L, LUA_REGISTRYINDEX, &Timer::ptr_env_key);
				luaU_byukey(L, -1, timer); // ptr_env env
				lua_remove(L, -2); // env
				if (!lua_istable(L, -1))
				{
					Console::print(L"Error: on_each_frame timer's env not defined", Console::Error);
					lua_pop(L, 1);
					continue;
				}

				luaU_byukey(L, -1, &Timer::ptr_function_key); // env ptr_function
				luaU_byukey(L, -2, &Timer::ptr_timer_key); // env ptr_function ptr_timer

				luaU_byukey(L, -2, timer); // env ptr_function ptr_timer function
				luaU_byukey(L, -2, timer); // env ptr_function ptr_timer function timer
				if (lua_isfunction(L, -2))
				{
					if (lua_pcall(L, 1, 0, 0)) // env ptr_function ptr_timer [error]
					{
						Console::print(lua_tostring(L, -1), Console::Error);
						lua_pop(L, 4);
					}
					lua_pop(L, 3);
				}
				else
					lua_pop(L, 5); // функцию и юзердату
			}
			
		}

		static NOX_FN(void, sub51ADF0, 0x51adf0);
		sub51ADF0();
	}
}//anonymous namespace

namespace Timer {

	void add_to_list(Timer_instance* timer)
	{
		std::list<Timer_instance*>::iterator iter = timer_list.begin();
		for (; iter != timer_list.end();++iter)
		{
			if ((*iter)->frame > timer->frame)
				break;
		}
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

		lua_State* L = unimod_State.L;
		luaU_newweaktable(L, "v");
		luaU_makeukey(L, LUA_REGISTRYINDEX, &ptr_env_key); // таблица, чтобы понять какой таймер, в какой среде
	}

}//Timer namespace