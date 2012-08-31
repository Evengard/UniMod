#include "stdafx.h"
#include <list>

int (__cdecl *getTTByNameSpriteMB)(void *Key);
int (__cdecl *createTextBubble)(void *BubbleStruct,wchar_t *Str);
void (__cdecl *sub_4738E0) ();

extern void (__cdecl *netClientSend) (int PlrN,int Dir,//1 - клиенту
								void *Buf,int BufSize);
extern DWORD (__cdecl *netGetUnitCodeServ)(void *Unit);
extern void (__cdecl *noxGuiDrawCursor) ();
int (__cdecl *mathAnglTo256) (void *xycords);

void *noxSpriteLast=0;
DWORD *gameFPS=(DWORD*)0x0085B3FC;


namespace
{
	DWORD *frameCounter=(DWORD*)0x0084EA04;

	int cliTimeoutNextId=1;
	struct cliTimeoutListRec
	{
		int Id;
		DWORD Frame;
		cliTimeoutListRec(int Id_,DWORD Frame_):Id(Id_),Frame(Frame_)
		{}
	};
	std::list<cliTimeoutListRec> cliTimeoutList;
	int cliSetTimeoutL(lua_State *L) /// теперь получает 3-й аргумент - таблицу
	{
		lua_settop(L,3);
		if ((lua_type(L,1)!=LUA_TFUNCTION) ||(lua_type(L,2)!=LUA_TNUMBER) 
			|| ((lua_type(L,3)!=LUA_TTABLE) && (lua_type(L,3)!=LUA_TNIL))
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushlightuserdata(L,&cliSetTimeoutL);/// функции
		lua_gettable(L,LUA_REGISTRYINDEX);
			lua_pushinteger(L,cliTimeoutNextId);
			lua_pushvalue(L,1);
			lua_settable(L,-3);
		lua_pushlightuserdata(L,&cliTimeoutNextId);/// сюда положим таблицу
		lua_gettable(L,LUA_REGISTRYINDEX);
			lua_pushinteger(L,cliTimeoutNextId);
			lua_pushvalue(L,3);
			lua_settable(L,-3);

		
		lua_pushinteger(L,cliTimeoutNextId);
		
		DWORD Time=*frameCounter +(DWORD)lua_tointeger(L,2);
		
		std::list<cliTimeoutListRec>::iterator I;
		for (I=cliTimeoutList.begin();I!=cliTimeoutList.end();I++)
		{
			if(I->Frame > Time)
				break;
		}
		cliTimeoutList.insert(I,cliTimeoutListRec(cliTimeoutNextId++,Time) );
		return 1;
	}

	void __cdecl cliOnEachFrame()
	{
		DWORD Time=*frameCounter;
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L,&cliTimeoutNextId);/// таблица аргументов
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,&cliSetTimeoutL);/// таблица функций
		lua_gettable(L,LUA_REGISTRYINDEX);

