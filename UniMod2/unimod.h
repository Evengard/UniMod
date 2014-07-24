#pragma once
#include "lua.hpp"

// ��� �������� ����-�� (������ ASSIGN)
#define NOX_FN(TYPE, NAME, OFFSET, ...)		TYPE (__cdecl * NAME)(__VA_ARGS__) = (TYPE (*)(__VA_ARGS__)) OFFSET;

class Unimod_State
{
public:
	lua_State *L;
	bool debug_mode;
	Unimod_State();
	~Unimod_State();
};

extern Unimod_State unimod_State;