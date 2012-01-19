#include "stdafx.h"
#include <string.h>
#include <math.h>

int UnimodVersion=010601;/// 00.06.01 

void (__cdecl *netClientSend) (int PlrN,int Dir,//1 - клиенту
								void *Buf,int BufSize);
// подозреваю что Bool - послать либо клиенту либо серверу

extern sprite_s *(__cdecl *spriteLoadAdd) (int thingType,int coordX,int coordY);
extern void (__cdecl *spriteDeleteStatic)(sprite_s *Sprite);

sprite_s *(__cdecl *netSpriteByCodeHi)(int netCode);// для статических
sprite_s *(__cdecl *netSpriteByCodeLo)(int netCode);// для динамических

void (__cdecl *netSendPointFx)(int PacketIdx,noxPoint* Pt);
void (__cdecl *netSendRayFx)(int PacketIdx,noxRectInt* Rc);
void (__cdecl *netPriMsg)(void *PlayerUnit,char *String,int Flag);

DWORD (__cdecl *netGetUnitCodeServ)(void *Unit);
DWORD (__cdecl *netGetUnitCodeCli)(void *Sprite);

void (__cdecl *netReportCharges) (int playerIdx, void *weapon, int a,int b);

void *(__cdecl *netGetUnitByExtent)(DWORD NetCode);/// только для динамических (на сервере)playerGoObserver

void (__cdecl *netSendShieldFx)(void *Unit,noxPoint* From);

extern "C" void conSendToServer(const char *Cmd);
extern "C" int conDoCmd(char *Cmd,bool &PrintNil);

extern void netOnTileChanged(BYTE *Buf,BYTE *End);
extern void netOnUpdateUnitDef(BYTE *Buf,BYTE *BufEnd);
extern int (__cdecl *consoleParse)(wchar_t*Str,int Mode);

int (__cdecl *netSendBySock)(int Player,void *Data,int Size, int Type);

byte authorisedState[0x20];
char *authorisedLogins[0x20];

void *playerSysop=(void*)0x0069D720;

//char *temp; //Временная переменная

extern void authCheckDelayed(byte playerIdx, char* pass);

extern char authSendWelcomeMsg[0x20];

extern void* getPlayerUDataFromPlayerIdx(int idx);

extern void* getPlayerUDataFromPlayerNetCode(int netCode);

extern void* getPlayerUDataFromPlayerInfo(void *addr);

int (__cdecl *netCreatePlayerStartPacket)(void* Dst, void* playerInfo);
void *(__cdecl *playerCheckDuplicateNames)(void* playerInfo);

int clientsVersions[0x20];

extern bool replayPacketHandler(BYTE *&BufStart, BYTE *&E, bool &found);

bigUnitStruct *netUnitByCodeServ(DWORD NetCode)
{
	if (NetCode & 0x8000)
		return (bigUnitStruct *)netGetUnitByExtent(NetCode&0x7FFF);
	return 0;
}
void netSendServ(void *Buf,int BufSize)
{
	netClientSend(0x1F,0,Buf,BufSize);// Похоже что так, но не уверен
}
void netSendNotOne(void *Buf,int BufSize,void *One) /// посылает всем клиентам  кроме одного
{
	for(bigUnitStruct* Plr=playerFirstUnit();Plr!=0;Plr=playerNextUnit(Plr))
	{
		BYTE *P2=(BYTE*)Plr->unitController;
		if (P2==One)
			continue;
		P2+=0x114;P2=*((BYTE **)P2);
		P2+=0x810;
		netClientSend(*P2,1,Buf,BufSize);
	}
}
void netSendAll(void *Buf,int BufSize)
{
	for(bigUnitStruct* Plr=playerFirstUnit();Plr!=0;Plr=playerNextUnit(Plr))
	{
		BYTE *P2=(BYTE*)Plr->unitController;
		P2+=0x114;P2=*((BYTE **)P2);
		P2+=0x810;
		netClientSend(*P2,1,Buf,BufSize);
	}
	
}
void conSendToServer(const char *Src)
{
	BYTE Buf[255],*P=Buf;
	size_t Size=strlen(Src)+1;
	
	if (Size>0 && Size<255)
	{
		netUniPacket(upLuaRq,P,Size);
		memcpy(P,Src,Size);
		netSendServ(Buf,Size+P-Buf);
	}

}

