#include "stdafx.h"
BYTE *(__cdecl *playerInfoFromNetCodeCli) (int NetCode);
void (__cdecl *clientCollideOrUse)(void *sprite);
extern BYTE **clientPlayerInfoPtr;
extern DWORD (__cdecl *netGetUnitCodeCli)(void *Sprite);
extern bigUnitStruct *netUnitByCodeServ(DWORD NetCode);

namespace
{
	int *idPolyp;
	int (__cdecl *client_4984B0)(void *sprite);

	void __cdecl onClientUse(void *sprite)
	{// сюда мы попадаем когда клиент нажал кнопку
		//не будем вызывать коллайд, пошлем свой пакет, а сервер пусть разбирается
		//clientCollideOrUse(sprite);
		if (sprite==NULL)
			return;
		BYTE Buf[20];
		BYTE *Pt=Buf;
		
		netUniPacket(upTryUnitUse,Pt,3);
//		*(Pt++) = (3 & (*clientPlayerInfoPtr)[0xE60]); // если обсервер или куда нить смотрит
		*((WORD*)Pt)=netGetUnitCodeCli(sprite);
		Pt+=2;
		netSendServ(Buf,Pt-Buf);
	}

#define or |
	int __cdecl clientPick1(BYTE *sprite,
		DWORD Class,DWORD SubClass,int TT,int NetCode) /// проверяет выделяемость цели
	{
		int Top=lua_gettop(L);
		lua_getglobal(L,"clientOnUnitHover");
		if (lua_type(L,-1)==LUA_TFUNCTION)
		{
			lua_pushinteger(L,TT);
			lua_pushinteger(L,NetCode);
			lua_pushlightuserdata(L,sprite);
			if (0==lua_pcall(L,3,1,0))
			{
				int X=lua_tointeger(L,-1);
				if (X>0)
				{	
					lua_settop(L,Top);
					return 1;
				}else if(X<0)
				{
					lua_settop(L,Top);
					return 0;
				}
			}
		}
		lua_settop(L,Top);

		if (Class & clMonster)
		{
			if (0x4000 & SubClass)
				return 0; //no target
		}
		if (0==(Class & (clMonster or clPlayer or clTrigger or clImmobile or clPickup )))
		{
			if (TT!=*idPolyp)	return 0;
		}
		if (0==client_4984B0(sprite))
			return 0;
		if (Class & clPlayer)
		{
			BYTE *P=playerInfoFromNetCodeCli(NetCode);
			if (P==NULL)	return 0;
			if (P[0xE60] & 1)	return 0;
		}
		if (Class & clImmobile)
		{
			if (0== (SubClass & 0x80))	return 0;
		}
		if (Class & clMonster)
		{
			if ( 0xA==*((DWORD*)(sprite+0x114)))
				return 0; //no target
		}
		return 1;
	}
	int __cdecl clientPick2(BYTE *sprite,
		DWORD Class,DWORD SubClass,int TT,int NetCode) 
			/// определяет какой курсор цели рисовать
	{
		int Top=lua_gettop(L);
		lua_getglobal(L,"clientOnUnitHover");
		if (lua_type(L,-1)==LUA_TFUNCTION)
		{
			lua_pushinteger(L,TT);
			lua_pushinteger(L,NetCode);
			lua_pushlightuserdata(L,sprite);
			if (0==lua_pcall(L,3,1,0))
			{
				int X=lua_tointeger(L,-1);
				lua_settop(L,Top);
				return X;
			}
		}
		lua_settop(L,Top);

		return 0;
	}
#undef or

	void __declspec(naked) clientPick1Pre()
	{
		__asm{
		push [esi+0x80]
		push [esi+0x6C]
		push [esi+0x74]
		push [esi+0x70]
		push esi
		call clientPick1
		add esp,0x14
		test eax,eax
		jnz l1
			push 0x004773AF
			ret
		l1:
			push 0x00477140
			ret
		}
	}
	void __declspec(naked) clientPick2Pre()
	{
		__asm{
		push [edi+0x80]
		push [edi+0x6C]
		push [edi+0x74]
		push [edi+0x70]
		push edi
		call clientPick2
		add esp,0x14
		test eax,eax
		jnz l1 
			mov eax,[edi+0x70]
			test eax, 2|4|0x400000 //сlMonster | clPlayer | clImmobile
			jz l2
				push 0x0046BE27
				ret
			l2:
			push 0x0046BF07
			ret
		l1:
			push eax
			push 0x0046C0CA // устанавливаем иконку
			ret
		}
	}

}
void netOnClientTryUse(BYTE *Start,BYTE *End,BYTE *MyUc,void *Player) // к серверу пришел пакет
{
	BYTE *P=Start;
	BYTE *PI=*((BYTE**)(MyUc+0x114));
	if (0!=(0x3&PI[0xE60]) )
		return;
	if (0!= *((DWORD*)(MyUc+0x118)))
		return;
	if (0!= *((DWORD*)(MyUc+0x11C)))
		return;
	bigUnitStruct *Unit=netUnitByCodeServ(*((WORD*)(Start)));
	void (__cdecl *Fn)(void *Me,void *Player,int);
	if (Unit==NULL)
		return;
	if (Unit->collideFn!=NULL)
	{
		ASSIGN(Fn,Unit->collideFn);
		Fn(Unit,Player,0);
	}
	
}

extern void InjectJumpTo(DWORD Addr,void *Fn);
extern void InjectOffs(DWORD Addr,void *Fn);
void clientViewInit()
{
	ASSIGN(idPolyp,0x006E015C);
	ASSIGN(client_4984B0,0x004984B0);
	ASSIGN(playerInfoFromNetCodeCli,0x00417040);
	ASSIGN(clientCollideOrUse,0x0042E810);

	InjectOffs(0x0042D820+1,onClientUse);
	InjectJumpTo(0x004770B6,&clientPick1Pre);
	InjectJumpTo(0x0046BE1A,&clientPick2Pre);
}

