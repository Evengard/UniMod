#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
//:004317B0 configLoad(const char*)
//00433290 configSave
extern void replayGuiUpdate();
extern bool serverStart(int port);
extern void serverClose();
extern char *(__cdecl *mapGetName)();
extern DWORD *GameFlags;
extern void tputs(int s,const char *S);
extern int tprintf(int s,const char *Format,...);
extern void send_headers(int f, int status, char *title, char *extra, char *mime, 
                  int length, time_t date);
extern void send_error(int f, int status, char *title, char *extra, char *text);

void (__cdecl *guiServerOptionsStartGameMB)();
int (__cdecl *playerKickByIdx)(int playerIdx, int unknownArg);
void (__cdecl *consoleServerMapLoad)(int spaceNumber, int tokensNumber, void* tokens);
int (__cdecl *mapLoadFromFile)(void* mapName);
void *(__cdecl *serverGetGameData)(int N);
void *(__cdecl *serverGetGameDataBySel)();
int (__cdecl *serverModeIndexByModeFlags)(DWORD Flags);
short *serverLessonLimitArr=0;
byte *serverTimeLimitArr=0;
int *serverInfoChanged;
//__int16 *serverModeChange;
int *mapLoadData;
int (__cdecl *mapLoadFlags)(void* mapLoadData);
void *(__cdecl *serverBanGetList)();
void (__cdecl *serverBanRemove)(int N);
time_t (__cdecl *_time)(time_t *N);

void *(__cdecl *memListNext)(void *Item);
void *(__cdecl *noxMapCycleNext)();
void *(__cdecl *mapLoadName)(char* mapName);
bool (__cdecl *noxMapCycleEnabledCheck)();
int (__cdecl *loadCfg)(char* fileName, int arg2);
int (__cdecl *saveCfg)(char* fileName);
int (__cdecl *loadBanList)(char* fileName);
int (__cdecl *saveBanList)(char* fileName);
void (__cdecl *flushBanList)(void* ptr);
void (__cdecl *playerGoObserver)(void* playerPtr, byte unk1, byte unk2);

bool mapNextSameForced=false;
char nextMapOverride[0x16]={0};
bool isNewGame=false;

//int (__cdecl *mapCurrentFragLimit)(short gameFlags);
//int (__cdecl *mapCurrentTimeLimit)(short gameFlags);

extern void teamCreateDefault(int TeamNumParam, bool notRestrict=false);
extern void httpGetCallback(lua_State *L);


//extern unsigned __stdcall httpAuth(void *Data1);

void *(__cdecl *getConfigData)();

void *(__cdecl *playerGetDataFromIndex)(int index);

extern byte authorisedState[0x20];
extern char* authorisedLogins[0x20];
DWORD* currentIP;
unsigned __int16 *currentPort;
extern void AuthProcess();
extern void updateAuthDBProcess();

extern bool specialAuthorisation;

using namespace std;
char authSendWelcomeMsg[0x20];




vector<string> mapCycleCurrentList;
__int16 mapCycleLastModeId;
int mapCycleCurrentPosition;

extern void* getPlayerUDataFromPlayerInfo(void *addr);

extern void netVersionServerRq(int sendTo);

extern int clientsVersions[0x20];



bool serverRequest(int f,char *path)
{
	int Top=lua_gettop(L);
	char *P=0,*E=0,*Cmd=0;
	char *Con=NULL;
	P=strchr(path,'?');
	if (P==NULL)
	{
		return false;
	}
	bool access=false;
	lua_getfield(L,LUA_REGISTRYINDEX,"serverFn");
	for (P=strtok_s(P+1,"&",&Con);P!=NULL;P=strtok_s(NULL,"&",&Con))
	{
		E=strchr(P,'=');
		if (E==NULL)
			continue;
		if (strncmp(P,"p",E-P)==0)
		{
			lua_getfield(L,LUA_REGISTRYINDEX,"serverPass");
			if (lua_isnil(L,-1))
				access=true;
			else
				access=(0==strcmp(lua_tostring(L,-1),E+1));
			lua_pop(L,1);
		}
		if (strncmp(P,"r",E-P)==0)
		{
			Cmd=E+1;
		}
	}
	if (Cmd!=NULL)
	{
		if (!access)
		{
			const char WrongPass[]="Access denied";
			send_headers(f,403,"/",NULL,"text/plain",strlen(WrongPass),0);
			tputs(f,WrongPass);
			lua_settop(L,Top);
			return true;
		}
		lua_pushstring(L,Cmd);
		if (0!=lua_pcall(L,1,1,0))
		{
			TString S;				
			S="{'err':'";
			S.append(lua_tostring(L,-1));
			S.append("'}");
			send_headers(f,500,"/",NULL,"application/json",S.size(),0);
			tputs(f,S.c_str());
		}
		else
		{
			send_headers(f,200,"/",NULL,"application/json",lua_objlen(L,-1),0);
			tputs(f,lua_tostring(L,-1));
		}
	}
	lua_settop(L,Top);
	return true;
}
bool needToFormGame=false;