void netSendChatMessage(char *sendChat, int sendTo, short sendFrom=0, bool fakeSystemMessage=false)
{
	if(sendFrom==0 && fakeSystemMessage==false)
	{
		DWORD *DW=(DWORD*)getPlayerUDataFromPlayerIdx(0x1F);
		if (0==(DW[2] & 0x4))
		{
			return;
		}
		BYTE *unit=(BYTE*)getPlayerUDataFromPlayerIdx(0x1F);
		void **PP=(void **)(((char*)unit)+0x2EC);
		PP=(void**)(((char*)*PP)+0x114);
		byte *P=(byte*)(*PP);
		sendFrom=*((short*)(P+0x80C));

	}
	short netCode=0;
	if(fakeSystemMessage==false)
		netCode = sendFrom;
	byte sendChatPacketHeader[0xB] = {0xA8, 0x2, 0x0, 0x2, 0x72, 0x0B, 0x7D, 0x0B, 0x5, 0x0, 0x0};
	byte *sendChatPacket=new byte[strlen(sendChat)+0xB+1];
	memcpy(sendChatPacket, sendChatPacketHeader, 0xB);
	memcpy(&sendChatPacket[0xB], sendChat, strlen(sendChat));
	sendChatPacket[0x8]=strlen(sendChat)+1;
	*((short*)(sendChatPacket+1)) = netCode;
	//memcpy(&sendChatPacket[0x1], &netCode, 2);
	sendChatPacket[0xB+strlen(sendChat)]=0x0;
	if(sendTo<0 || sendTo>31)
	{
		netSendAll(sendChatPacket,strlen(sendChat)+0xB+1);
	}
	else
	{
		netClientSend(sendTo,1,sendChatPacket,strlen(sendChat)+0xB+1);
	}
	delete [] sendChatPacket;
	return;
}

bool specialAuthorisation=false; //Отключение альтернативной авторизации
namespace {

	BYTE *fakePlayerInputPacket(BYTE* BufStart)
	{
		BYTE *P=BufStart;
		if(P[1]<10)
		{
			BufStart+=P[1]+2;
			return BufStart;
		}
		else
		{
			const BYTE replace[12]={0x3f, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x81, 0x01, 0x00, 0x00, 0x00, 0x81};
			if(P[1]>10)
				BufStart+=(P[1]+2)-12;
			memcpy(BufStart, replace, 12);
			return BufStart;
		}
	}

