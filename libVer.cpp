#include "stdafx.h"

extern void injectCon();
extern "C" void __cdecl onNetPacket(BYTE *&BufStart,BYTE *E);
extern "C" void __cdecl onNetPacket2(BYTE *&BufStart,BYTE *E, BYTE *MyPlayer, BYTE *MyUc);
extern "C" int __cdecl  playerOnTrySpell(bigUnitStruct *Unit,byte *Uc,spellPacket *Pckt);
extern "C" void conSendToServer(const char *Cmd);

void* conAddr=NULL;

bool justDoNoxCmd=false;

bool conDelayed=true;

namespace
{
	void __declspec(naked) netOnCli()
	{
		__asm
		{
			jb b
		a:
			push 0x4942B5
			retn
		b:
			push ebp
			push ebx
			lea eax, [esp+8-4]
			push eax
			call onNetPacket
			add esp, 8
			pop ebp
			cmp ebp, ebx
			jnb a
			push 0x48EAA8
			retn
		}
	}

	void __declspec(naked) netOnServ()
	{
		__asm
		{
			jb b
		a:
			push 0x51CE4E
			retn
		b:
			mov ecx, [esp+2Ch]
			push esi
			push edi
			push ebp
			lea  eax, [esp+0Ch-4]
			push ecx
			push eax
			call onNetPacket2
			add esp, 10h
			pop esi
			cmp esi, [esp+2Ch]
			jnb a
			push 0x51BBA1
			retn
		}
	}

	void __declspec(naked) MyOnTryCastSpell()
	{
		__asm
		{
				push esi
				push edi
				push ebp
				call playerOnTrySpell
				add esp, 0Ch
				xor edi, edi
				cmp eax, 0
				jnz a
				push 0x51C121
				retn
			a:
				js b
				inc edi
			b:
				push 0x51C19C
				retn
		}
	}

	int delayedConL(lua_State *L)
	{
		int top;
		const char *errorstr;
		char* Dest;

		lua_rawgeti(L, 2, 1);
		if (lua_pcall(L, 0, 0, 0))
		{
			top = lua_gettop(L);
			errorstr = (const char *)lua_tolstring(L, top, 0);
			Dest = new char[strlen(errorstr)+10];
			strcpy(Dest, "Error: ");
			strncat(Dest, errorstr, strlen(errorstr));
			conPrintI(Dest);
		  }
		  return 0;
	}

	void __declspec(naked) FixMeBack()
	{
		int data[4];
		int bt[1];
		data[0]=0x40FF43;
		data[1]=0x40FEF6;
		data[2]=0x40FEEC;
		data[3]=0x40FF00;
		bt[0]=0x40FF48;
		DWORD OldProtect;
		VirtualProtect((byte*)bt[0],0x10,PAGE_EXECUTE_READWRITE,&OldProtect);
		memcpy((byte*)bt[0],data,0x10);
		VirtualProtect((byte*)bt[0],0x10,OldProtect,&OldProtect);
		__asm
		{
			retn;
		}
	}
	void __declspec(naked) InitMe()
	{
		FixMeBack();
		luaL_openlibs(L);
		lua_pushcclosure(L, delayedConL, 0);
		lua_setfield(L, LUA_REGISTRYINDEX, "delayedCon");
		injectCon();
		__asm
		{
			push 0x401C70;
			retn;
		}
	}

}






extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);
extern void InjectAddr(DWORD Addr,void *Fn);


void initModLib1(HMODULE hModule)
{
	void* MyModule = (void*)hModule;
	void* addr = (char*)InitMe + (int)MyModule - 0x36D0FF58;
	byte *bt=(byte*)(0x40FF54);
	DWORD OldProtect;
	VirtualProtect(bt,0x4,PAGE_EXECUTE_READWRITE,&OldProtect);
	memcpy((byte*)bt,&addr,0x4);
	VirtualProtect(bt,0x4,OldProtect,&OldProtect);
	InjectJumpTo(0x48EAA2, netOnCli);
	InjectJumpTo(0x494296, netOnCli);
	InjectJumpTo(0x51BB9B, netOnServ);
	InjectJumpTo(0x51CE48, netOnServ);
	InjectAddr(0x51CF08, MyOnTryCastSpell);
}

void __cdecl initModLib2()
{
	conAddr = (void*)0x443C86;
}