void doFormGame()
{
	int Top=lua_gettop(L);
	bool sameMap = false;
	bool dontUseGUIFunc = false;
	bool newGame=false;

	lua_getfield(L,LUA_REGISTRYINDEX,"formGameTable");
	if (lua_type(L,Top+1)==LUA_TTABLE)
	{
		lua_getfield(L,Top+1,"new");
		if (lua_type(L,-1)!=LUA_TNIL)
		{
			if (0!=lua_toboolean(L,-1))
			{
				newGame=true;
			}
		}
		ServerData *Data=(ServerData*)serverGetGameDataBySel();
		strncpy(Data->mapName, mapGetName(),0x8);
		Data->gameFlags=(__int16)*GameFlags;
		//__int16 GameFlagsTruncated = (__int16)*GameFlags;
		//memcpy(Data->fragLimit,GameFlagsTruncated,0x2);
		//Data->fragLimit=(__int16)mapCurrentFragLimit((__int16)*GameFlags);
		//Data->timeLimitMB=(__int16)mapCurrentTimeLimit((__int16)*GameFlags);
		lua_getfield(L,Top+1,"map");
		if (lua_type(L,-1)!=LUA_TNIL)
		{
			char Buf[16]={0};
			strncpy(Buf,lua_tostring(L,-1),16);
			char *P;
			P=strstr(Buf,".map");
			if (P!=NULL)
			{
				*P=0;
			}
			lua_getfield(L,Top+1,"reload");
			if(strcmp(Buf,mapGetName())==0 && (lua_type(L,-1)!=LUA_TNIL && 0!=lua_toboolean(L,-1)))
			{
				strncpy(Data->mapName,mapGetName(),8);
				sameMap=true;
			}
			else
			{
				strncpy(Data->mapName,Buf,8);
				sameMap=false;
			}
			if(strcmp(Buf,mapGetName())==0)
			{
				dontUseGUIFunc=true;
			}
		}
		else
		{
			lua_getfield(L,Top+1,"reload");
			if(lua_type(L,-1)!=LUA_TNIL && 0!=lua_toboolean(L,-1))
			{
				strncpy(Data->mapName,mapGetName(),8);
				sameMap=true;
			}
			dontUseGUIFunc=true;
		}
		if(mapLoadFromFile((void*)&Data->mapName))
		{
			__int16 availableMode=(__int16)mapLoadFlags(mapLoadData);
			lua_getfield(L,Top+1,"mode");
			const char *Mode=lua_tostring(L,-1);
			int Index=0;
			if (Mode!=NULL)
			{
				bool modeSet=false;
				if (0==strcmpi(Mode,"ctf") && (availableMode&0x20))
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x20;
					modeSet=true;
				}
				else if (0==strcmpi(Mode,"kotr") && (availableMode&0x10))
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x10;
					modeSet=true;
				}
				else if ((0==strcmpi(Mode,"highlander") || 0==strcmpi(Mode,"elimination")) && (availableMode&0x400))
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x400;
					modeSet=true;
				}
				else if ((0==strcmpi(Mode,"gameball") || 0==strcmpi(Mode,"flagball")) && (availableMode&0x40))
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x40;
					modeSet=true;
				}
				/*else if (0==strcmpi(Mode,"quest"))
				{
					Data->gameFlags=0x3007;
				}*///От греха, а то грузить напрямую слишком бажно. Лучше уж полностью сервак с нуля пересоздавать...
				else if (0==strcmpi(Mode,"arena") && (availableMode&0x100))
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x100;//arena
					modeSet=true;
				}
				else
				{
					__int16 currentGameFlags=(__int16)*GameFlags&0x1FF0;
					if(availableMode&currentGameFlags) //Если игра поддерживает текущий режим игры
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|currentGameFlags;
					}
					else if(availableMode&0x100) //arena
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x100;
					}
					else if(availableMode&0x20) //ctf
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x20;
					}
					else if(availableMode&0x40) //flagball
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x40;
					}
					else if(availableMode&0x80) //chat
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x80;
					}
					else if(availableMode&0x400) //elim
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x400;
					}
					else if(availableMode&0x10) //kotr
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x10;
					}
					else //Все остальные пробуем загрузить с текущими флагами
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|currentGameFlags;
					}
				}
				//*serverModeChange=*serverModeChange&0x0E80;
				//*serverModeChange=*serverModeChange&Data->gameFlags;
				if(modeSet==true && strcmp(Data->mapName,mapGetName())==0)
				{
					sameMap=true;
				}
			}
			else if(newGame==false)
			{
				__int16 currentGameFlags=(__int16)*GameFlags&0x1FF0;
				if(availableMode&currentGameFlags) //Если игра поддерживает текущий режим игры
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|currentGameFlags;
				}
				else if(availableMode&0x100) //arena
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x100;
				}
				else if(availableMode&0x20) //ctf
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x20;
				}
				else if(availableMode&0x40) //flagball
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x40;
				}
				else if(availableMode&0x80) //chat
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x80;
				}
				else if(availableMode&0x400) //elim
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x400;
				}
				else if(availableMode&0x10) //kotr
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x10;
				}
				else //Все остальные пробуем загрузить с текущими флагами
				{
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|currentGameFlags;
				}
			}

			if (newGame==true)
			{
				
				Data->gameFlags=Data->gameFlags&0xE00F;
				Data->gameFlags=Data->gameFlags|0x80;
				if(*GameFlags&0x80)
				{
					Data->isNew=0;
				}
				else
					Data->isNew=1;
				dontUseGUIFunc=false;
				if(strcmp(Data->mapName,mapGetName())==0 && availableMode&0x80)
					sameMap=true;
				//*serverModeChange=*serverModeChange&0x0E80;
				//*serverModeChange=*serverModeChange&Data->gameFlags;
			}
			Index=serverModeIndexByModeFlags(Data->gameFlags);
			lua_getfield(L,Top+1,"fraglimit");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				Data->fragLimit=lua_tointeger(L,-1);
				serverLessonLimitArr[Index]=Data->fragLimit;
				*serverInfoChanged=1;
			}
			
			lua_getfield(L,Top+1,"timelimit");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				Data->timeLimitMB=lua_tointeger(L,-1);
				serverTimeLimitArr[Index]=Data->timeLimitMB;
				*serverInfoChanged=1;
			}
			
			
			lua_pushnil(L);
			lua_setfield(L,LUA_REGISTRYINDEX,"formGameTable");
			
			if(mapLoadFlags(mapLoadData)&0x60 && newGame==false)
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
			else if(dontUseGUIFunc==false)
			{
				guiServerOptionsStartGameMB();
			}
		}
	}
	lua_settop(L,Top);
}


