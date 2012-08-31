#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <math.h>
#define REPLAY_VERSION 1
#define REPLAY_PLAYERSTART_SIZE 0x81
typedef unsigned int u_int;


using namespace std;

bool replayIsNewPacket=false;
bool replayIsNewMap=false;
void* origClientPacketHandlerEnd;
void* origClientPacketHandlerPointer;
int origClientPacketHandlerSize;
byte* lastEntry;
int lastEntrySize=0;



// импорты из Нокса
bool (__cdecl *noxReplayStartSave)(const char* filename);
void (__cdecl *noxReplayStopSave)();
bool (__cdecl *noxReplayStartView)(const char* filename);
void (__cdecl *noxReplayStopView)();
void (__cdecl *sub_470A80)();
int *mapFrameTime_6F9838;
int (__cdecl *noxNewSessionStart)();
extern void *(__cdecl *serverGetGameDataBySel)();
extern void *(__cdecl *serverGetGameData)(int N);
extern char *(__cdecl *mapGetName)();
extern void teamCreateDefault(int TeamNumParam, bool notRestrict=false);
extern void (__cdecl *consoleServerMapLoad)(int spaceNumber, int tokensNumber, void* tokens);
extern void (__cdecl *guiServerOptionsStartGameMB)();
extern int (__cdecl *serverModeIndexByModeFlags)(DWORD Flags);
extern short *serverLessonLimitArr;
extern byte *serverTimeLimitArr;
extern int *serverInfoChanged;
extern int (__cdecl *noxCheckGameFlags) (int);
extern void (__cdecl *playerGoObserver)(void* playerPtr, byte unk1, byte unk2);
extern bigUnitStruct* (__cdecl *playerFirstUnit)(); ///возвращает первый юнит сетевого игрока
extern bigUnitStruct* (__cdecl *playerNextUnit)(void* Prev); /// Возвращает следующего сетевого игрока
void* (__cdecl *playerInfoFirst)();
void* (__cdecl *playerInfoNext)(void* Prev);
int (__cdecl *initGame)();
extern DWORD *GameFlags;
extern int (__cdecl *netCreatePlayerStartPacket)(void* Dst, void* playerInfo);
int (__cdecl *deletePlayer)(int netCode);
extern BYTE *(__cdecl *playerInfoFromNetCodeCli) (int NetCode);
void* (__cdecl *netOnPacketRecvCli)(int plrId, void* listPtr, int listSize);
extern int (__cdecl *playerKickByIdx)(int playerIdx, int unknownArg);
extern void* getPlayerUDataFromPlayerIdx(int idx);
extern bigUnitStruct *netUnitByCodeServ(DWORD NetCode);
extern void (__cdecl *noxDeleteObject) (void *Unit);
bigUnitStruct* (__cdecl *unitCreateByThingType)(int thingType);

ServerData replayViewFormGameData;

enum replayEntries
{
	REPLAY_HEADER,
	REPLAY_FRAME,
};

struct replayFileHeader
{
	byte entryType; //default: (byte)REPLAY_HEADER
	u_int fileVersion; //default: REPLAY_VERSION
	u_int fileFramesCount;
	char mapName[8];
	__int16 mapMode;
	__int16 mapFragLimit;
	byte mapTimeLimit;
	byte mapPlayersQuantity;
	// Followed by a dynamic-sized array of playerStarts packets of players on the map
};

struct replayFileFrame
{
	byte entryType; //default: (byte)REPLAY_FRAME
	u_int frameId;
	u_int frameSize;
	// Followed by a dynamic-sized array of bytes with frame data
};

// replayView block



namespace
{
	struct replayViewLoadMapStruct
	{
		char mapName[8];
		__int16 mapMode;
		__int16 mapFragLimit;
		byte mapTimeLimit;
	};
	
	bool replayViewEnabled=false;
	bool replayViewFormGameRequired=false;
	bool replayViewFrameDispatched=false;
	bool replayViewRestoreHoster=false;
	bool replayViewRestoreHosterFinalize=false;
	int replayViewSpeedFactor=0;
	int replayViewSpeedFactorCount=0;
	map<int, int> replayViewFakeNetCodes;
	u_int replayViewFrameSize=0;
	byte* replayViewFrame=NULL;
	u_int replayViewFrameCount=0;
	u_int replayViewSafeSuspendFrames=0;
	replayViewLoadMapStruct replayViewLoadMap;
	byte* replayViewHosterData=NULL;
	ifstream replayView;

	bool replayViewReadData();
	void replayViewKickAllButHoster();

