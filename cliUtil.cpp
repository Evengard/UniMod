#include "stdafx.h"

int (__cdecl *getTTByNameSpriteMB)(void *Key);


namespace
{

	int clientTimeoutL()
	{
		luaL_loadstring(L,"print('a')");
		if (lua_pcall(L,0,0,0))
		{
			const char *err=lua_tostring(L,-1);
			lua_pop(L,1);
		}
		return 0;
	}

	void __declspec(naked) asmToCliTimer() // вызываем тик 
	{
		__asm
		{
			call getTTByNameSpriteMB
			call clientTimeoutL
			push 0x47582C 
			ret
		}
	}


}

extern void InjectJumpTo(DWORD Addr,void *Fn);

void cliUntilInit()
{
	ASSIGN(getTTByNameSpriteMB,0x044CFC0);


	InjectJumpTo(0x475827,&asmToCliTimer);

}