namespace
{
	vector<string> mapCycleFormListMode(__int16 modeId)
	{
		string mode;
		vector<string> formedMapcycle;
		switch(modeId)
		{
		case 0x80:
			mode="[chat]";
			break;
		case 0x100:
			mode="[deathmatch]";
			break;
		case 0x400:
			mode="[elimination]";
			break;
		case 0x20:
			mode="[capture the flag]";
			break;
		case 0x40:
			mode="[flagball]";
			break;
		case 0x10:
			mode="[king of the realm]";
			break;
		default:
			mode="[other]";
			break;
		}
		ifstream file("mapcycle.txt", ios::in | ios::binary);
		if(file.is_open())
		{
			file.seekg(0, ios::end);
			size_t filesize=file.tellg();
			file.seekg(0, ios::beg);
			char searchMode=0;
			string fileLine;
			while(searchMode<2)
			{
				getline(file, fileLine);
				if(!file.eof())
				{
					if(fileLine[fileLine.size()-1]=='\r')
						fileLine.erase((fileLine.size()-1), 1); // Тут мы CR удаляем. Тот самый атавизм со времён печатных машинок
					transform(fileLine.begin(), fileLine.end(), fileLine.begin(), tolower);
					if(searchMode==1 && fileLine[0]!='[')
					{
						fileLine = fileLine.substr(0,13); // Вместе с .map может быть только 13 символов максимум. Отрезаем всё лишнее.
						fileLine = fileLine.substr(0, fileLine.rfind(".map")); // Отрезаем .map. Не именуйте плз карты так: my.map.m! Ну или не прописывайте их в таком виде в мапцикл, а прописывайте по старинке с расширением - my.map.m.map. А не то не загрузится.
						fileLine = fileLine.substr(0,8); // А без .map может быть только 8 символов. Опять таки отрезаем лишнее.
						if(mapLoadFromFile((char*)fileLine.c_str()) && mapLoadFlags(mapLoadData)&modeId) // Проверка, а валидный ли это вообще файл карты... И нужного ли режима!
						{
							fileLine.append(".map"); // Аппендим обратно .map. Требуется для собственно загрузчика карт мапцикла
							formedMapcycle.push_back(fileLine); // Загоняем в список
						}
					}
					else if(searchMode==1) // Сюда мы падаем только если мы достигли следующего mode-а
					{
						searchMode++;
						break;
					}
					if(searchMode==0 && fileLine.compare(mode)==0) // Ура, мы нашли наш режим игры! Переходим в режим скана названий мап...
						searchMode++;
				}
				else // Файло кончилось, что получилось то и получилось
				{
					searchMode=2;
					break;
				}
			}
		}
		return formedMapcycle;
	}