	int sendToServer(lua_State *L)
	{
		const char *S=lua_tostring(L,1);
		if (S)
			conSendToServer(S);
		return 0;
	}
	int netGetCodeServL(lua_State *L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *P=lua_touserdata(L,1);
		if (P==0)
		{
			lua_pushnil(L);return 1;
		}
		lua_pushinteger(L,netGetUnitCodeServ(P));
		return 1;
	}
	int netReportChargesL(lua_State *L)
	{
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA) ||
			(lua_type(L,2)!=LUA_TLIGHTUSERDATA)||
			(lua_type(L,3)!=LUA_TNUMBER)||
			(lua_type(L,4)!=LUA_TNUMBER))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		bigUnitStruct *Plr=(bigUnitStruct *)lua_touserdata(L,1);
		BYTE *P2=(BYTE*)Plr->unitController;
		P2+=0x114;P2=*((BYTE **)P2);
		P2+=0x810;
		netReportCharges(*P2,lua_touserdata(L,2),lua_tointeger(L,3),lua_tointeger(L,4));
		return 0;
	}
		
	
	int netPointFx(lua_State *L)
	{
		lua_settop(L,3);
		if (
			(lua_type(L,-3)!=LUA_TNUMBER)||
			(lua_type(L,-2)!=LUA_TNUMBER)||
			(lua_type(L,-1)!=LUA_TNUMBER)||
			0
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		noxPoint P(lua_tonumber(L,-2),lua_tonumber(L,-1));
		int Code=lua_tointeger(L,-3);
		// Внимание - коды нужно брать из netMsgNames причем только подходящие площадные
		// причем позиция в списке +0x27
		// несогласные получат крэш
		// 0xA0-андед,0xA3  - манабомб
		netSendPointFx(Code,&P);
		return 0;
	}
	int netRayFx(lua_State *L)
	{
		lua_settop(L,5);

		if (
			(lua_type(L,-5)!=LUA_TNUMBER)||
			(lua_type(L,-4)!=LUA_TNUMBER)||
			(lua_type(L,-3)!=LUA_TNUMBER)||
			(lua_type(L,-2)!=LUA_TNUMBER)||
			(lua_type(L,-1)!=LUA_TNUMBER)||
			0
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		noxRectInt R(lua_tointeger(L,-4),lua_tointeger(L,-3),
			lua_tointeger(L,-2),lua_tointeger(L,-1));
		int Code=lua_tointeger(L,-5);
		// Внимание - коды нужно брать из netMsgNames причем только подходящие площадные
		// причем позиция в списке +0x27
		// следующий список:11,14,15,16,19,20 - к этому прибавть 0x9F
		// несогласные получат крэш
		// 
		netSendRayFx(Code,&R);
		return 0;
	}
	int netShieldFx(lua_State *L)
	{
		lua_settop(L,3);
		if (lua_type(L,-3)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		
		noxPoint Pt(lua_tonumber(L,-2),lua_tonumber(L,-1));
		netSendShieldFx(lua_touserdata(L,-3),&Pt);
		return 0;
	}
	int netRename(lua_State *L)
	{
		
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA || lua_type(L,2)!=LUA_TSTRING)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		DWORD *DW=(DWORD*)lua_touserdata(L,1);
		if (0==(DW[2] & 0x4))
		{
			lua_pushstring(L,"wrong args: unit is not a player!");
			lua_error_(L);
		}
		char RP[0x81];
		memset(&RP,0,sizeof(RP));
		BYTE *unit=(BYTE*)lua_touserdata(L,1);
		void **PP=(void **)(((char*)unit)+0x2EC);
		PP=(void**)(((char*)*PP)+0x114);
		byte *P=(byte*)(*PP);
		mbstowcs((wchar_t*)(P+0x1260), lua_tostring(L, 2), 0x18);
		mbstowcs((wchar_t*)(P+0x889), lua_tostring(L, 2), 0x18);
		mbstowcs((wchar_t*)(P+0x8E2), "\0\0", 0x2);
		playerCheckDuplicateNames(P);
		netCreatePlayerStartPacket(&RP,P);
		//memcpy(&RP.NameMB, (wchar_t*)(P+0x1260), (sizeof(wchar_t)*0x30));
		netSendAll(&RP,sizeof(RP));
		return 0;
	}
	int netFake(lua_State *L) 
			/// высылает сообщение о смерти, но от этого помирает нокс 
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
	
		struct {
			BYTE Pckt;
			USHORT Code;
		} B={0xE8,netGetUnitCodeServ(lua_touserdata(L,-1))};

		netSendAll(&B,sizeof(B));

		return 0;
	}
	int netOnRespL(lua_State *L)
	{
		bigUnitStruct *Plr=(bigUnitStruct *)lua_touserdata(L,1);
		char Buf[280];
		sprintf(Buf,"cli %p %s%s",Plr,(lua_tointeger(L,2)==0)?"error:":"ok:",lua_tostring(L,3) );
		conPrintI(Buf);	
		return 0;
	}
	int netDoReq(lua_State *L)/// собственно отправка сообщения
	{
		BYTE Buf[255],*P=Buf;
		size_t Size;
		const char *Src=lua_tolstring(L,1,&Size);
		if (Size>0 && Size<255)
		{
			Size++;
			netUniPacket(upLuaRq,P,Size);
			memcpy(P,Src,Size-1);
			netSendAll(Buf,Size+P-Buf);
		}
		else
		{
			lua_pushstring(L,"wrong: string too long (max 255)!");
			lua_error_(L);
		}
		return 0;
	}
	int netDoDelayed(lua_State *L)
	{
		int n=lua_gettop(L);
		lua_pushvalue(L,lua_upvalueindex(1));///вызываем сет таймаут
			lua_pushvalue(L,lua_upvalueindex(2));// fn
			lua_pushinteger(L,0); // 0 - time
			lua_newtable(L);
				for (int i=1;i<=n;i++)
				{
					lua_pushvalue(L,i);
					lua_rawseti(L,-2,i); //{"string"}
				}
		lua_call(L,3,0);
		return 0;
	}

	void netLuaRq(BYTE *P,BYTE *End)
	{
		BYTE Buf[255],*Pt=Buf;
		size_t Size;
		const char *Src;
		bool Ok=true;
		int From = lua_gettop(L); /// старая позиция стэка
		End[-1]=0;
		sprintf((char*)Buf,"cmd: %s",P);
		conPrintI((char*)Buf);	

		if (0!=luaL_loadstring(L,(char*)P))
		{ 
			Ok=false;
		}
		if (Ok && (0!=lua_pcall(L,0,1,0)) )
		{
			Ok=false;
		}
		Src=lua_tolstring(L,-1,&Size);
		if (Size<255)
		{
			netUniPacket(upLuaResp,Pt,Size+1);
			*(Pt++)=Ok?1:0;
			if (Src==NULL)
				*Pt=0;
			else
				memcpy(Pt,Src,Size);
			netSendServ(Buf,Size+Pt-Buf);
		}
		lua_settop(L,From);

	}
	void netDelStatic(BYTE *Start,BYTE *End)
	{
		int Code=*((int*)(Start+0));
		if (0==(Code&0x8000)) // не статический
			return;
		sprite_s *S=netSpriteByCodeHi(Code);
		BYTE* P=(BYTE *)S;
		*((int *)(P+0x80))|=0x8000;
		if (S)
			spriteDeleteStatic(S);
	}
	void netMoveStatic(BYTE *Start,BYTE *End)
	{
		int Code=*((int*)(Start+0));
		//TODO:
	}

	void netNewStatic(BYTE *Start,BYTE *End)
	{
		BYTE *S=
			(BYTE *)spriteLoadAdd( *((int*)(Start+0)),
				(*((float*)(Start+4))),
				(*((float*)(Start+8))));
		DWORD *Code=(DWORD *)(S+0x80);
		if (*Code!=*((DWORD*)(Start+0x0C)) )
			Code++;
	}
	int netDoPrintConsole(const char *Src,BYTE *pli,int color, bool crop=false)
	{
		BYTE Buf[259],*P=Buf;
		size_t Size;
		Size=strlen(Src);
		char* NextSrc = new char[Size+1];
		memset(NextSrc, 0, (Size+1));
		if (Size>0 && Size<200)
		{
		}
		else if(crop)
		{
			Size=199;
			strncpy(NextSrc, &Src[199], (Size+1));
		}
		else
		{
			return 1;
		}
		Size++;
		netUniPacket(upSendPrintToCli,P,Size+4);
		memcpy(P,&color,4);
		P+=4;
		memcpy(P,Src,Size);
		char* zero="\0";
		strncpy((char*)&P[199], zero, 1);
		pli+=0x810;
		netClientSend(*pli,1,Buf,Size+P-Buf);
		pli-=0x810;
		if(crop && strlen(NextSrc)>0)
			netDoPrintConsole(NextSrc, pli, color, crop);
		delete [] NextSrc;
		return 0;
	}

	int netDoPrintConsoleL(lua_State *L)
	{
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA || lua_type(L,2)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args");
			lua_error_(L);
		}
		BYTE *P=(BYTE*)lua_touserdata(L,1);
		int unitClass=*(P+8);
		if ((unitClass & clPlayer)==0)
		{		
			lua_pushstring(L,"wrong args");
			lua_error_(L);
		}
		P+=0x2EC;
		P=*((BYTE**)P);
		P+=0x114;
		P=*((BYTE**)P);
		int color=lua_tointeger(L,3);
		if (color==NULL || color<0 || color>16)
			color=2;
		int res = netDoPrintConsole(lua_tostring(L,2),P,color);
		if(res==1)
		{
			lua_pushstring(L,"wrong: string too long (max 255)!");
			lua_error_(L);
		}
		return 0;
	}

	void netPrintConsole(BYTE *P,BYTE *End)
	{
		int color=(int)*P;
		P+=4;
		char *Str=(char*)P;
		conSetNextColor(color);
		conPrintI(Str);
	}
	void netOnVersionRq(BYTE *Start,BYTE *End,bigUnitStruct* Plr)
	{
		if (End-Start<4)
			return;// какая-то ошибка
		BYTE *P2=(BYTE*)Plr->unitController;
		P2+=0x114;P2=*((BYTE **)P2);
		P2+=0x810;
		int Top= lua_gettop(L);
		int OtherVersion=*((int*)Start);
		clientsVersions[*P2]=OtherVersion;
		if (floor((double)(UnimodVersion/100))>floor((double)(OtherVersion/100)))
		{
			getServerVar("serverOnClientVersionWrong");
			if (lua_type(L,-1)==LUA_TFUNCTION)
			{
				lua_pushinteger(L,OtherVersion);
				lua_pushinteger(L,UnimodVersion);
				lua_pushinteger(L,*P2);
				lua_pushlightuserdata(L,Plr);
				lua_pcall(L,4,0,0);
			}
		}
		const char *Src;
		size_t Size=0;
		if (End-Start>4)
		{
			do
			{
				if (0!=luaL_loadbuffer(L,(char*)Start+4,End-Start-4,"netOnVersion"))
					break;
				lua_getfield(L,LUA_REGISTRYINDEX,"server");			
				lua_setfenv(L,-2);
				lua_pcall(L,0,1,0);
			} while(0);
			Src=lua_tolstring(L,-1,&Size);
			if (Size>200)
				Size=200;
			if (Src==NULL)
				Size=0;
		}
		BYTE Buf[255],*Pt=Buf;
		netUniPacket(upVersionResp,Pt,Size+4);
		*((int*)Pt)=UnimodVersion;
		Pt+=4;
		if (Size>0)
		{
			memcpy(Pt,Src,Size);
			Pt+=Size;
		}
		netClientSend(*P2,1,Buf,Pt-Buf);
		lua_settop(L,Top);
	}
	void netOnVersionResp(BYTE *Start,BYTE *End)
	{
		if (End-Start<4)
			return;// какая-то ошибка
		int Top= lua_gettop(L);
		int OtherVersion=*((int*)Start);

		getClientVar("clientOnServerVersion");
		if (lua_type(L,-1)==LUA_TFUNCTION)
		{
			lua_pushinteger(L,OtherVersion);
			lua_pushinteger(L,UnimodVersion);
			if (End-Start<4)
				lua_pushnil(L);
			else
			{
				lua_pushlstring(L,(char*)Start+4,End-Start-4);
			}
			lua_pcall(L,3,0,0);
		}
		lua_settop(L,Top);
	}
	
	void netOnVersionServerRq(BYTE *Start,BYTE *End)
	{
		BYTE Buf[255],*Pt=Buf;
		netUniPacket(upVersionRq,Pt,4);
		*((int*)Pt)=UnimodVersion;
		Pt+=4;
		netSendServ(Buf,Pt-Buf);
	}

	

	int netVersionRq(lua_State*L)
	{
		lua_settop(L,1);
		size_t Size;
		const char *Src=lua_tolstring(L,1,&Size);
		if (Size>200)
			Size=200;
		if (Src==NULL)
			Size=0;
		BYTE Buf[255],*Pt=Buf;
		netUniPacket(upVersionRq,Pt,Size+4);
		*((int*)Pt)=UnimodVersion;
		Pt+=4;
		if (Size>0)
		{
			memcpy(Pt,Src,Size);
			Pt+=Size;
		}
		netSendServ(Buf,Pt-Buf);
		return 0;
	}
	int netGetVersion(lua_State*L)
	{
		lua_pushinteger(L,UnimodVersion);
		return 1;
	}
	int netSendFx(lua_State*L)
	{
		lua_pushstring(L,"not implemented yet");
		lua_error(L);
		return 1;
	}
	void sysopMyTrap(wchar_t*Str,int Mode)
	{
		BYTE *P=(BYTE*) playerSysop;
		byte *playerInfo=*((byte**)(P));
		bool UniComplete=false;// если код выполнился то ставим true
		//здесь код1АДЫНАДЫН 
		int Len = wcslen(Str);
		char* Cmd = new char[Len+1];
		wcstombs(Cmd,Str,Len+1);
		lua_getfield(L,LUA_REGISTRYINDEX,"serverFnSysop");
		int success = luaL_loadstring(L, Cmd);
		if(success==0)
			UniComplete=true;
		if (UniComplete==false)
		{
			consoleParse(Str,Mode);
			return;
		}
		lua_pcall(L, 1, 1, 0);
		char* result=(char*)lua_tostring(L, -1);
		delete[] Cmd;
		void* unit = getPlayerUDataFromPlayerInfo(playerInfo);
		if(unit==0)
			return;
		DWORD *DW=(DWORD*)unit;
		if (0==(DW[2] & 0x4))
		{
			return;
		}
		void **PP=(void **)(((char*)unit)+0x2EC);
		PP=(void**)(((char*)*PP)+0x114);
		byte *P2=(byte*)(*PP);
		char *BuffS = new char[strlen(result)+10];
		memset(BuffS, 0, strlen(result)+10);
		sprintf(BuffS,"sysop> %s",result);
		netDoPrintConsole(BuffS,P2,14, true);
		delete[] BuffS;
	}
}

