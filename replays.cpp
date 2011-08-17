#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

bool (__cdecl *noxReplayStartSave)(const char* filename);
void (__cdecl *noxReplayStopSave)();
bool (__cdecl *noxReplayStartView)(const char* filename);
void (__cdecl *noxReplayStopView)();

void (__cdecl *sub_470A80)();

bool replayIsNewPacket=false;
bool replayIsNewMap=false;

map<__int16, BYTE*> replayMembers;


int *mapFrameTime_6F9838;
byte replayNewMapPacket[0x29];
ofstream replaySave;
byte currentReplay=0;
char* defaultName=".//replays//lastReplay%i.nru";

bool replaySaveEnabled=false;
bool replaySaveRequired=false;

bool replayViewEnabled=false;

int (__cdecl *noxNewSessionStart)();

byte* packet=NULL;

ifstream replayView;


extern void *(__cdecl *serverGetGameDataBySel)();
extern void *(__cdecl *serverGetGameData)(int N);

int framesNotOverriden=0;

extern char *(__cdecl *mapGetName)();

extern void teamCreateDefault(int TeamNumParam, bool notRestrict=false);
extern void (__cdecl *consoleServerMapLoad)(int spaceNumber, int tokensNumber, void* tokens);
extern void (__cdecl *guiServerOptionsStartGameMB)();
extern int (__cdecl *serverModeIndexByModeFlags)(DWORD Flags);
extern short *serverLessonLimitArr;
extern byte *serverTimeLimitArr;
extern int *serverInfoChanged;

extern int (__cdecl *noxCheckGameFlags) (int);

bool replayFormGameRequest=false;

extern void (__cdecl *playerGoObserver)(void* playerPtr, byte unk1, byte unk2);

extern bigUnitStruct* (__cdecl *playerFirstUnit)(); ///возвращает первый юнит сетевого игрока
extern bigUnitStruct* (__cdecl *playerNextUnit)(void* Prev); /// Возвращает следующего сетевого игрока


void* (__cdecl *playerInfoFirst)();
void* (__cdecl *playerInfoNext)(void* Prev);


int (__cdecl *initGame)();

extern DWORD *GameFlags;
extern char *(__cdecl *mapGetName)();

extern int (__cdecl *netCreatePlayerStartPacket)(void* Dst, void* playerInfo);

int (__cdecl *deletePlayer)(int netCode);

extern BYTE *(__cdecl *playerInfoFromNetCodeCli) (int NetCode);
bool replayViewFakeClient=false;

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

void replayViewEOF()
{
	replayView.close();
	replayView.clear();
	//*GameFlags=*GameFlags|0x1;
	replayViewEnabled=false;
	replayViewFakeClient=false;
}

void replayFormGame()
{
	unsigned int length=0;
	replayView.read((char*)&length, 4);
	if(replayView.eof())
	{
		replayViewEOF();
		return;
	}
	ServerData *Data=(ServerData*)serverGetGameDataBySel();
	replayView.read((char*)Data, length);
	if(replayView.eof())
	{
		replayViewEOF();
		return;
	}
	bool sameMap=false;
	if(strcmp(Data->mapName,mapGetName())==0)
	{
		sameMap=true;
	}

	int Index=serverModeIndexByModeFlags(Data->gameFlags);
	serverLessonLimitArr[Index]=Data->fragLimit;
	serverTimeLimitArr[Index]=Data->timeLimitMB;
	*serverInfoChanged=1;

	if(Data->isNew!=1 && Data->gameFlags&0x60)
	{
		teamCreateDefault(2);
	}
	else
	{
		teamCreateDefault(-1);
	}
	if(sameMap)
	{
		wchar_t command[5] = L"load";
		wchar_t wMapName[9]={0};
		int *commandPointer=(int*)&command;
		int *wMapNamePointer=(int*)&wMapName;
		MultiByteToWideChar(CP_ACP, MB_COMPOSITE, Data->mapName, 8, wMapName, 9);
		BYTE result[8]={0};
		memcpy(&result[0], &commandPointer, 4);
		memcpy(&result[4], &wMapNamePointer, 4);
		consoleServerMapLoad(1, 2, (void*)&result);
	}
	else
	{
		guiServerOptionsStartGameMB();
	}
}