	void* mapCycleNext()
	{
		__int16 currentMode = ((__int16)*GameFlags)&0x1FF0;
		int nextPosition=mapCycleCurrentPosition+1;
		if(currentMode!=mapCycleLastModeId || mapCycleCurrentList.size()<=nextPosition) // Если мы достигли конца предварительно сгенеренного списка из файла мапцикла, или же режим игры сменился - перегенерим список мапцикла для текущего режима игры из файла
		{
			mapCycleLastModeId=currentMode;
			mapCycleCurrentList=mapCycleFormListMode(mapCycleLastModeId);
			nextPosition=0;
		}
		
		if(mapCycleCurrentList.size()<=nextPosition) // Если несмотря на перегенерирование списка мапцикла мы всё равно достигли его конца - ну чтож, начит мапцикл невалиден - возвращаем текущую карту и умываем руки
		{
			char* ret = new char[13];
			strcpy(ret, mapGetName());
			strcat(ret, ".map");
			return ret;
		}
		mapCycleCurrentPosition=nextPosition; // Фиксируем текущий индекс позиции
		return (void*)mapCycleCurrentList[nextPosition].c_str();
		// А ведь раньше тут был мегаиндийский код аж на 327 строк, не считая объявлений переменных выше...
	}
		

	int formGame(lua_State *L)
	{
		lua_settop(L,1);
		if (lua_type(L,1)==LUA_TSTRING)
		{
			lua_newtable(L);
			lua_pushvalue(L,1);
			lua_setfield(L,-2,"map");
			lua_remove(L,1);
		}
		if (lua_type(L,1)!=LUA_TTABLE)
		{
				lua_newtable(L);
				lua_pushboolean(L,true);
				lua_setfield(L,-2,"reload");
				lua_remove(L,1);
		}
		lua_setfield(L,LUA_REGISTRYINDEX,"formGameTable");
		needToFormGame=true;
		return 0;
	}


	int loadCfgL(lua_State *L)
	{
		loadCfg("nox.cfg", 1);
		return 1;
	}

	int saveCfgL(lua_State *L)
	{
		saveCfg("nox.cfg");
		return 1;
	}

	int saveBanListL(lua_State *L)
	{
		saveBanList("ban.txt");
		return 1;
	}

	int loadBanListL(lua_State *L)
	{
		loadBanList("ban.txt");
		return 1;
	}

	int flushBanListL(lua_State *L)
	{
		flushBanList((void*)0x62F038);
		flushBanList((void*)0x62F0C0);
		return 1;
	}

	int reloadBanListL(lua_State *L)
	{
		flushBanListL(L);
		loadBanList("ban.txt");
		return 1;
	}

