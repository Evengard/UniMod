#include "lua.hpp"
#include "unimod.h"

Unimod_State::Unimod_State()
	:L(luaL_newstate()), debug_mode(true)
{}
Unimod_State::~Unimod_State()
{
	lua_close(L);
}
Unimod_State unimod_State;
