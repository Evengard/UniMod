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

Nox::Server_flags *Nox::server_flags = (Nox::Server_flags*)0x005D53A4;
int Nox::check_server_flags(Nox::Server_flags f)
{
	return int(*Nox::server_flags) & int(f);
}