	void replayViewStop()
	{
		*GameFlags=(*GameFlags)|0x1;
		replayViewEnabled=false;
		replayViewFormGameRequired=false;
		replayViewSafeSuspendFrames=0;
		if(replayViewFrame!=NULL)
		{
			delete [] replayViewFrame;
			replayViewFrame=NULL;
			replayViewFrameSize=0;
		}
		replayViewFrameDispatched=false;
		replayView.close();
		replayView.clear();
		replayViewKickAllButHoster();
		replayViewRestoreHoster=true;
		replayViewRestoreHosterFinalize=false;
		replayViewFakeNetCodes.clear();
	}

	bool replayViewStart(const char* fileName)
	{
		replayViewStop();
		replayViewRestoreHoster=false;
		for(byte* P=(byte*)playerInfoFirst(); P!=NULL; P=(byte*)playerInfoNext(P))
		{
			byte playerIdx = *((byte*)(P+0x810));
			if(playerIdx==0x1F)
			{
				replayViewHosterData = new byte[REPLAY_PLAYERSTART_SIZE];
				netCreatePlayerStartPacket(replayViewHosterData, P);
			}
			//REPLAY_PLAYERSTART_SIZE - размер пакета
		}
		if(replayViewHosterData==NULL)
		{
			replayViewStop();
			return false;
		}
		replayView.open(fileName, ios::in | ios::binary);
		if(!replayView.is_open())
		{
			replayViewStop();
			return false;
		}
		char fileType[4];
		replayView.read(fileType, 4);
		if(replayView.eof() && 0!=strncmp(fileType, "bnru", 4))
		{
			replayViewStop();
			return false;
		}
		
		replayViewEnabled=true;
		replayViewSafeSuspendFrames=0;
		return true;
	}

	bool replayViewReadData()
	{
		u_int packetSize=0;
		/*char packetIndicator[6];
		replayView.read(packetIndicator, 5);*/
		replayView.read((char*)&packetSize, sizeof(packetSize));
		if(replayView.eof())
		{
			replayViewStop();
			return false;
		}
		packetSize-=sizeof(packetSize);
		u_int currentPosition=replayView.tellg();
		byte packetEntry;
		replayView.read((char*)&packetEntry, 1);
		if(replayView.eof())
		{
			replayViewStop();
			return false;
		}
		replayView.seekg((u_int)replayView.tellg()-1, ios::beg);
		switch(packetEntry)
		{
		case REPLAY_HEADER:
			{
				replayFileHeader packetHeader;
				replayView.read((char*)&packetHeader, sizeof(packetHeader));
				if(packetHeader.fileVersion>REPLAY_VERSION || replayView.eof())
				{
					// Unsupported replay file version
					replayViewStop();
					return false;
				}
				replayViewFrameCount = packetHeader.fileFramesCount;
				strncpy(replayViewLoadMap.mapName, packetHeader.mapName, 8);
				replayViewLoadMap.mapMode = packetHeader.mapMode;
				replayViewLoadMap.mapFragLimit = packetHeader.mapFragLimit;
				replayViewLoadMap.mapTimeLimit = packetHeader.mapTimeLimit;
				replayViewFrameSize = REPLAY_PLAYERSTART_SIZE*packetHeader.mapPlayersQuantity;
				replayViewFrame = new byte[replayViewFrameSize];
				replayView.read((char*)replayViewFrame, replayViewFrameSize);
				if(replayView.eof())
				{
					replayViewStop();
					return false;
				}
				replayViewFormGameRequired=true;
				replayViewFrameDispatched=true;
			}
			break;
		case REPLAY_FRAME:
			{
				replayFileFrame packetHeader;
				replayView.read((char*)&packetHeader, sizeof(packetHeader));
				if(replayView.eof())
				{
					replayViewStop();
					return false;
				}
				replayViewFrameSize = packetHeader.frameSize;
				replayViewFrameDispatched=true;
				if(replayViewFrameSize>0)
				{
					replayViewFrame = new byte[replayViewFrameSize];
					replayView.read((char*)replayViewFrame, replayViewFrameSize);
					if(replayView.eof())
					{
						replayViewStop();
						return false;
					}
				}
			}
			break;
		default:
			replayView.seekg(currentPosition+packetSize, ios::beg);
		}
		if((currentPosition+packetSize)!=replayView.tellg())
		{
			// File probably corrupted
			replayViewStop();
			return false;
		}
		//replayView.read(packetIndicator, 3);
		return true;
	}

	void replayViewKickAllButHoster()
	{
		void* player=playerFirstUnit();
		while(player!=NULL)
		{
			void **PP=(void **)(((char*)player)+0x2EC);
			PP=(void**)(((char*)*PP)+0x114);
			byte *P=(byte*)(*PP);
			byte playerIdx = *((byte*)(P+0x810));
			if(playerIdx!=0x1F)
			{
				playerKickByIdx((int)playerIdx, 4);
			}
			player=playerNextUnit(player);
		}
	}