		for (std::list<cliTimeoutListRec>::iterator I=cliTimeoutList.begin();I!=cliTimeoutList.end();)
		{
			if ( Time < I->Frame )
				break;
			lua_pushinteger(L,I->Id);
			if (I->Frame <= Time)
			{
				lua_gettable(L,-2);
				if(lua_type(L,-1)==LUA_TFUNCTION)
				{


					lua_pushinteger(L,Time);
					lua_pushinteger(L,I->Id);
					// таблица с аргументом
					lua_gettable(L,-5); // id,Time,Fn, {Fns},{Args}
					if (0!=lua_pcall(L,2,0,0))
					{
						char Err[250];
						sprintf(Err,"Error: %240s",lua_tostring(L,-1));
						conPrintI(Err);
						lua_pop(L,1);
					}
					///conOutput,env, fn, {Fns},{Args}
				/*	lua_getfield(L,-2,"conOutput");// conOutput функция енв функция
					if (lua_type(L,-1)==LUA_TNIL) // если задали другую функцию - то так и оставим, но как ее обнулить?
					{
						lua_pop(L,1);
						lua_setfield(L,-2,"conOutput");
						lua_pop(L,2);
					}
					else
						lua_pop(L,3); */

					lua_pushinteger(L,I->Id); // чтобы было чего удалять
				}
				else
					lua_pop(L,1);

			}
			lua_pushnil(L);
			lua_settable(L,-3); // удаляем функцию
			lua_pushinteger(L,I->Id);
			lua_pushnil(L);
			lua_settable(L,-4);// удаляем аргумент

			I=cliTimeoutList.erase(I);
		}
		lua_settop(L,Top);
	}






	void __declspec(naked) asmToCliTimer() // вызываем тик 
	{
		__asm
		{
			call noxGuiDrawCursor
			call cliOnEachFrame
			push 0x0043E77D
			ret
		}
	}
	struct BubblePacket
	{
		byte PacketType;//+0
		short NetCode;//+1
		byte Dummy;// +3
		short PosX,PosY;//+4
		byte TimeoutSecMB;//+8
		short TimeoutFrames;//+9 если 0 - использвоать в секундах
		
	};
	int cliShowBubble(lua_State *L) /// теперь получает 3-й аргумент - таблицу
	{
		lua_settop(L,3);
		if ( (!lua_isnumber(L,1))
			|| (!lua_isstring(L,2))
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BubblePacket P;
		memset(&P,0,sizeof(P));
		P.NetCode=lua_tointeger(L,1);
		int v=lua_tointeger(L,3);
		if (v<1 || v>250)
			v=2;
		P.TimeoutSecMB=v;
		wchar_t WBuf[100];
		mbstowcs(WBuf,lua_tostring(L,2),99);
		createTextBubble(&P,WBuf);
		return 0;
	}
	//юнит, строка, [таблица кому == всем],[таймаут==2]
	int createBubble(lua_State *L) /// теперь получает 3-й аргумент - таблицу
	{
		lua_settop(L,5);

		int i=2;
		int x=0,y=0;
		short Code=0;
		if (lua_isuserdata(L,1))
		{
			void *Unit=lua_touserdata(L,1);
			if (Unit==0)
			{
				lua_pushstring(L,"wrong args!");
				lua_error_(L);			
			}
			Code=netGetUnitCodeServ(Unit);
		}else if ( lua_isnumber(L,1))
		{
			x=lua_tointeger(L,1);
			y=lua_tointeger(L,2);
			i++;
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}

		size_t Len;
		const char *S=lua_tolstring(L,i,&Len);
		i++;
		if (lua_istable(L,i))
		{
			i++;
		}
		int v=lua_tointeger(L,i);
		if (v<1 || v>250)
			v=2;
		if (Len>200)
			Len=200;
		BYTE Buf[208],*P=Buf;
		netUniPacket(upSendBubble,P,Len+6);

		if (Code==0)
			*((unsigned short*)P)=x;
		else
			*((unsigned short*)P)=Code;
		P+=2;
		*((unsigned short*)P)=y;
		P+=2;
		*(P++)=v;
		memcpy(P,S,Len+1);
		P+=Len;
		for(bigUnitStruct* Plr=playerFirstUnit();Plr!=0;Plr=playerNextUnit(Plr))
		{
			if ((Code!=0)?(i!=3):(i!=4))
			{
				bool Skip=true;
				lua_pushnil(L);
				while (lua_next(L,i-1)!=0)
				{
					lua_pushlightuserdata(L,Plr);
					if (lua_equal(L,-1,-2))
					{
						Skip=false;
						lua_pop(L,3);
						break;
					}
					lua_pop(L,2);
				}
				if (Skip)
					continue;
			}
			BYTE *P2=(BYTE*)Plr->unitController;
			P2+=0x114;P2=*((BYTE **)P2);
			P2+=0x810;
			netClientSend(*P2,1,Buf,P-Buf);
		}

		return 0;
	}
	void netOnBubble(BYTE *Packet)
	{

		BubblePacket P;
		memset(&P,0,sizeof(P));
		P.PosX=*((short*)Packet);
		Packet+=2;
		P.PosY=*((short*)Packet);
		Packet+=2;
		if (P.PosY==0)
		{
			P.NetCode=P.PosX;
			P.PosX=0;
		}
		int v=*(Packet++);
		if (v<1 || v>250)
			v=2;
		P.TimeoutFrames= v * (*gameFPS);
		wchar_t WBuf[240];
		mbstowcs(WBuf,(char*)Packet,240);
		createTextBubble(&P,WBuf);
	}

	/* читерство поганное! потом подумаем что с этим делать
	int spriteGetL(lua_State*L)
		{
		if (lua_type(L,1)!=LUA_TFUNCTION)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE *P=(BYTE*)noxSpriteLast; // непотяно все ли спрайты попадут или нет
		P=*(BYTE**)(P);
		if (P==0)
			return 0;
		BYTE *P1=P;
		while (P!=0)
		{
			lua_pushvalue(L,1);
			lua_pushlightuserdata(L,(void *)P);
			lua_pcall(L,1,0,0);
			P=*(BYTE**)(P+0x170);
		}
		return 0;
	}

	int spriteGetPosL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args");
			lua_error_(L);
		}
		BYTE *P=(BYTE*) lua_touserdata(L,1);
		lua_pushnumber(L,*((int*)(P+0xC)));
		lua_pushnumber(L,*((int*)(P+0x10)));
		return 2;
	}

	int spriteThingTypeL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args");
			lua_error_(L);
		}
		BYTE *P=(BYTE*) lua_touserdata(L,1);
		lua_pushnumber(L,*((int*)(P+0x6C)));
		return 1;
	}
	*/
	
	struct xyCoords
	{
		float x;
		float y;
	};
	int directL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TNUMBER ||
			lua_type(L,2)!=LUA_TNUMBER ||
			lua_type(L,3)!=LUA_TNUMBER ||
			lua_type(L,4)!=LUA_TNUMBER)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		float x=lua_tonumber(L,1) - lua_tonumber(L,3);
		float y=lua_tonumber(L,2) - lua_tonumber(L,4);
		xyCoords xy;
		xy.x=x; xy.y=y;
		int ang=mathAnglTo256(&xy);
		lua_pushinteger(L,ang);
		return 1;
	}
	
}

extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);
void cliUntilInit()
{
	ASSIGN(getTTByNameSpriteMB,0x044CFC0);
	ASSIGN(createTextBubble,0x0048D880);
	ASSIGN(sub_4738E0,0x4738E0);
	ASSIGN(noxSpriteLast,0x6D3DC0);
	ASSIGN(mathAnglTo256,0x509ED0); // arrrr


	lua_pushlightuserdata(L,&cliSetTimeoutL);/// функции
	lua_newtable(L);
	lua_settable(L,LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L,&cliTimeoutNextId);/// значения
	lua_newtable(L);
	lua_settable(L,LUA_REGISTRYINDEX);
	

	lua_pushcfunction(L,&cliSetTimeoutL); 
	// очень важная функция, ее надо в реестр луа класть 
	// чтобы нельзя было удалить случайно
	lua_pushvalue(L,-1); 
	lua_setfield(L,LUA_REGISTRYINDEX,"cliSetTimeout");
	registerClientVar("cliSetTimeout");
	//registerclient("spriteGet",&spriteGetL);
	registerclient("cliBubble",&cliShowBubble);
	registerserver("createBubble",&createBubble);
	//registerclient("spriteGetPos",&spriteGetPosL);
	//registerclient("spriteThingType",&spriteThingTypeL);
	registerclient("directGet",&directL);
	netRegClientPacket(upSendBubble,&netOnBubble);

	InjectJumpTo(0x0043E778,&asmToCliTimer);

}