	int formNextGame(lua_State *L)
	{
		lua_settop(L,1);
		if (lua_type(L,1)==LUA_TSTRING)
		{
			lua_newtable(L);
			lua_pushvalue(L,1);
			lua_setfield(L,-2,"map");
			lua_remove(L,1);
		}
		if (lua_type(L,1)==LUA_TTABLE)
		{
			lua_getfield(L,1,"disable");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				if (0!=lua_toboolean(L,-1))
				{
					lua_pushnil(L);
					lua_setfield(L,LUA_REGISTRYINDEX,"formNextGameTable");
					return 0;
				}
			}
			lua_remove(L,-1);
		}
		if (lua_type(L,1)!=LUA_TTABLE)
		{
				lua_newtable(L);
				lua_pushboolean(L,true);
				lua_setfield(L,-2,"reload");
				lua_remove(L,1);
		}
		lua_setfield(L,LUA_REGISTRYINDEX,"formNextGameTable");
		return 0;
	}
	
	bool __cdecl onMapCycleEnabledCheck()
	{
		lua_getfield(L,LUA_REGISTRYINDEX,"formNextGameTable");
		if (lua_type(L,-1)==LUA_TTABLE)
		{
			return true;
		}
		else
			return noxMapCycleEnabledCheck();
	}

	void* __cdecl onLoadMapCycle()
	{
		if(mapNextSameForced)
		{
			mapNextSameForced=false;
			return nextMapOverride;
		}
		int Top=lua_gettop(L);
		bool sameMap = false;
		//bool dontUseGUIFunc = false;
		bool newGame=false;
		void *result=NULL;
		lua_getfield(L,LUA_REGISTRYINDEX,"formNextGameTable");
		if (lua_type(L,Top+1)==LUA_TTABLE)
		{
			lua_getfield(L,Top+1,"new");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				if (0!=lua_toboolean(L,-1))
				{
					newGame=true;
				}
			}
			ServerData *Data=(ServerData*)serverGetGameDataBySel();
			strncpy(Data->mapName, mapGetName(),0x8);
			Data->gameFlags=(__int16)*GameFlags;
			//__int16 GameFlagsTruncated = (__int16)*GameFlags;
			//memcpy(Data->fragLimit,GameFlagsTruncated,0x2);
			//Data->fragLimit=(__int16)mapCurrentFragLimit((__int16)*GameFlags);
			//Data->timeLimitMB=(__int16)mapCurrentTimeLimit((__int16)*GameFlags);
			lua_getfield(L,Top+1,"map");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				char Buf[16]={0};
				strncpy(Buf,lua_tostring(L,-1),16);
				char *P;
				P=strstr(Buf,".map");
				if (P!=NULL)
				{
					*P=0;
				}
				lua_getfield(L,Top+1,"reload");
				if(strcmp(Buf,mapGetName())==0 && (lua_type(L,-1)!=LUA_TNIL && 0!=lua_toboolean(L,-1)))
				{
					strncpy(Data->mapName,mapGetName(),8);
					sameMap=true;
				}
				else
				{
					strncpy(Data->mapName,Buf,8);
					sameMap=false;
				}
				if(strcmp(Buf,mapGetName())==0)
				{
					sameMap=true;
				}
			}
			else
			{
				lua_getfield(L,Top+1,"reload");
				if(lua_type(L,-1)!=LUA_TNIL && 0!=lua_toboolean(L,-1))
				{
					strncpy(Data->mapName,mapGetName(),8);
					sameMap=true;
				}
				//sameMap=true;
			}
			if(mapLoadFromFile((void*)&Data->mapName))
			{
				__int16 availableMode=(__int16)mapLoadFlags(mapLoadData);
				lua_getfield(L,Top+1,"mode");
				const char *Mode=lua_tostring(L,-1);
				int Index=0;
				if (Mode!=NULL)
				{
					bool modeSet=false;
					if (0==strcmpi(Mode,"ctf") && (availableMode&0x20))
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x20;
						modeSet=true;
					}
					else if (0==strcmpi(Mode,"kotr") && (availableMode&0x10))
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x10;
						modeSet=true;
					}
					else if ((0==strcmpi(Mode,"highlander") || 0==strcmpi(Mode,"elimination")) && (availableMode&0x400))
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x400;
						modeSet=true;
					}
					else if ((0==strcmpi(Mode,"gameball") || 0==strcmpi(Mode,"flagball")) && (availableMode&0x40))
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x40;
						modeSet=true;
					}
					/*else if (0==strcmpi(Mode,"quest"))
					{
						Data->gameFlags=0x3007;
					}*///От греха, а то грузить напрямую слишком бажно. Лучше уж полностью сервак с нуля пересоздавать...
					else if (0==strcmpi(Mode,"arena") && (availableMode&0x100))
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x100;//arena
						modeSet=true;
					}
					else
					{
						__int16 currentGameFlags=(__int16)*GameFlags&0x1FF0;
						if(availableMode&currentGameFlags) //Если игра поддерживает текущий режим игры
						{
							Data->gameFlags=Data->gameFlags&0xE00F;
							Data->gameFlags=Data->gameFlags|currentGameFlags;
						}
						else if(availableMode&0x100) //arena
						{
							Data->gameFlags=Data->gameFlags&0xE00F;
							Data->gameFlags=Data->gameFlags|0x100;
						}
						else if(availableMode&0x20) //ctf
						{
							Data->gameFlags=Data->gameFlags&0xE00F;
							Data->gameFlags=Data->gameFlags|0x20;
						}
						else if(availableMode&0x40) //flagball
						{
							Data->gameFlags=Data->gameFlags&0xE00F;
							Data->gameFlags=Data->gameFlags|0x40;
						}
						else if(availableMode&0x80) //chat
						{
							Data->gameFlags=Data->gameFlags&0xE00F;
							Data->gameFlags=Data->gameFlags|0x80;
						}
						else if(availableMode&0x400) //elim
						{
							Data->gameFlags=Data->gameFlags&0xE00F;
							Data->gameFlags=Data->gameFlags|0x400;
						}
						else if(availableMode&0x10) //kotr
						{
							Data->gameFlags=Data->gameFlags&0xE00F;
							Data->gameFlags=Data->gameFlags|0x10;
						}
						else //Все остальные пробуем загрузить с текущими флагами
						{
							Data->gameFlags=Data->gameFlags&0xE00F;
							Data->gameFlags=Data->gameFlags|currentGameFlags;
						}
					}
					//*serverModeChange=*serverModeChange&0x0E80;
					//*serverModeChange=*serverModeChange&Data->gameFlags;
					if(modeSet==true && strcmp(Data->mapName,mapGetName())==0)
					{
						sameMap=true;
					}
				}
				else if(newGame==false)
				{
					__int16 currentGameFlags=(__int16)*GameFlags&0x1FF0;
					if(availableMode&currentGameFlags) //Если игра поддерживает текущий режим игры
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|currentGameFlags;
					}
					else if(availableMode&0x100) //arena
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x100;
					}
					else if(availableMode&0x20) //ctf
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x20;
					}
					else if(availableMode&0x40) //flagball
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x40;
					}
					else if(availableMode&0x80) //chat
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x80;
					}
					else if(availableMode&0x400) //elim
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x400;
					}
					else if(availableMode&0x10) //kotr
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|0x10;
					}
					else //Все остальные пробуем загрузить с текущими флагами
					{
						Data->gameFlags=Data->gameFlags&0xE00F;
						Data->gameFlags=Data->gameFlags|currentGameFlags;
					}
				}

				if (newGame==true)
				{
					
					Data->gameFlags=Data->gameFlags&0xE00F;
					Data->gameFlags=Data->gameFlags|0x80;
					if(*GameFlags&0x80)
					{
						Data->isNew=0;
					}
					else
						Data->isNew=1;
					//sameMap=false;
					isNewGame=true;
					if(strcmp(Data->mapName,mapGetName())==0 && availableMode&0x80)
						sameMap=true;
					//*serverModeChange=*serverModeChange&0x0E80;
					//*serverModeChange=*serverModeChange&Data->gameFlags;
				}
				Index=serverModeIndexByModeFlags(Data->gameFlags);
				lua_getfield(L,Top+1,"fraglimit");
				if (lua_type(L,-1)!=LUA_TNIL)
				{
					Data->fragLimit=lua_tointeger(L,-1);
					serverLessonLimitArr[Index]=Data->fragLimit;
					*serverInfoChanged=1;
				}
				
				lua_getfield(L,Top+1,"timelimit");
				if (lua_type(L,-1)!=LUA_TNIL)
				{
					Data->timeLimitMB=lua_tointeger(L,-1);
					serverTimeLimitArr[Index]=Data->timeLimitMB;
					*serverInfoChanged=1;
				}
				
				
				lua_pushnil(L);
				lua_setfield(L,LUA_REGISTRYINDEX,"formNextGameTable");
				
				if(mapLoadFlags(mapLoadData)&0x60 && newGame==false)
				{
					teamCreateDefault(2);
				}
				else
				{
					teamCreateDefault(-1);
				}
				/*if(sameMap)
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
				else if(dontUseGUIFunc==false)
				{
					guiServerOptionsStartGameMB();
				}*/
				nextMapOverride[0]='\0';
				strcpy(nextMapOverride, Data->mapName);
				strcat(nextMapOverride, ".map");
				result=nextMapOverride;
				if(sameMap)
					mapNextSameForced=true;
			}
			else
			{
				if(noxMapCycleEnabledCheck())
					result=mapCycleNext();
				else
				{
					mapNextSameForced=true;
					return onLoadMapCycle();
				}

			}
		}
		else
		{
			if(noxMapCycleEnabledCheck())
				result=mapCycleNext();
			else
			{
				mapNextSameForced=true;
				return onLoadMapCycle();
			}
		}
		lua_settop(L,Top);
		return result;
	}

	void onMapLoadName(char* mapName)
	{
		if(isNewGame)
		{
			isNewGame=false;
			guiServerOptionsStartGameMB();
		}
		else
			mapLoadName(mapName);
	}


	int banGetList(lua_State *L)
	{
		lua_createtable(L,5,0);
		void *Ptr=serverBanGetList();
		for (int i=1;Ptr!=NULL;Ptr=memListNext(Ptr),i++)
		{
			char Buf[0x40]={0};
			wcstombs(Buf,((wchar_t*)Ptr)+0x6,0x24-0x6);
			lua_pushstring(L,Buf);
			lua_rawseti(L,-2,i);
		}
		return 1;
	}
	int banRemove(lua_State *L)
	{
		lua_settop(L,1);
		if (lua_type(L,1)==LUA_TSTRING)
		{
			void *Ptr=serverBanGetList();
			for (int i=0;Ptr!=NULL;Ptr=memListNext(Ptr),i++)
			{
				char Buf[0x40]={0};
				wcstombs(Buf,((wchar_t*)Ptr)+0x6,0x24-0x6);
				if (0==strcmp(Buf,lua_tostring(L,1)))
				{
					serverBanRemove(i);
					return 0;
				}
				
			}
		}
		else if (lua_type(L,1)==LUA_TNUMBER)
		{
			serverBanRemove(lua_tointeger(L,1));
		}
		else
		{
				lua_pushstring(L,"wrong args - not a team!");
				lua_error_(L);
		}
		return 0;
	}

	int playerKickUData(lua_State *L)
	{
		lua_settop(L,1);
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)
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
		void **PP=(void **)(((char*)lua_touserdata(L,1))+0x2EC);
		PP=(void**)(((char*)*PP)+0x114);
		byte *P=(byte*)(*PP);
		byte playerIdx = *((byte*)(P+0x810));
		if(playerIdx==0x1F)
		{
			lua_pushstring(L,"can't kick the hoster!");
			lua_error_(L);
		}
		playerKickByIdx((int)playerIdx, 4);
		return 0;
	}

	int httpServerL(lua_State *L)
	{
		lua_settop(L,2);
		serverClose();
		if (lua_type(L,1)!=LUA_TNIL)
		{
			int R=serverStart(lua_tointeger(L,1))?1:0;
			lua_pushvalue(L,2);
			lua_setfield(L,LUA_REGISTRYINDEX,"serverPass");

			lua_pushboolean(L,R);
		}
		else
			lua_pushboolean(L,0);
		return 1;
	}
	void (__cdecl*guiUpdate)();

	time_t onEndGame(time_t *TimePtr)
	{
		int Top=lua_gettop(L);
		getServerVar("onEndGame");
		if (!lua_isfunction(L,-1))
			return _time(TimePtr);
		lua_pcall(L,0,0,0);
		lua_settop(L,Top);
		return _time(TimePtr);
	}

	void* __cdecl onServerStart()
	{
		for(byte i=0; i<0x20; i++)
		{
			authorisedState[i]=0;
			if(authorisedLogins[i]!=0 && strcmp(authorisedLogins[i], "")!=0)
				delete [] authorisedLogins[i];
			authorisedLogins[i]="";
			authSendWelcomeMsg[i]=0;
		}
		mapCycleLastModeId=((__int16)*GameFlags)&0x1FF0;
		mapCycleCurrentPosition=0;
		return getConfigData();
	}
	
	void __cdecl OnGuiUpdate()
	{
		guiUpdate();
		httpGetCallback(L);
		updateAuthDBProcess();
		if (needToFormGame)
		{
			needToFormGame=false;
			doFormGame();
		}
		replayGuiUpdate();
		/*if(replayViewFormGameRequest)
		{
			replayViewFormGameRequest=false;
			replayViewFormGame();
		}*/
	}

	void* onPlayerLeave(byte Index)
	{
		void* result = playerGetDataFromIndex(Index);
		authorisedState[Index]=0;
		if(strcmp(authorisedLogins[Index], "")!=0)
			delete [] authorisedLogins[Index];
		authorisedLogins[Index]="";
		clientsVersions[Index]=0;
		int Top=lua_gettop(L);
		getServerVar("playerOnLeave");
		if (lua_isfunction(L,-1))
		{
			void* Player = getPlayerUDataFromPlayerInfo(result);
			lua_pushlightuserdata(L,Player);
			if (0!=lua_pcall(L,1,0,0))
				conPrintI(lua_tostring(L,-1));
		}
		lua_settop(L,Top);
		return result;
	}

	void* onPlayerJoin(int Index)
	{
		clientsVersions[Index]=0;
		void* result=playerGetDataFromIndex(Index);
		if(Index!=0x1F && specialAuthorisation==true)
		{
			authorisedState[(byte)Index]=0; // Перестраховщик я какой то... (c) Evengard
			authorisedLogins[(byte)Index]="";
			playerGoObserver(result, 1, 1);
			authSendWelcomeMsg[Index]=30;

			netVersionServerRq(Index);
		}
		return result;
	}

}
extern "C" void adminInit(lua_State *L);
extern void authInit(lua_State *L);
extern void InjectOffs(DWORD Addr,void *Fn);