	void replayViewLoadFrame(BYTE **frameBuffer, int *size)
	{
		if(replayViewEnabled)
		{
			if(replayViewSafeSuspendFrames==0 && replayViewFormGameRequired==false)
			{
				byte* unitedFrame=NULL;
				u_int unitedSize=0;
				if(replayViewSpeedFactor<0 && replayViewSpeedFactorCount==0)
					replayViewSpeedFactorCount=replayViewSpeedFactor-2;
				if(replayViewSpeedFactorCount<0)
				{
					replayViewSpeedFactorCount++;
					if(replayViewSpeedFactorCount<-1)
						replayViewFrameDispatched=true;
				}
				int speedFactor=replayViewSpeedFactor+1;
				do
				{
					speedFactor--;
					while(replayViewFrameDispatched==false)
					{
						if(!replayViewReadData() || replayViewSafeSuspendFrames>0)
						{
							return;
						}
					}
					replayViewFrameDispatched=false;
					u_int unitedSizeBefore=unitedSize;
					unitedSize+=replayViewFrameSize;
					if(unitedSize>0 && unitedSizeBefore!=unitedSize)
					{
						unitedFrame=(byte*)realloc(unitedFrame, unitedSize);
						memcpy(&unitedFrame[unitedSizeBefore], replayViewFrame, replayViewFrameSize);
					}
				}
				while(speedFactor>0);
				*GameFlags=(*GameFlags)^0x1;
				*size = unitedSize;
				*frameBuffer = unitedFrame;

			}
			else
				replayViewSafeSuspendFrames--;
			*GameFlags=(*GameFlags)|0x1;
		}
		if(replayViewRestoreHoster)
		{
			if(replayViewHosterData!=NULL)
			{
				*size = REPLAY_PLAYERSTART_SIZE;
				*frameBuffer = replayViewHosterData;
				replayViewRestoreHosterFinalize=true;
			}
			replayViewRestoreHoster=false;
		}
	}

	void replayViewFinalizeFrame()
	{
		if(replayViewEnabled && replayViewFrameDispatched==false)
		{
			if(replayViewFrame!=NULL)
			{
				delete[] replayViewFrame;
				replayViewFrame=NULL;
				replayViewFrameSize=0;
			}
		}
		if(replayViewRestoreHosterFinalize)
		{
			delete [] replayViewHosterData;
			replayViewHosterData=NULL;
			replayViewRestoreHosterFinalize=false;
		}
	}

	void replayViewFormGame()
	{
		if(replayViewFormGameRequired && replayViewEnabled)
		{
			*GameFlags=(*GameFlags)|0x1;
			ServerData *Data=(ServerData*)serverGetGameDataBySel();
			strncpy(Data->mapName, replayViewLoadMap.mapName, 8);
			Data->gameFlags = replayViewLoadMap.mapMode|0xe00f;
			Data->fragLimit = replayViewLoadMap.mapFragLimit;
			Data->timeLimitMB = replayViewLoadMap.mapTimeLimit;
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
			replayViewSafeSuspendFrames=30;
			replayViewFormGameRequired=false;
		}
	}

	void replayViewEachFrame()
	{
		
	}

	int replayViewSimulateClient(int gf)
	{
		if((replayViewEnabled && replayViewSafeSuspendFrames==0) || replayViewRestoreHosterFinalize)
		{
			return 0;
		}
		return noxCheckGameFlags(gf);
	}

	int replayViewFakeNetcode(int oldNetCode)
	{
		if(replayViewEnabled)
		{
			int newNetCode=oldNetCode;
			if(replayViewFakeNetCodes.count(oldNetCode)!=0)
				newNetCode = replayViewFakeNetCodes[oldNetCode];
			return newNetCode;
		}
		else
			return oldNetCode;
	}

	int replayViewFakeNetcodeCreate(int thingType, int oldNetCode)
	{
		if(replayViewEnabled && replayViewSafeSuspendFrames==0 && replayViewFormGameRequired==false)
		{
			int newNetCode=oldNetCode;
			if(replayViewFakeNetCodes.count(oldNetCode)!=0)
				newNetCode = replayViewFakeNetCodes[oldNetCode];
			else if(thingType!=0)
			{
				u_int currentPosition=replayView.tellg();
				bigUnitStruct* unit = unitCreateByThingType(thingType);
				newNetCode = unit->netCode;
				replayViewFakeNetCodes[oldNetCode]=newNetCode;
			}
			return newNetCode;
		}
		else
			return oldNetCode;
	}

