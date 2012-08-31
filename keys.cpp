#include "stdafx.h"

#define ASSIGN(X,Y) *((DWORD*)&X)=(DWORD)Y;


/// Просто чтобы знать
void (__cdecl *keyNextSpell) ();
void (__cdecl *keyPrevSpell) ();
void (__cdecl *keyResetSpell) ();

int (__cdecl *sub_4160F0)(int A,int B);/// B - небольшой интервал 
void (__cdecl *sub_4160D0)(int A);/// непонятного действия
void (__cdecl *wnd_452D80)(int A,int B);/// A - код рисунка? B - почти всегда 100
DWORD (__cdecl *get_57AF20)();

void (__cdecl *drawSpellIcons) (keyPack *Keys);
void (__cdecl *keyUpdateRow) (int N);
void (__cdecl *clientStoreLastButton) (int N);

DWORD *dw_6D4918,*dw_6D492C;

keyPack *noxKeyPack;

extern void InjectJumpTo(DWORD Addr,void *Fn);

namespace 
{
	void __cdecl myKeyNextSpell()
	{
		if ((*dw_6D4918)!=0 || (*dw_6D492C)!=0 )return;
		if (0!=get_57AF20()) return;
		lua_getglobal(L,"keyOnNext");
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,1);return;
		}
		if(0!=lua_pcall(L,0,0,0))
			lua_pop(L,1);
	}
	void __cdecl myKeyPrevSpell()
	{
		if ((*dw_6D4918)!=0 || (*dw_6D492C)!=0 )return;
		if (0!=get_57AF20()) return;
		lua_getglobal(L,"keyOnPrev");
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,1);return;
		}
		if(0!=lua_pcall(L,0,0,0))
			lua_pop(L,1);
	}
	int keyRowSelectL(lua_State*L)
	{
		if(lua_isnil(L,1))
		{
			lua_pushinteger(L,noxKeyPack->selectedRow);
			return 1;
		}
		int r=lua_tointeger(L,1)-1;
		if(r<0) r=0;
		if(r>4) r=4;		
		clientStoreLastButton(-1);
		keyUpdateRow(noxKeyPack->selectedRow=r);
		lua_pushinteger(L,r+1);
		return 1;
	}
	int keyGetRowL(lua_State*L)
	{
		int r=lua_tointeger(L,1)-1;
		if((r<0)||(r>4))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_newtable(L);
		for(int i=0;i<5;i++)
		{
			lua_pushinteger(L,noxKeyPack->Spells[r*5 + i ].Spell);
			lua_rawseti(L,-2,i+1);
		}
		return 1;
	}
	int keySetRowL(lua_State*L)
	{
		if (
			(lua_type(L,1)!=LUA_TTABLE)||
			(lua_type(L,2)!=LUA_TNUMBER)		
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int r=lua_tointeger(L,2)-1;
		if((r<0)||(r>4))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushnil(L);
		int i=0;
		while(lua_next(L,1))
		{
			noxKeyPack->Spells[r*5 + i ].Spell=lua_tointeger(L,-1);
			lua_pop(L,1);
			i++;
			if(i==5) break;
		}
		return 0;
	}
	int myKeySelectedRow=4;
	void __cdecl myKeyResetSpell()
	{
		if ((*dw_6D4918)!=0 || (*dw_6D492C)!=0 )return;
/*		lua_getglobal(L,"keyOnReset");
		if(lua_type(L,-1)!=LUA_TFUNCTION)
			lua_pop(L,1);
		else
		{
			if(0!=lua_pcall(L,0,1,0))
				lua_pop(L,1);
			if(lua_toboolean(L,-1))
			{
				lua_pop(L,1);
				drawSpellIcons(noxKeyPack);
				return;
			}
			lua_pop(L,1);
		}*/

		if (sub_4160F0(7, *((DWORD*)0x85B3FC)>>1 )==0)
		{
			myKeySelectedRow++;
			if(myKeySelectedRow<5)
			{
				noxKeyPack->selectedRow=4;
				myKeySelectedRow=4;
				return;
			}
		}
		else
			myKeySelectedRow=0;
		noxKeyPack->selectedRow=myKeySelectedRow;
		noxKeyPack->keyRowPtr=noxKeyPack->keyRowPtr + 5 * noxKeyPack->selectedRow;
		sub_4160D0(7);
		wnd_452D80(0x31E,100);//может это звук? 0x31E было
		drawSpellIcons(noxKeyPack);
	}
	int myFixKeyFn(lua_State*L)
	{
		InjectJumpTo(0x4604F0,&myKeyNextSpell);
		InjectJumpTo(0x460540,&myKeyPrevSpell);
		InjectJumpTo(0x460590,&myKeyResetSpell);
		return 0;
	}
}



void keysInit()
{
	ASSIGN(keyNextSpell,0x4604F0);
	ASSIGN(keyPrevSpell,0x460540);
	ASSIGN(keyResetSpell,0x460590);
	ASSIGN(noxKeyPack, *((DWORD*)0x5A7968) );
	ASSIGN(get_57AF20,0x57AF20);
	ASSIGN(clientStoreLastButton,0x45DAD0);
	ASSIGN(dw_6D4918,0x6D4918);
	ASSIGN(dw_6D492C,0x6D492C);
	ASSIGN(sub_4160F0,0x4160F0);
	ASSIGN(sub_4160D0,0x4160D0);
	ASSIGN(wnd_452D80,0x452D80);
	ASSIGN(get_57AF20,0x57AF20);
	ASSIGN(drawSpellIcons,0x45DDF0);
	ASSIGN(keyUpdateRow,0x45E110);
	registerclient("keyLoad",&myFixKeyFn);
	registerclient("keyRowSet",&keySetRowL);
	registerclient("keyRowGet",&keyGetRowL);
	registerclient("keyRowSel",&keyRowSelectL);
	
}