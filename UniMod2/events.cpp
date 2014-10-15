#include "events.h"
#include "timer.h"

namespace Events {
	void init()
	{
		Timer::init();
	}
	int open_lib(lua_State *L)
	{
 		lua_newtable(L);
		Timer::open_lib(L);
		lua_setfield(L, -2, "events");
		return 1;
	}

}//Events namespace 