	int replayViewStopL(lua_State* L)
	{
		replayViewStop();
		return 0;
	}

	int replayViewStartL(lua_State* L)
	{
		if(lua_type(L, -1)==LUA_TSTRING)
			replayViewStart(lua_tostring(L, 1));
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		return 0;
	}

	int replayViewSpeedL(lua_State* L)
	{
		if(lua_type(L, -1)==LUA_TNUMBER)
		{
			int coef = lua_tonumber(L, 1);
			if(coef>=-10 && coef<=10)
			{
				replayViewSpeedFactor=coef;
			}
			else
			{
				lua_pushstring(L,"wrong args: value out of bounds!");
				lua_error_(L);
			}
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		return 0;
	}


	//replaySave block
	bool replaySaveEnabled=false;
	bool replaySaveFrameSaved=false;
	u_int replaySaveDropFrame=0;
	char* replaySaveFileName = NULL;
	byte* replaySaveHeader = NULL;
	u_int replaySaveHeaderSize=0;
	ofstream replaySave;
	u_int replaySaveFrameCount=0;
	u_int replaySavePreviousMapLocation=0;
	byte* replaySaveFramePackets=NULL;
	byte* replaySaveFramePacketsSeeker=NULL;
	byte* replaySaveFramePacketsEnd=NULL;
	byte* replaySaveFramePayload=NULL;
	u_int replaySaveFramePayloadSize=0;
	byte* replaySaveFrameBaseAddress=NULL;
	
	bool replaySaveWriteData(replayEntries type, void* payload=NULL, u_int size=0);

	void replaySaveStop(bool resetFileName=true)
	{
		if(resetFileName && replaySaveFileName!=NULL)
		{
			delete [] replaySaveFileName;
			replaySaveFileName=NULL;
			replaySaveEnabled=false;
		}
		if(replaySave.is_open())
		{
			replaySaveWriteData(REPLAY_HEADER);
		}
		replaySaveFrameCount=0;
		if(replaySaveHeader!=NULL)
		{
			delete [] replaySaveHeader;
			replaySaveHeader=NULL;
		}
		replaySaveDropFrame=0;
		replaySaveHeaderSize=0;
		replaySaveFrameBaseAddress=NULL;
		replaySaveFrameSaved=false;
		replaySave.close();
		replaySave.clear();
	}

	bool replaySaveStart(const char* fileName=NULL, unsigned int fileNameLength=0)
	{
		if(fileNameLength>0)
		{
			if(replaySaveFileName!=NULL)
			{
				replaySaveStop();
			}
			else
				replaySaveStop(!replaySaveEnabled);
			replaySaveFileName = new char[fileNameLength+1];
			memset(replaySaveFileName, 0, fileNameLength+1);
			strncpy(replaySaveFileName, fileName, fileNameLength);
		}
		else
			replaySaveStop(!replaySaveEnabled);
		string fileNameFull="replays/";
		TIME_ZONE_INFORMATION tz;
		SYSTEMTIME time;
		GetLocalTime(&time);
		GetTimeZoneInformation(&tz);
		char timeZone[7];
		if(tz.Bias==0)
			strcpy(timeZone, "Z");
		else
		{
			char symbol=(tz.Bias>0)?('-'):('+');
			u_int tzHours = floor((long double)(abs(tz.Bias)/60));
			u_int tzMinutes = abs(tz.Bias)%60;
			sprintf(timeZone, "%c%02u.%02u", symbol, tzHours, tzMinutes);
		}
		char timeStamp[30];
		// Почти ISO 8601 (к сожалению двоеточия в имени файлов не допустимы)
		sprintf(timeStamp, "%04i-%02i-%02iT%02i.%02i.%02i.%03i%s", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, timeZone);
		if(strlen(replaySaveFileName)>0)
		{
			fileNameFull.append(replaySaveFileName);
			fileNameFull.append("_");
		}
		fileNameFull.append(timeStamp);
		fileNameFull.append("_");
		fileNameFull.append(mapGetName());
		fileNameFull.append(".nru");
		replaySave.open(fileNameFull.c_str(), ios::out | ios::trunc | ios::binary);
		if(!replaySave.is_open())
		{
			replaySaveStop();
			return false;
		}
		replaySave.write("bnru", 4); // Ставим метку формата, заодно тестируем, работает ли запись
		if(!replaySave.is_open())
		{
			replaySaveStop();
			return false;
		}
		replaySaveEnabled=true;
		replaySaveFrameCount=0;
		replaySaveWriteData(REPLAY_HEADER);
		return true;
	}

	void replaySaveWriteFrameCount()
	{
		u_int currentPosition=replaySave.tellp();
		u_int offset=4+1+sizeof(u_int);
		replaySave.seekp(offset, ios::beg);
		replaySave.write((const char*)&replaySaveFrameCount, sizeof(u_int));
		replaySave.seekp(currentPosition, ios::beg);
	}


	bool replaySaveWriteData(replayEntries type, void* payload, u_int size)
	{
		if(!replaySave.is_open())
		{
			replaySaveStop();
			return false;
		}

		
		u_int totalLength=0;
		byte* packet;
		switch(type)
		{
		case REPLAY_HEADER:
			{
				// Готовим данные. Для определения размеров
				if(replaySaveHeader==NULL)
				{
					byte* playersList=NULL;
					byte playersListQuantity=0;
					for(void* P=playerInfoFirst(); P!=NULL; P=playerInfoNext(P))
					{
						playersListQuantity+=1;
						playersList=(byte*)realloc(playersList, playersListQuantity*REPLAY_PLAYERSTART_SIZE);
						netCreatePlayerStartPacket(&playersList[(playersListQuantity-1)*REPLAY_PLAYERSTART_SIZE], P);
						//REPLAY_PLAYERSTART_SIZE - размер пакета
					}

					// Формируем заголовок пакета
					replayFileHeader* packetHelper = new replayFileHeader;
					memset(packetHelper, 0, sizeof(packetHelper));
					packetHelper->entryType=REPLAY_HEADER;
					packetHelper->fileVersion=REPLAY_VERSION;
					packetHelper->fileFramesCount=replaySaveFrameCount;
					strncpy(packetHelper->mapName, mapGetName(),8);
					packetHelper->mapMode=(__int16)*GameFlags&0x1FF0;
					int Index=serverModeIndexByModeFlags(*GameFlags);
					packetHelper->mapFragLimit=serverLessonLimitArr[Index];
					packetHelper->mapTimeLimit=serverTimeLimitArr[Index];
					packetHelper->mapPlayersQuantity=playersListQuantity;
					replaySaveHeaderSize=sizeof(replayFileHeader)+playersListQuantity*REPLAY_PLAYERSTART_SIZE;
					replaySaveHeader = new byte[replaySaveHeaderSize];
					memcpy(replaySaveHeader, packetHelper, sizeof(replayFileHeader));
					memcpy(&replaySaveHeader[sizeof(replayFileHeader)], playersList, playersListQuantity*REPLAY_PLAYERSTART_SIZE);
					// Зачистка временных данных
					realloc(playersList, 0);
					delete [] packetHelper;
				}
				else
				{
					replayFileHeader* packetHelper = (replayFileHeader*)replaySaveHeader;
					packetHelper->fileFramesCount=replaySaveFrameCount;
				}

				// Подготовка данных завершена. Формируем сам пакет
				packet=new byte[replaySaveHeaderSize];
				memset(packet, 0, replaySaveHeaderSize);
				memcpy(packet, replaySaveHeader, replaySaveHeaderSize);
				totalLength+=replaySaveHeaderSize; // финальный размер пакета

				
			}
			break;
		case REPLAY_FRAME:
			{
				// Формируем заголовок пакета
				replayFileFrame* packetHelper = new replayFileFrame;
				memset(packetHelper, 0, sizeof(packetHelper));
				packetHelper->entryType=REPLAY_FRAME;
				packetHelper->frameId=replaySaveFrameCount;
				packetHelper->frameSize = size;

				// Подготовка данных завершена. Формируем сам пакет
				packet=new byte[sizeof(replayFileFrame)+size];
				memset(packet, 0, sizeof(replayFileFrame)+size);
				memcpy(packet, packetHelper, sizeof(replayFileFrame));
				memcpy(&packet[sizeof(replayFileFrame)], payload, size);
				totalLength+=sizeof(replayFileFrame)+size;

				// Зачистка временных данных
				delete [] packetHelper;
			}
			break;
		}
		u_int relocate=0;
		if(type==REPLAY_HEADER)
		{
			relocate=replaySave.tellp();
			if(relocate>4)
				replaySave.seekp(4, ios::beg);
		}
		/*char packetIndicator[6];
		strncpy(packetIndicator,"start", 5);
		replaySave.write(packetIndicator, 5);*/
		totalLength+=sizeof(totalLength);
		replaySave.write((const char*)&totalLength, sizeof(totalLength));
		replaySave.write((const char*)packet, totalLength-sizeof(totalLength));
		/*strncpy(packetIndicator,"end", 3);
		replaySave.write(packetIndicator, 3);*/
		if(type==REPLAY_HEADER && relocate>4)
		{
			replaySave.seekp(relocate, ios::beg);
		}
		delete [] packet;
	}

	void replaySaveAddFramePayload(byte* payload, int size)
	{
		if(size>0)
		{
			replaySaveFramePayload=(byte*)realloc(replaySaveFramePayload, replaySaveFramePayloadSize+size);
			byte* pointer=replaySaveFramePayload+replaySaveFramePayloadSize;
			memset(pointer, 0, size);
			memcpy(pointer, payload, size);
		}
		replaySaveFramePayloadSize+=size;
	}

	bool replaySaveFullFrame(BYTE *BufStart, int size)
	{
		if(replaySaveEnabled)
		{
			// Сохраняем весь фрейм в буффер, потом от него будем отрезать попакетно. Заодно сейвим в файл инфу о новом фрейме. Нужно чтоб в реплей не попали "лишние" пакеты.
			if(size>0)
			{
				replaySaveFramePackets = new byte[size];
				memcpy(replaySaveFramePackets, BufStart, size);
			}
			replaySaveFramePacketsSeeker=replaySaveFramePackets;
			replaySaveFramePacketsEnd=replaySaveFramePackets+size;
			replaySaveFrameCount++;
			replaySaveFrameBaseAddress=BufStart;
			
			return true;
		}
		return false;
	}

	bool replaySavePacketChecker(byte* packet, int size)
	{
		// Здесь мы просто проверим, пихать ли этот пакет в реплей или ну его нафиг.
		byte *P = packet;
		bool allowed=true; //Если пакет проверку не проходит - false
		// Тут осуществлять проверку пакета на вносимость в реплей. На коды пакетов.
		if(*P==0x2B)
		{
			// Обработка смены мапы
			allowed=false;
			replaySaveDropFrame=5; // Дропаем фреймы, что на стыке двух файлов
			replaySaveStart(); //Рестарт процесса происходит (для записи в новый файл). Заметьте - без стопа. Стоп обрабатывается в самом начале replaySaveStart. Эксплиситный стоп нуже для финализации всего процесса.
		}
		if(*P==0x6C)
		{
			allowed=false;
		}
		if(*P==0x6D)
		{
			allowed=false;
		}
		if(allowed)
		{
			replaySaveAddFramePayload(packet, size);
		}
		return true;
	}

	bool replaySavePacketHandler(BYTE *&BufStart, BYTE *&E, bool &found)
	{
		if(replaySaveEnabled)
		{
			// Анализ пакетов - отрезаем по-пакетно от общего буфера фрейма. BufStart после анализа пакета самим обработчиком сдвигается вперёд, что позволяет нам засечь пакет.
			u_int size=BufStart-replaySaveFrameBaseAddress;
			if(size>0 && (replaySaveFramePacketsSeeker+size)<=replaySaveFramePacketsEnd)
			{
				byte* packet = new byte[size];
				memcpy(packet, replaySaveFramePacketsSeeker, size);
				replaySavePacketChecker(packet, size);
				delete [] packet;
				replaySaveFramePacketsSeeker+=size;
			}
			replaySaveFrameBaseAddress=BufStart;
			return true;
		}
		return false;
	}

	bool replaySaveFinalizeFrame()
	{
		if(replaySaveEnabled)
		{
			// Анализ последнего пакета - отрезаем по-пакетно. Вызов в самм конце обработчика пакетов.
			if(replaySaveFramePacketsSeeker<=replaySaveFramePacketsEnd)
			{
				u_int size=replaySaveFramePacketsEnd-replaySaveFramePacketsSeeker;
				replaySaveFrameBaseAddress=NULL;
				if(size>0)
				{
					byte* packet = new byte[size];
					memcpy(packet, replaySaveFramePacketsSeeker, size);
					replaySaveFramePacketsSeeker=replaySaveFramePacketsEnd;
					replaySavePacketChecker(packet, size);
				}
				// Финализируем фрейм (пишем в файл) - если не надо дропать фрейм. Фрейм дропается на стыке между двумя файлами.
				if(replaySaveDropFrame==0)
					replaySaveWriteData(REPLAY_FRAME, replaySaveFramePayload, replaySaveFramePayloadSize);
				else
					replaySaveDropFrame--;
				replaySaveFrameSaved = true;
				// Зачищаем
				replaySaveFramePayloadSize=0;
				if(replaySaveFramePayload!=NULL)
				{
					realloc(replaySaveFramePayload, 0);
					replaySaveFramePayload=NULL;
				}
				if(replaySaveFramePackets!=NULL)
				{
					delete [] replaySaveFramePackets;
					replaySaveFramePackets=NULL;
				}
				replaySaveFramePacketsSeeker=NULL;
				replaySaveFramePacketsEnd=NULL;
			}
			return true;
		}
		return false;
	}

	void replaySaveZeroFrame()
	{
		if(replaySaveEnabled && replaySaveDropFrame==0)
		{
			if(replaySaveFrameSaved)
			{
				replaySaveFrameSaved=false;
			}
			else
			{
				replaySaveWriteData(REPLAY_FRAME);
			}
		}
	}

	int replaySaveStopL(lua_State* L) //обёртка для ЛУА
	{
		replaySaveStop();
		return 0;
	}

	int replaySaveStartL(lua_State* L) //обёртка для ЛУА
	{
		if(lua_type(L, -1)==LUA_TSTRING)
		{
			const char *S=lua_tostring(L,-1);
			if(replaySaveStart(S, strlen(S)))
				return 0;
			else
			{
				lua_pushstring(L,"file save error!");
				lua_error_(L);
			}
		}
		else
		{
			if(replaySaveStart())
				return 0;
			else
			{
				lua_pushstring(L,"file save error!");
				lua_error_(L);
			}
		}
		return 0;
	}

	//common replay block

	//Самый конец netOnPacketRecvCli. 
	void clientPacketHandlerEnd(void* BufStart, int size, int edx, int ecx, int eax, void* returnFunction)
	{
		returnFunction = origClientPacketHandlerEnd;
		BufStart = origClientPacketHandlerPointer;
		size = origClientPacketHandlerSize;
		replaySaveFinalizeFrame();
		replayViewFinalizeFrame();
	}

	//internal - что-то из области чёрной магии. Сам уже не помню что.
	void __declspec(naked) clientPacketHandlerEndTrap()
	{
		__asm
		{
			push 0
			push eax
			push ecx
			push edx
			push 0
			push 0
			call clientPacketHandlerEnd
			pop edx
			pop ecx
			mov [esp+16+4], edx
			mov [esp+16+4+4], ecx
			pop edx
			pop ecx
			pop eax
			ret
		}
	}

	// Самое начало netOnPacketRecvCli. Адрес возврата, начало обрабатываемого буфера и размер
	void* __cdecl replayInterceptClientPackets(void* noxOrigClientPacketHandlerEnd, BYTE *BufStart, int size) //Внимание! Модификация переменных функции приведёт к изменению оных в onPacketRecvCli!
	{
		origClientPacketHandlerEnd=noxOrigClientPacketHandlerEnd; // Здесь адрес старого return-а.
		origClientPacketHandlerPointer=BufStart;
		origClientPacketHandlerSize=size;
		replaySaveFullFrame(BufStart, size);
		replayViewLoadFrame(&BufStart, &size);
		return &clientPacketHandlerEndTrap; // Подмена return-а из onNetPacketCli.
	}

	//internal
	void __cdecl packetHandlerBegin()
	{
		//replayIsNewPacket=true;
		sub_470A80();
	}

	//internal - тож чёрная магия.
	void __declspec(naked) clientPacketHandlerBeginTrap()
	{
		__asm
		{
			push eax
			push ebp
			mov eax, [esp+0x1748+4+8] // Вытаскиваем адрес стандартного return-а
			push eax
			call replayInterceptClientPackets
			mov [esp+0x1748+4+8+4], eax // Подмена return-а из onNetPacketCli.
			pop eax
			pop ebp
			pop eax
			lea ebx, [eax+ebp]
			mov [esp+0x1748+4-0x1704], ebx //Последняя позиция в стеке перед call - 1748h. Позиция в стеке переменной - -1704h. Соотв. +4 (адрес возврата по ret)
			mov [esp+0x1748+4+0x8], ebp //Аналогично предыдущему
			mov [esp+0x1748+4+0xC], eax	//Аналогично предыдущему
			call packetHandlerBegin
			ret
		}
	}
}
void replayGuiUpdate()
{
	replaySaveZeroFrame();
	replayViewFormGame();
}

//onNetPacket для реплеев.
bool replayPacketHandler(BYTE *&BufStart, BYTE *&E, bool &found)
{
	replaySavePacketHandler(BufStart, E, found);
	return true;
}

void __cdecl replayQuitGame()
{
	replaySaveStop();
	replayViewStop();
	//
	noxReplayStopSave();
}

void replayEachFrame()
{
	replayViewEachFrame();
}

int __cdecl replayOnSpriteRequest(int oldNetCode)
{
	//return replayViewFakeNetcode(oldNetCode);
	return oldNetCode;
}

int __cdecl replayOnSpriteCreate(int thingType, int oldNetCode)
{
	//return replayViewFakeNetcodeCreate(thingType, oldNetCode);
	return oldNetCode;
}

void __declspec(naked) replayOnSpriteRequestTrap()
{
	__asm
	{
		mov eax, [esp+4]
		push eax
		call replayOnSpriteRequest
		add esp, 4
		mov [esp+4], eax
		mov eax, 0x006D3DC0
		mov eax, [eax]
		test eax,eax
		push 0x0045A6F7
		ret
	}
}

void __declspec(naked) replayOnSpriteCreateTrap()
{
	__asm
	{
		push esi
		mov esi, [esp+8+8]
		push eax
		mov eax, [esp+8+4+4]
		push esi
		push eax
		call replayOnSpriteCreate
		add esp,8
		mov esi,eax
		pop eax
		mov [esp+8+8],esi
		push 0x0048E976
		ret
	}
}

void replayNullSub()
{
	return;
}

extern "C" void replayInit(lua_State *L);


extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);