extern "C" int conDoCmd(char *Cmd,bool &PrintNil);
int conDoCmd(char *Cmd,bool &PrintNil)
{
	int gf = ~*GameFlags & 1;
	int top = lua_gettop(L);
	const char* mode;
	if(!gf)
		mode="server";
	else
		mode="client";
	lua_getfield(L, LUA_REGISTRYINDEX, mode);
	lua_getfield(L, LUA_GLOBALSINDEX, "conInput");
	if ( lua_type(L, -1) == LUA_TFUNCTION )
	{
		lua_pushstring(L, Cmd);
		int errcode = lua_pcall(L, 1, 1, 0);
		if (!errcode)
			errcode = lua_tointeger(L, -1);
		lua_settop(L, top);
		return errcode;
	}
	int statuscode = luaL_loadstring(L, Cmd);
	int errcode = statuscode;
	if(statuscode==3)
	{
		int cmdLength = strlen(Cmd);
		bool allMatched = false;
		char* cmdPointer;
		for(cmdPointer = Cmd; strchr("_ABCDEFGHIJKLMNOPQRSTUVWXUZabcdefghijklmnopqrstuvwxyz1234567890", *cmdPointer); cmdPointer++)
		{
			if(cmdPointer==(Cmd+cmdLength))
				break;
		}
		if(cmdPointer==(Cmd+cmdLength))
		{
			lua_getfield(L, LUA_GLOBALSINDEX, "tostring");
			lua_getfield(L, top + 1, Cmd);
			if(lua_type(L, -1))
			{
				if (lua_pcall(L, 1, 1, 0))
				{
					errcode = 1;
				}
				else
				{
					const char* returnString = lua_tolstring(L, -1, 0);
					char Dest[200];
					strcpy(Dest, " = ");
					strncat(&Dest[strlen(Dest)], returnString, (199-strlen(Dest)));
					Dest[199] = 0;
					conPrintI(Dest);
					errcode = 0;
				}
			}
			else
				PrintNil = true;
		}
	}
	else
	{
		if(!statuscode)
		{
			lua_pushvalue(L, top + 1);
			lua_setfenv(L, -2);
			if(conDelayed)
			{
				const char* timeoutFunc;
				if(!gf)
					timeoutFunc="setTimeout";
				else
					timeoutFunc="cliSetTimeout";
				lua_getfield(L, LUA_REGISTRYINDEX, timeoutFunc);
				lua_getfield(L, LUA_REGISTRYINDEX, "delayedCon");
				lua_pushinteger(L, 0);
				lua_createtable(L, 0, 0);
				lua_pushvalue(L, -5);
				lua_rawseti(L, -2, 1);
				errcode = lua_pcall(L, 3, 0, 0);
				if(errcode)
				{
					int newTop = lua_gettop(L);
					const char* errorStr = lua_tolstring(L, newTop, 0);
					char Dest[200];
					strcpy(Dest, "Adv error: ");
					strncat(&Dest[strlen(Dest)], errorStr, (199-strlen(Dest)));
					conPrintI(Dest);
				}
			}
			else
			{
				errcode=lua_pcall(L, 0, 0, 0);
				if(errcode)
				{
					int newTop = lua_gettop(L);
					const char* errorStr = lua_tolstring(L, newTop, 0);
					char Dest[200];
					strcpy(Dest, "error: ");
					strncat(&Dest[strlen(Dest)], errorStr, (199-strlen(Dest)));
					conPrintI(Dest);
				}
			}
		}
		lua_settop(L, top);
	}
	return errcode;
}

void onComCmd2();

DWORD __cdecl onConCmd(wchar_t *A,DWORD B) // TODO: TO REIMPLEMENT FROM ASM!!!
{
	int cmdResult = 0;
	int result;
	char Dest[0xC8];

	bool printNil=false;
	if (justDoNoxCmd)
	{
		cmdResult = 1; 
	}
	else
	{
		int length = wcslen(A);
		if ( length+1 >= 0xC8 )
			return 0;
		wcstombs(Dest, A, 0xC7);
		Dest[0xC7] = 0;
		if (*GameFlags) // Закладка выпилена
		{
			cmdResult = conDoCmd(Dest, printNil); // TODO: Implement conDoCmd
		}
		else
		{
			cmdResult = 1;
		}
	}
	if (cmdResult)
	{
		__asm
		{
			push B
			push A
			push onConCmd_GOBACK
			sub esp,0x88
			jmp conAddr
			onConCmd_GOBACK:
			add esp,8
			mov result,eax
		}
		if(printNil && !result && !justDoNoxCmd)
		{
			conPrintI(" = nil");
		}
		//result = ((int (__cdecl *)(unsigned __int32, unsigned __int32))conAddr)((unsigned __int32)A, B);
	}
	else
		result = 1;
	return result;
}