namespace
{

	int __cdecl replayStartSave()
	{
		if(replaySaveRequired)
		{
			char *filename = new char[strlen(defaultName)+4];
			sprintf(filename, defaultName, (int)currentReplay);
			replaySave.clear();
			replaySave.open(filename, ios::out | ios::trunc | ios::binary);
			replaySaveEnabled=true;
			
			ServerData *Data=(ServerData*)serverGetGameDataBySel();
			if(((__int16)*GameFlags)&0x80)
			{
				Data->isNew=1;
				Data->gameFlags=Data->gameFlags&0xE00F;
				Data->gameFlags=Data->gameFlags|0x80;
			}
			strncpy(Data->mapName, mapGetName(), 8);
			unsigned int length = sizeof(*Data);
			replaySave.write((const char*)&length, 4);
			replaySave.write((const char*)Data, length);
			Data->isNew=0;
			length = replayMembers.size()*0x81;
			replaySave.write((const char*)&length, 4);
			for(map<__int16, BYTE*>::const_iterator iter = replayMembers.begin(); iter != replayMembers.end(); ++iter)
			{
				replaySave.write((const char*)iter->second, 0x81);
			}
		}
		return initGame();
	}

	void __cdecl replayStopSave()
	{
		if(replaySaveEnabled)
		{
			replaySaveEnabled=false;
			replaySave.close();
			replaySave.clear();
		}
		noxReplayStopSave();
	}

	void __cdecl replayLoadFromFile(BYTE* BufStart, int size) //Внимание! Модификация переменных функции приведёт к изменению оных в onPacketRecvCli!
	{
		//BufStart++;
		if(replaySaveEnabled && size==0) //Дада, Нокс не каждый кадр посылает файло. И об этом надо помнить... Придётся писать нуль сюда
		{
			unsigned int length=0;
			replaySave.write((const char*)&length, 4);
		}
		if(replayViewEnabled)
		{
			if(packet!=NULL)
			{
				delete [] packet;
				packet=NULL;
			}
			if(framesNotOverriden==0)
			{
				replayViewFakeClient=true;
				unsigned int packetLength=0;
				replayView.read((char*)&packetLength, 4);
				if(replayView.eof())
				{
					replayViewEOF();
					return;
				}
				packet = new byte[packetLength];
				replayView.read((char*)packet, packetLength);
				if(replayView.eof())
				{
					replayViewEOF();
					return;
				}
				BufStart = packet;
				size = packetLength;
			}
			else
				framesNotOverriden--;
		}
	}

	void __cdecl replayNotifyNewPacket()
	{
		replayIsNewPacket=true;
		sub_470A80();
	}

	void __declspec(naked) replayTrackParse()
	{
		__asm
		{
			push eax
			push ebp
			call replayLoadFromFile
			pop ebp
			pop eax
			lea ebx, [eax+ebp]
			mov [esp+0x1748+4-0x1704], ebx //Последняя позиция в стеке перед call - 1748h. Позиция в стеке переменной - -1704h. Соотв. +4 (адрес возврата по ret)
			mov [esp+0x1748+4+0x8], ebp //Аналогично предыдущему
			mov [esp+0x1748+4+0xC], eax	//Аналогично предыдущему
			call replayNotifyNewPacket
			ret
		}
	}

/*	int replayStartSaveL(lua_State *L)
	{
		if (lua_type(L,-1)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		else
		{
			if(noxReplayStartSave(lua_tostring(L, -1)))
			{
				lua_pushstring(L, "ok");
				return 1;
			}
			else
			{
				lua_pushstring(L, "start error");
				lua_error_(L);
			}
		}
		return 0;
	}

	int replayStopSaveL(lua_State *L)
	{
		replayStopSave();
		lua_pushstring(L, "ok");
		return 1;
	}

	int replayStopViewL(lua_State *L)
	{
		noxReplayStopView();
		lua_pushstring(L, "ok");
		return 1;
	}*/

