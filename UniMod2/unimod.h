#pragma once
#include "lua.hpp"

#define ASSIGN(X,Y) *((int*)&(X))=((int)(Y));

class Unimod_State
{
public:
	lua_State *L;
	bool debug_mode;
	Unimod_State();
	~Unimod_State();
};

extern Unimod_State unimod_State;