#include "lua.hpp"
#include "unimod.h"

Unimod_State::Unimod_State()
	:L(luaL_newstate()), debug_mode(false)
{}
Unimod_State::~Unimod_State()
{
	lua_close(L);
}
Unimod_State unimod_State;

namespace {
	Nox::Server_flags* server_flags = (Nox::Server_flags*)0x005D53A4;
	unsigned __int32* nox_frame_counter = (unsigned __int32*)0x0084EA04;
}//anonymous namespace


namespace Nox {
	int check_server_flags(Nox::Server_flags f)
	{
		return int(*server_flags) & int(f);
	}
	unsigned __int32 frame_counter()
	{
		return *nox_frame_counter;
	}
}