	int replayStartViewL(lua_State *L)
	{
		if (lua_type(L,-1)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		char* defaultPath=".\\replays\\%s";
		char Buffer[0xFF];
		sprintf(Buffer, defaultPath, lua_tostring(L, -1));
		replayView.clear();
		replayView.open(Buffer, ios::in | ios::binary);
		if(replayView.is_open())
		{
			
			replayView.seekg(0, ios::end);
			size_t filesize=replayView.tellg();
			replayView.seekg(0, ios::beg);
			replayViewEnabled=true;
			//*GameFlags=*GameFlags&0x1;
			replayFormGameRequest=true;
			framesNotOverriden=30*2;//2 секунды (2 по 30 фреймов) + фрейм на удаление игрока.
			/*for(void* P=playerFirstUnit(); P!=0; P=playerNextUnit(P))
			{
				BYTE *B=(BYTE *)P;
				B+=0x2EC;//контроллер
				B=*((BYTE**)B);
				B+=0x114;//плэеринфо?
				B=*((BYTE**)B);
				playerGoObserver(B, 0, 1);
			}*/
		}
		return 0;
	}

	void __cdecl replayLoadSavePlayerInfo(void* playerInfo, int netCode, void* packetBuf)
	{
		
		if(replaySaveRequired)
		{
			BYTE* newPlayerPacket = new BYTE[0x81];
			memcpy(newPlayerPacket, packetBuf, 0x81);
			replayMembers.insert(pair<__int16, BYTE*>((__int16)netCode, newPlayerPacket));
		}
	}
	
	int loadNewPlayerPacket(int gf)
	{
		if(replayViewFakeClient && gf&0x1)
		{
			return 0;
		}
		return noxCheckGameFlags(gf);
	}

	int __declspec(naked) replayNewPlayerPopulate(int gf)
	{
		__asm
		{
			push ebp
			push edx
			push eax
			call replayLoadSavePlayerInfo
			add esp,0xC
			mov eax, [esp+4]
			push eax
			call loadNewPlayerPacket
			add esp,4
			ret
		}
	}
	
	byte* prevPacket;
	int prevPacketSize;

	void replayReadPackets(BYTE *BufStart, BYTE *E)
	{
		
		
		void *P=playerInfoFirst();
		while(P!=0)
		{
			BYTE *B=(BYTE *)P;
			bool isObserver = (*((int*)(B+0xE60)))&0x1;
			if(isObserver==0)
				return;
			P=playerInfoNext(P);
		}
		delete [] prevPacket;
		prevPacket = new byte[E-BufStart+1];
		prevPacketSize = E-BufStart;
		memcpy(prevPacket, BufStart, E-BufStart);
		return;
	}
}

bool replaySavePackets(BYTE *BufStart, BYTE *E)
{
	/*ofstream file("packetlist", ios::out | ios::app | ios::ate | ios::binary);
	BYTE *P=BufStart;
	file.write((const char*)P, 1);
	file.close();*/
	replayReadPackets(BufStart, E);
	BYTE *P=BufStart;
	if (*P==0x2E)
	{
		P++;
		delete [] replayMembers[(__int16)*P];
		replayMembers.erase((__int16)*P);
	}
	if(replaySaveEnabled)
	{
		/*
		ofstream file("packetlist", ios::out | ios::app | ios::ate | ios::binary);
		file.write((const char*)P, 1);
		file.close();*/
		P=BufStart;
		if(*P==0x2B)
		{
			P=P+0x25;
			if((int)*P>*mapFrameTime_6F9838)
			{
				replayIsNewMap=true;
				memcpy(replayNewMapPacket, BufStart, 0x29);
				replaySaveEnabled=false;
				replaySave.close();
				replaySave.clear();
				currentReplay++;
				char *filename = new char[strlen(defaultName)+4];
				sprintf(filename, defaultName, (int)currentReplay);
				replaySave.clear();
				replaySave.open(filename, ios::out | ios::trunc | ios::binary);
				delete [] filename;
				
				
				
				
				ServerData *Data=(ServerData*)serverGetGameDataBySel();
				if(((__int16)*GameFlags)&0x80)
				{
					Data->isNew=1;
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x80;
				}
				strncpy(Data->mapName, mapGetName(), 8);
				unsigned int length = sizeof(*Data);
				replaySave.write((const char*)&length, 4);
				replaySave.write((const char*)Data, length);
				Data->isNew=0;
				length = replayMembers.size()*0x81;
				replaySave.write((const char*)&length, 4);
				for(map<__int16, BYTE*>::const_iterator iter = replayMembers.begin(); iter != replayMembers.end(); ++iter)
				{
					replaySave.write((const char*)iter->second, 0x81);
				}

				replaySaveEnabled=true;
			}
		}
		if(replayIsNewPacket)
		{
			replayIsNewPacket=false;
			unsigned int length = (int)(E-BufStart);
			replaySave.write((const char*)&length, 4);
			replaySave.write((const char*)BufStart, (int)(E-BufStart));
			// Тут пишем в LastReplay предыдущий пакет, а текущий сейвим во временное хранилище. Чтобы отслеживать новую мапу.
			return true;
		}
	}
	return false;
}


extern "C" void replayInit(lua_State *L);


extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);