void replayInit(lua_State *L)
{
	/*
	//Blocked for now
	ASSIGN(noxReplayStartSave, 0x004D3370);
	ASSIGN(noxReplayStopSave, 0x004D33B0);
	ASSIGN(noxReplayStartView, 0x004D34C0);
	ASSIGN(noxReplayStopView, 0x004D3530);
	
	ASSIGN(sub_470A80, 0x00470A80);

	ASSIGN(mapFrameTime_6F9838, 0x006F9838);
	//ASSIGN(initGame, 0x00435CC0);
	ASSIGN(deletePlayer, 0x004DE7C0);

	ASSIGN(playerInfoFirst, 0x00416EA0);
	ASSIGN(playerInfoNext, 0x00416EE0);

	ASSIGN(netOnPacketRecvCli, 0x0048EA70);

	ASSIGN(unitCreateByThingType, 0x004E3450);
	
	InjectOffs(0x0048EA9B+1, clientPacketHandlerBeginTrap);
	//InjectOffs(0x004018EF+1, replayStartSave);
	InjectOffs(0x004D3219+1, replayQuitGame);
	
	InjectOffs(0x004908CF+1, replayViewSimulateClient);
	InjectOffs(0x00491D63+1, replayViewSimulateClient);
	//InjectOffs(0x0048EB87+1, replayViewSimulateClient);
	//InjectOffs(0x0048EBAA+1, replayViewSimulateClient);
	InjectOffs(0x0048F2E7+1, replayViewSimulateClient);
	InjectOffs(0x0048F46A+1, replayViewSimulateClient);
	InjectOffs(0x0048F694+1, replayViewSimulateClient);
	//InjectOffs(0x0048FA27+1, replayViewSimulateClient);
	//InjectOffs(0x0048FA49+1, replayViewSimulateClient);
	InjectOffs(0x0048FC39+1, replayViewSimulateClient);
	InjectOffs(0x0048FC67+1, replayViewSimulateClient);
	InjectOffs(0x0048FCB5+1, replayViewSimulateClient);
	InjectOffs(0x004903C6+1, replayViewSimulateClient);
	InjectOffs(0x004908CF+1, replayViewSimulateClient);
	InjectOffs(0x00490A1B+1, replayViewSimulateClient);
	InjectOffs(0x00490D39+1, replayViewSimulateClient);
	InjectOffs(0x00490E23+1, replayViewSimulateClient);
	InjectOffs(0x00490F9F+1, replayViewSimulateClient);
	InjectOffs(0x0049127A+1, replayViewSimulateClient);
	InjectOffs(0x00491592+1, replayViewSimulateClient);
	//InjectOffs(0x00491A3A+1, replayViewSimulateClient);
	InjectOffs(0x0049302F+1, replayViewSimulateClient);
	//InjectOffs(0x00493B3A+1, replayViewSimulateClient);
	InjectOffs(0x00493BB5+1, replayViewSimulateClient);
	
	InjectOffs(0x0048E9F9+1, replayNullSub);*/
/*	//Blocked permanently
	InjectOffs(0x00491D63+1, replayViewSimulateClient);

	InjectOffs(0x00409DE6+1, replaySaveNewMapLoading);
	InjectOffs(0x00491EED+1, replaySavePlayerLeaving);

*/

	//registerserver("replayStartSave",&replayStartSaveL);
	//registerserver("replayStopSave",&replayStopSaveL);
	
	
/* //Blocked for now
	registerserver("replayViewStart",&replayViewStartL);
	registerserver("replayViewStop",&replayViewStopL);
	registerserver("replayViewSpeed",&replayViewSpeedL);

	registerclient("replaySaveStart",&replaySaveStartL);
	registerclient("replaySaveStop",&replaySaveStopL);


	//registerserver("replayStopView",&replayStopViewL);


	InjectJumpTo(0x0045A6F0, &replayOnSpriteRequestTrap);
	InjectJumpTo(0x0048E971, &replayOnSpriteCreateTrap);*/
}