void netVersionServerRq(int sendTo)
{
	BYTE Buf[255],*Pt=Buf;
	netUniPacket(upVersionServerRq,Pt,0);
	netClientSend(sendTo,1,Buf,Pt-Buf);
}
/* пускай будет регистрация */
	ClientMap_s ClientRegMap;
	ServerMap_s ServerRegMap;

void netRegClientPacket(uniPacket_e Event,netClientFn_s Fn)
{
	ClientRegMap.insert(std::make_pair(Event,Fn));
}
void netRegServPacket(uniPacket_e Event,netServFn_s Fn)
{
	ServerRegMap.insert(std::make_pair(Event,Fn));
}
extern "C" void __cdecl onNetPacket(BYTE *&BufStart,BYTE *E);



void netUniPacket(uniPacket_e Code,BYTE *&Data,int Size)/// Отправить наш пакет
{
	*(Data++)=0xF8;//унипакет
	*(Data++)=Size+1;
	*(Data++)=(BYTE)Code;
}
extern void netOnWallChanged(wallRec *newData);
extern void netOnSendArchive(int Size,char *Name,char *NameE);
extern void netOnAbortDownload();
void __cdecl onNetPacket(BYTE *&BufStart,BYTE *E)/// Полученые клиентом
{
	bool found=true;
	// ВНИМАНИЕ! Для всех ВЫФИЛЬТРОВЫВАЕМЫХ пакетов - обязательно ставьте found=true! Иначе не будет производиться обработка других пакетов в ЭТОМ же фрейме!
	while(found)
	{
		found=false;
		replayPacketHandler(BufStart, E, found); //По умолчанию false
		BYTE *P=BufStart;
		if (*P==186)
			netOnAbortDownload();
/*		if(*P==0x2B) // Обработчик смены мапы. Пишем предыдущую мапу, потом текущую, потом снова список всех.
			{
				struct ServerData
{
	char mapName[8];
	char gap[1];
	char gameName[10];
	char gap_13[5];
	int field_18[5];
	DWORD weaponRestrictions;
	DWORD armorRestrictions;
	__int16 gameFlags;
	__int16 fragLimit;
	char timeLimitMB;
	char isNew;
};
				extern int *mapFrameTime_6F9838;
extern void *(__cdecl *serverGetGameDataBySel)();
extern char *(__cdecl *mapGetName)();
				P=P+0x25;
				ServerData *Data=(ServerData*)serverGetGameDataBySel();
					char* mapName2 = mapGetName();
					char* mapName = Data->mapName;
					mapName2 = mapGetName();

			}*/ // For testing purposes only
		else if (*P==0xF8)/// это будет первый юнимод-пакет {F8,<длина>, данные}
		{
			P++;
			BufStart+=2+*(P++);// выфильтровываем его нафиг
			found=true;
			
			char Buf[60];
			sprintf(Buf,"Unipacket cli %x",*P);
			conPrintI(Buf);

			switch (*(P++))
			{
			case upWallChanged:
				netOnWallChanged((wallRec*)P);
				break;
			case upLuaRq:
				netLuaRq(P,BufStart);
				break;
			case upNewStatic:
				netNewStatic(P,BufStart);
				break;
			case upDelStatic:
				netDelStatic(P,BufStart);
				break;
			case upMoveStatic:
				netMoveStatic(P,BufStart);
				break;
			case upUpdateDef:
				netOnUpdateUnitDef(P-UNIPACKET_HEAD,BufStart);
				break;
			case upSendArchive:
				netOnSendArchive( *((int*)P),(char*)P+sizeof(int),(char*)BufStart);
				break;
			case upChangeTile:
				netOnTileChanged(P,BufStart);
				break;
			case upVersionResp:
				netOnVersionResp(P,BufStart);
				break;
			case upVersionServerRq:
				netOnVersionServerRq(P,BufStart);
				break;
			case upSendPrintToCli:
				netPrintConsole(P,BufStart);
				break;
			default:
				{
					ClientMap_s::const_iterator I=ClientRegMap.find(P[-1]);
					if (I!=ClientRegMap.end())
						I->second(P);
				}
			};
		}
	}
}
extern void spellServDoCustom(int SpellArr[5],bool OnSelf,BYTE *MyPlayer,BYTE *MyUc);