void adminInit(lua_State *L)
{
	ASSIGN(guiServerOptionsStartGameMB,0x00459150);
	ASSIGN(serverGetGameData,0x00416590);
	ASSIGN(serverGetGameDataBySel,0x004165B0);
	ASSIGN(serverBanGetList,0x00416900);
	ASSIGN(serverBanRemove,0x00416820);
	ASSIGN(memListNext,0x00416910);
	ASSIGN(guiUpdate,0x0046B740);
	ASSIGN(serverModeIndexByModeFlags,0x00409A70);
	ASSIGN(serverLessonLimitArr,0x005D5334);
	ASSIGN(serverTimeLimitArr,0x005D5340);
	ASSIGN(serverInfoChanged,0x005D5360);
	//ASSIGN(serverModeChange,0x0062F0B6);
	ASSIGN(consoleServerMapLoad,0x00441910);
	ASSIGN(mapLoadFromFile,0x004CFE10);// функция загрузки карты из файла
	ASSIGN(mapLoadName,0x00409D70);// функция загрузки карты из файла - используется при загрузке из мапцикла
	ASSIGN(mapLoadData, 0x00974880);// оффсет куда mapLoadFromFile грузит карту
	ASSIGN(mapLoadFlags, 0x004CFFA0);// функция получения флагов из загруженного файла
	//ASSIGN(mapCurrentTimeLimit, 0x0040A180);
	//ASSIGN(mapCurrentFragLimit, 0x0040A020);
	ASSIGN(noxMapCycleNext, 0x004D0CF0);
	ASSIGN(_time,0x00566BD3);
	ASSIGN(noxMapCycleEnabledCheck, 0x004D0D70);
	ASSIGN(loadCfg, 0x004317B0);
	ASSIGN(saveCfg, 0x00433290);
	ASSIGN(loadBanList, 0x004E41B0);
	ASSIGN(saveBanList, 0x004E43F0);
	ASSIGN(flushBanList, 0x00425760);
	ASSIGN(getConfigData, 0x00416640);
	ASSIGN(playerGetDataFromIndex,0x00417090);
	ASSIGN(playerGoObserver,0x004E6860);
	ASSIGN(currentIP,0x0097EBC4);
	ASSIGN(currentPort,0x0097EBC8);
	ASSIGN(playerKickByIdx,0x004DEAB0);

	InjectOffs(0x004D280B+1,&onMapCycleEnabledCheck); //Обход проверки на вкл. мапцикл если использовалась formNextGame
	InjectOffs(0x0043E333+1,&OnGuiUpdate);
	InjectOffs(0x00413D37+1,&onEndGame);
	InjectOffs(0x004D281D+1,&onLoadMapCycle); //Хук на мапцикл
	InjectOffs(0x004D283F+1,&onLoadMapCycle); //Хук на мапцикл
	InjectOffs(0x004D284F+1,&onMapLoadName); //Поддержка new=1 в formNextGame (а вообще походу костыль)
	InjectOffs(0x0043AAB9+1,&onServerStart); //Server startup hook - инициализируем тут свои переменные
	InjectOffs(0x004DE55E+1,&onPlayerLeave); //Уход игрока с серва - деавторизуем его
	InjectOffs(0x004DDF6C+1,&onPlayerJoin); //Игрок вошёл на сервер - вводим его в обсерв

	int Top=lua_gettop(L);
	if (0!=luaL_loadstring(L,
		"local f=loadstring; "
		"return function (s) "
		"s=string.gsub(s,'%%(%x%x)',function (v) return string.char('0x'..v) end); "
		"local r=f(s); "
		"setfenv(r,getfenv(1)) "
		"return json.encode( r() ) end; "))
	{
		MessageBox(0,lua_tostring(L,-1),0,0);
		luaL_loadstring(L,"");
	}
	else
	{
		if (0==lua_pcall(L,0,1,0))
		{
		}
		else
		{
			MessageBox(0,lua_tostring(L,-1),0,0);
			luaL_loadstring(L,"");
		}
	}
	lua_getfield(L,LUA_REGISTRYINDEX,"server");
	lua_setfenv(L,-2);
	lua_setfield(L,LUA_REGISTRYINDEX,"serverFn");
	if (0!=luaL_loadstring(L,
		"return function (s) "
		"local r=s; "
		"setfenv(r,getfenv(1)) "
		"return json.encode( r() ) end; "))
	{
		MessageBox(0,lua_tostring(L,-1),0,0);
		luaL_loadstring(L,"");
	}
	else
	{
		if (0==lua_pcall(L,0,1,0))
		{
		}
		else
		{
			MessageBox(0,lua_tostring(L,-1),0,0);
			luaL_loadstring(L,"");
		}
	}
	lua_getfield(L,LUA_REGISTRYINDEX,"server");
	lua_setfenv(L,-2);
	lua_setfield(L,LUA_REGISTRYINDEX,"serverFnSysop");
	registerserver("httpServer",&httpServerL);
	registerserver("formGame",&formGame);
	registerserver("formNextGame",&formNextGame);
	registerserver("banList",&banGetList);
	registerserver("banRemove",&banRemove);
	registerserver("loadCfg",&loadCfgL);
	registerserver("saveCfg",&saveCfgL);
	registerserver("loadBanList",&loadBanListL);
	registerserver("saveBanList",&saveBanListL);
	registerserver("reloadBanList",&reloadBanListL);
	registerserver("flushBanList",&flushBanListL);
	registerserver("playerKickUData",&playerKickUData);
	authInit(L);
	//strcpy((char*)0x005AFA20, "So_Forum"); // Смена дефолтной чат-мапы при игре через "локальную" сеть
	lua_settop(L,Top);
}