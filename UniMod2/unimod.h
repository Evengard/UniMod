#pragma once
#include "lua.hpp"

// для екстерна нокс-фн (вместо ASSIGN)
#define NOX_FN(TYPE, NAME, OFFSET, ...)		TYPE (__cdecl * NAME)(__VA_ARGS__) = (TYPE (*)(__VA_ARGS__)) OFFSET

class Unimod_State
{
public:
	lua_State *L;
	bool debug_mode;
	Unimod_State();
	~Unimod_State();

	operator lua_State*() const { return L; }
};

extern Unimod_State unimod_State;

namespace Nox {
	enum Server_flags {
		sf_is_server = 0x1
	};
	int check_server_flags(Nox::Server_flags);
	unsigned __int32 frame_counter();
}