extern void netOnClientTryUse(BYTE *Start,BYTE *End,BYTE *MyUc,void *Player);
extern "C" void __cdecl onNetPacket2(BYTE *&BufStart,BYTE *E,
		BYTE *MyPlayer, /// bigUnitStruct
		BYTE *MyUc)/// Полученые сервером
{
	bool found=true;
	// ВНИМАНИЕ! Для всех ВЫФИЛЬТРОВЫВАЕМЫХ пакетов - обязательно ставьте found=true! Иначе не будет производиться обработка других пакетов в ЭТОМ же фрейме!
	while(found)
	{
		found=false;
		BYTE *P=BufStart;
		if (*P==0x3F && specialAuthorisation==true)
		{
			void **PP=(void **)(((char*)MyPlayer)+0x2EC);
			PP=(void**)(((char*)*PP)+0x114);
			byte *P1=(byte*)(*PP);
			byte playerIdx = *((byte*)(P1+0x810));
			if(playerIdx!=0x1F) // Так Нокс определяет Хоста
				switch(authorisedState[playerIdx])
				{
					case 0: // Поидее сюда вообще не должно падать - игрока на сервере ещё нет
					case 1: // Игрок на сервере, не начинал процедуру авторизации
					case 2: // Игрок на сервере, ввёл логин
					case 3: // Игрок на сервере, ввыл логин и пароль, ожидает авторизации
						BufStart=fakePlayerInputPacket(BufStart);
						// В этом состоянии игрок будет всё время пока не залогинется
						break;
					case 4: // Игрок на сервере, авторизован
						// А в этом - залогинился наш голубчик
						break;
				}
		}
		else if(*P==0xA8 && specialAuthorisation==true)
		{
			
			void **PP=(void **)(((char*)MyPlayer)+0x2EC);
			PP=(void**)(((char*)*PP)+0x114);
			byte *Pl=(byte*)(*PP);
			byte playerIdx = *((byte*)(Pl+0x810));
			char *authCmd="//auth ";
			int cmdTokenL=0;
			if(strncmp((char*)&P[0xB], authCmd, ((BufStart[0x8]>strlen(authCmd))?(strlen(authCmd)):(BufStart[0x8])))==0)
				cmdTokenL=strlen(authCmd);
			if(playerIdx!=0x1F && authorisedState[playerIdx]>=0 && authorisedState[playerIdx]<4)
			{
				switch(authorisedState[playerIdx])
				{
					case 0: 
					case 3:
						BufStart+=BufStart[0x8]+0xB;
						found=true;
						break;
					case 1:
						// Тут только логин сейвим
						{
							char *login=NULL;
							login = new char[P[0x8]-cmdTokenL];
							strncpy(login, (char*)&P[0xB+cmdTokenL], P[0x8]-cmdTokenL);
							authorisedLogins[playerIdx]=login;
							authorisedState[playerIdx]++;
							authSendWelcomeMsg[playerIdx]=-1;
							BufStart+=BufStart[0x8]+0xB;
							found=true;
							login = NULL;
						}
						break;
					case 2:
						{
							//char *data=NULL;
							//data = new char[P[0x8]+1];
							char* pass = new char[P[0x8]-cmdTokenL];
							//memcpy(data, &playerIdx, 1);
							//strncpy(&data[1], (char*)&P[0xB], P[0x8]);
							strncpy(pass, (char*)&P[0xB+cmdTokenL], P[0x8]-cmdTokenL);

							// Сюда добавить логику запуска аутентификации по http

							// TEMPORARY!
							//temp=data;
							// TEMPORARY! END
							authorisedState[playerIdx]++;
							authSendWelcomeMsg[playerIdx]=-1;
							authCheckDelayed(playerIdx, pass);
							BufStart+=BufStart[0x8]+0xB;
							found=true;
							//data = NULL;
							pass=NULL;
						}
						break;
					/*case 3:
						{
							BufStart+=BufStart[0x8]+0xB;

							// TEMPORARY!
							if(strcmp(&temp[1], "password")==0)
								authorisedState[playerIdx]++;
							else
								authorisedState[playerIdx]-=2;
							// TEMPORARY! END
						}
						break;*/
				}
			}
			if(cmdTokenL>0 && found==false)
			{
				BufStart+=BufStart[0x8]+0xB;
				found=true;
			}
		}
		else if(*P==0xBB && specialAuthorisation==true && ((char)P[0x4])>0)
		{
			
			void **PP=(void **)(((char*)MyPlayer)+0x2EC);
			PP=(void**)(((char*)*PP)+0x114);
			byte *Pl=(byte*)(*PP);
			byte playerIdx = *((byte*)(Pl+0x810));
			char *authCmd=" //auth ";
			int cmdTokenL=0;
			char *command = new char[BufStart[0x4]];
			wcstombs(command,((wchar_t*)&BufStart[0x5]),BufStart[0x4]);
			if(strncmp((char*)command, authCmd, ((BufStart[0x4]>strlen(authCmd))?(strlen(authCmd)):(BufStart[0x4])))==0)
				cmdTokenL=strlen(authCmd);
			if(playerIdx!=0x1F && authorisedState[playerIdx]>=0 && authorisedState[playerIdx]<4 && cmdTokenL==strlen(authCmd))
			{
				switch(authorisedState[playerIdx])
				{
					case 0: 
					case 3:
						BufStart+=BufStart[0x4]*2+0x7;
						found=true;
						break;
					case 1:
						// Тут только логин сейвим
						{
							char *login=NULL;
							login = new char[P[0x4]-cmdTokenL];
							strncpy(login, (char*)&command[cmdTokenL], P[0x4]-cmdTokenL);
							authorisedLogins[playerIdx]=login;
							authorisedState[playerIdx]++;
							authSendWelcomeMsg[playerIdx]=-1;
							BufStart+=BufStart[0x4]*2+0x7;
							found=true;
							login = NULL;
						}
						break;
					case 2:
						{
							//char *data=NULL;
							//data = new char[P[0x8]+1];
							//char* pass = new char[P[0x8]-cmdTokenL];
							//memcpy(data, &playerIdx, 1);
							//strncpy(&data[1], (char*)&P[0xB], P[0x8]);
							//strncpy(pass, (char*)&P[0xB+cmdTokenL], P[0x8]-cmdTokenL);
							char* pass = new char[P[0x4]-cmdTokenL];
							strncpy(pass, (char*)&command[cmdTokenL], P[0x4]-cmdTokenL);
							// Сюда добавить логику запуска аутентификации по http

							// TEMPORARY!
							//temp=data;
							// TEMPORARY! END
							authorisedState[playerIdx]++;
							authSendWelcomeMsg[playerIdx]=-1;
							authCheckDelayed(playerIdx, pass);
							BufStart+=BufStart[0x4]*2+0x7;
							found=true;
							//data = NULL;
							pass=NULL;
						}
						break;
					/*case 3:
						{
							BufStart+=BufStart[0x8]+0xB;

							// TEMPORARY!
							if(strcmp(&temp[1], "password")==0)
								authorisedState[playerIdx]++;
							else
								authorisedState[playerIdx]-=2;
							// TEMPORARY! END
						}
						break;*/
				}
			}
			if(cmdTokenL>0 && found==false)
			{
				BufStart+=BufStart[0x4]*2+0x7;
				found=true;
			}
		}
		else if (*P==0xF8)/// это будет первый юнимод-пакет {F8,<длина>, данные}
		{
			P++;
			int BufSize=*(P++);
			BufStart+=2+BufSize;// выфильтровываем его нафиг
			found=true;
			BufSize--;// пускай диспетчерский байт не в счет

			char Buf[60];
			sprintf(Buf,"Unipacket serv %x",*P);
			conPrintI(Buf);

			switch (*(P++))
			{
			case upLuaResp: //TODO: диспетчеризацию ответов на разные вопросы
				lua_getglobal(L,"netOnResp");
				if (lua_type(L,-1)==LUA_TFUNCTION)
				{
					lua_pushlightuserdata(L,MyPlayer);
					lua_pushinteger(L,*(P++));
					lua_pushlstring(L,(char*)P,BufSize-1);
					lua_pcall(L,3,0,0);
				}
				break;
			case upLuaRq:
				char Buf[200];bool Unused;
				strncpy(Buf,(char *)P,199);Buf[199]=0;
				conDoCmd(Buf,Unused);
				sprintf(Buf,"cmd %s",P);
				conPrintI(Buf);
				break;
			case upTryUnitUse:
				netOnClientTryUse(P,BufStart,MyUc,MyPlayer);
				break;
			case upVersionRq:
				netOnVersionRq(P,BufStart,(bigUnitStruct*)MyPlayer);
				break;
			default:
				{
					ServerMap_s::const_iterator I=ServerRegMap.find(P[-1]);
					if (I!=ServerRegMap.end())
						I->second(P,MyPlayer,MyUc);
				}
			}
			//return;
		} else if (*P==0x79) // TRY_SPELL
		{
			if ( *((DWORD*)(P+1)) > 0x100 ) /// если это "наши" спелы - надо самим решать чего с ними делать
			{
				spellServDoCustom((int*)(P+1),(P[15])!=0,MyPlayer,MyUc);
				found=true;
				BufStart+=0x16;
			}
		}
		else if (*P==0x72) // попытка выкинуть предмет
		{ // 7 байт {BYTE pkt, USHORT Obj,X,Y;}
			char Buf[80];
			USHORT V=toShort(P+1);
			netUnitByCodeServ(V);
			sprintf(Buf,"72 %x (%d,%d)",V,toShort(P+3),toShort(P+5) );
			conPrintI(Buf);	

		}
	}
}
extern void InjectJumpTo(DWORD Addr,void *Fn);
extern void InjectOffs(DWORD Addr,void *Fn);
int sendChat(lua_State* L)
{
	if(lua_isstring(L, 2) && lua_isnumber(L, 1))
	{
		bool fakeSys=false;
		if(lua_isnumber(L, 4))
		{
			 fakeSys = (bool)lua_tonumber(L, 4);
		}
		short from=0;
		if(lua_type(L, 3)==LUA_TNUMBER)
		{
			from=(short)lua_tonumber(L, 3);
		}
		netSendChatMessage((char*)lua_tostring(L, 2), (int)lua_tonumber(L, 1), from, fakeSys);
	}
	else
	{
		lua_pushstring(L,"wrong args!");
		lua_error_(L);
	}
	return 0;
}
void netInit()
{
	ASSIGN(playerSysop,0x0069D720);

	ASSIGN(netSpriteByCodeHi,0x0045A720);
	ASSIGN(netSpriteByCodeLo,0x0045A6F0);

	ASSIGN(netClientSend,0x0040EBC0);
	ASSIGN(netGetUnitCodeServ,0x00578AC0);
	ASSIGN(netGetUnitByExtent,0x4ED020);
	ASSIGN(netGetUnitCodeCli,0x00578B00);
	ASSIGN(netSendBySock,0x00552640);
	ASSIGN(netPriMsg,0x004DA2C0);
	ASSIGN(playerCheckDuplicateNames,0x004DDA00);
	ASSIGN(netReportCharges,0x004D82B0);
	
	InjectOffs(0x4441AE+1,&sysopMyTrap);

	const char Block2[]="Srv";
	registerserver("servNetCode",&netGetCodeServL);

	/// Стандартные спецэффекты
	ASSIGN(netSendPointFx,0x522FF0);
	ASSIGN(netSendRayFx,0x5232F0);
	ASSIGN(netSendShieldFx,0x00523670);

	ASSIGN(netCreatePlayerStartPacket,0x004DDA90);

	registerserver("netOnResp",&netOnRespL); /// реакция сервера на ответ клиента
	registerserver("netPointFx",&netPointFx);
	registerserver("netRayFx",&netRayFx);
	registerserver("netShieldFx",&netShieldFx);
	registerserver("netReq",&netDoReq);

	registerserver("netReportCharges",&netReportChargesL);
	registerserver("netClientPrint",&netDoPrintConsoleL);
	registerserver("netFake",&netFake);
	registerserver("sendChat",&sendChat);
	registerclient("netGetVersion",netGetVersion);
	registerclient("netVersionRq",&netVersionRq); /// функция проверки клиентом версии сервера
	registerclient("netSendFx",&netSendFx);
	char Buf[40]="";
	sprintf(Buf,"net%s%s%d","To",Block2,2);/// чтобы не выдавать важную команду всяким ларбосам

	registerclient("netRename",&netRename);
	registerclient(Buf,&sendToServer);
}