void replayCommandLineSave()
{
	replaySaveRequired=true;
}


void replayInit(lua_State *L)
{
	ASSIGN(noxReplayStartSave, 0x004D3370);
	ASSIGN(noxReplayStopSave, 0x004D33B0);
	ASSIGN(noxReplayStartView, 0x004D34C0);
	ASSIGN(noxReplayStopView, 0x004D3530);
	
	ASSIGN(sub_470A80, 0x00470A80);

	ASSIGN(mapFrameTime_6F9838, 0x006F9838);
	ASSIGN(initGame, 0x00435CC0);
	ASSIGN(deletePlayer, 0x004DE7C0);

	ASSIGN(playerInfoFirst, 0x00416EA0);
	ASSIGN(playerInfoNext, 0x00416EE0);
	
	InjectOffs(0x0048EA9B+1, replayTrackParse);
	InjectOffs(0x004018EF+1, replayStartSave);
	InjectOffs(0x004D3219+1, replayStopSave);
	InjectOffs(0x00491D63+1, replayNewPlayerPopulate);

	//InjectOffs(0x004908CF+1, loadNewPlayerPacket);
	//InjectOffs(0x0048EB87+1, loadNewPlayerPacket);
	//InjectOffs(0x0048EBAA+1, loadNewPlayerPacket);
	InjectOffs(0x0048F2E7+1, loadNewPlayerPacket);
	InjectOffs(0x0048F46A+1, loadNewPlayerPacket);
	InjectOffs(0x0048F694+1, loadNewPlayerPacket);
	//InjectOffs(0x0048FA27+1, loadNewPlayerPacket);
	//InjectOffs(0x0048FA49+1, loadNewPlayerPacket);
	InjectOffs(0x0048FC39+1, loadNewPlayerPacket);
	InjectOffs(0x0048FC67+1, loadNewPlayerPacket);
	InjectOffs(0x0048FCB5+1, loadNewPlayerPacket);
	InjectOffs(0x004903C6+1, loadNewPlayerPacket);
	InjectOffs(0x004908CF+1, loadNewPlayerPacket);
	InjectOffs(0x00490A1B+1, loadNewPlayerPacket);
	InjectOffs(0x00490D39+1, loadNewPlayerPacket);
	InjectOffs(0x00490E23+1, loadNewPlayerPacket);
	InjectOffs(0x00490F9F+1, loadNewPlayerPacket);
	InjectOffs(0x0049127A+1, loadNewPlayerPacket);
	InjectOffs(0x00491592+1, loadNewPlayerPacket);
	//InjectOffs(0x00491A3A+1, loadNewPlayerPacket);
	InjectOffs(0x0049302F+1, loadNewPlayerPacket);
	//InjectOffs(0x00493B3A+1, loadNewPlayerPacket);
	InjectOffs(0x00493BB5+1, loadNewPlayerPacket);

	//registerserver("replayStartSave",&replayStartSaveL);
	//registerserver("replayStopSave",&replayStopSaveL);
	registerserver("replayStartView",&replayStartViewL);
	//registerserver("replayStopView",&replayStopViewL);
}