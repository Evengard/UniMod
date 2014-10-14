#pragma once
#include "lua.hpp"

namespace Timer {
	struct Timer_instance {
		unsigned __int32 frame; // когда запуск
		int delta; // через сколько времени запускаем
	};

	void init();
	int open_lib(lua_State *L);

	bool is_in_list(Timer_instance* timer);
	void add_to_list(Timer_instance* timer);
	void erase_from_list(Timer_instance* timer);
}