#include "stdafx.h"
//:004317B0 configLoad(const char*)
//00433290 configSave

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
void (__cdecl *consoleServerMapLoad)(int spaceNumber, int tokensNumber, void* tokens);
void *(__cdecl *serverGetGameData)(int N);
void *(__cdecl *serverGetGameDataBySel)();
int (__cdecl *serverModeIndexByModeFlags)(DWORD Flags);
short *serverLessonLimitArr=0;
byte *serverTimeLimitArr=0;
int *serverInfoChanged;
int *serverModeChange;

void *(__cdecl *serverBanGetList)();
void (__cdecl *serverBanRemove)(int N);

void *(__cdecl *memListNext)(void *Item);

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
namespace
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
				lua_pushstring(L,"wrong args!");
				lua_error_(L);
		}
		lua_setfield(L,LUA_REGISTRYINDEX,"formGameTable");
		return 0;
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
	void __cdecl OnGuiUpdate()
	{
		guiUpdate();
		int Top=lua_gettop(L);
		bool sameMap = false;
		lua_getfield(L,LUA_REGISTRYINDEX,"formGameTable");
		if (lua_type(L,Top+1)==LUA_TTABLE)
		{
			ServerData *Data=(ServerData*)serverGetGameDataBySel();
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
				if(strcmp(Buf,mapGetName())==0 || (0x80 & *GameFlags))
					sameMap=true;
				strncpy(Data->mapName,Buf,8);
			}
			lua_getfield(L,Top+1,"mode");
			const char *Mode=lua_tostring(L,-1);
			int Index=0;
			if (Mode!=NULL)
			{
				if (0==strcmpi(Mode,"ctf"))
				{
					Data->gameFlags=0x20;
				}
				else if (0==strcmpi(Mode,"kotr"))
				{
					Data->gameFlags=0x10;
				}
				else if (0==strcmpi(Mode,"highlander") || 0==strcmpi(Mode,"elimination"))
				{
					Data->gameFlags=0x400;
				}
				else if (0==strcmpi(Mode,"gameball") || (0==strcmpi(Mode,"flagball")))
				{
					Data->gameFlags=0x40;
				}
				else if (0==strcmpi(Mode,"quest"))
				{
					Data->gameFlags=0x1000;
				}
				else 
				{
					Data->gameFlags=0x100;//arena
				}
				*serverModeChange=*serverModeChange&0x0E80;
				*serverModeChange=*serverModeChange&Data->gameFlags;
			}
			Index=serverModeIndexByModeFlags(Data->gameFlags);

			lua_getfield(L,Top+1,"new");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				if (0!=lua_toboolean(L,-1))
				{
					Data->isNew=1;
					Data->gameFlags=0x80;
					*serverModeChange=*serverModeChange&0x0E80;
					*serverModeChange=*serverModeChange&Data->gameFlags;
				}
			}		
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
		lua_settop(L,Top);
	}

}
extern "C" void adminInit(lua_State *L);
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
	ASSIGN(serverModeChange,0x0062F0B6);
	ASSIGN(consoleServerMapLoad,0x00441910);
	InjectOffs(0x0043E333+1,&OnGuiUpdate);

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
	registerserver("httpServer",&httpServerL);
	registerserver("formGame",&formGame);
	registerserver("banList",&banGetList);
	registerserver("banRemove",&banRemove);
	lua_settop(L,Top);
}
