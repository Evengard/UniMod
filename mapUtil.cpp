#include "stdafx.h"
#include "direct.h"

extern "C" void mapUtilInit(lua_State*L);
/*
Mode 
0=wb
1=rb
2=r+b
CodeType
0x13=MapEncoding
*/
bool (__cdecl *cryptOpen)(const char *Filename,int Mode,int CodeType);
void (__cdecl *cryptClose)();
int (__cdecl *fileReadWrite)(void *DstBuf, size_t DstSize);
int (__cdecl *genWriteMapMB)(char *Path,int Type);
char *(__cdecl *getGameFolder)();

void **noxWallGlobalList=(void**)0x0075396C;
/*
Формат файла:
DWORD Sign;//FADEFACE
DWORD Crc;
DWORD Var1;
DWORD Var2;
затем разделы

Section
{
byte NameLen;
byte Name[];
DWORD SectionCrcORSize;
}
*/
namespace
{
/*
Тестовые функции для шифрования файлов
	int mapDecode(lua_State*L)
	{
		lua_settop(L,2);
		if (
			lua_type(L,1)!=LUA_TSTRING ||
			(lua_type(L,2)!=LUA_TSTRING)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		FILE *G=fopen(lua_tostring(L,1),"rb");
		if (G==NULL)
		{
			lua_pushstring(L,"mapDecode: unable to open src file!");
			lua_error_(L);
		}
		fseek(G,0,SEEK_END);
		int FileSize=ftell(G);
		fclose(G);

		if (!cryptOpen(lua_tostring(L,1),1,0x13))
		{
			lua_pushstring(L,"mapDecode: unable to open src file!");
			lua_error_(L);
		}
		FILE *F=fopen(lua_tostring(L,2),"wb");
		if (F==NULL)
		{
			lua_pushstring(L,"mapDecode: unable to open src file!");
			lua_error_(L);
		}
		int Remain=FileSize;
		int X;
		const int BufSize=1024;
		char Buf[BufSize]={0};
		while(Remain>0)
		{
			int BlockSize=Remain>BufSize?BufSize:Remain;
			if (0==fileReadWrite(Buf,BlockSize))
			{
				lua_pushstring(L,"mapDecode: read error!");
				lua_error_(L);
			}
			fwrite(Buf,BlockSize,1,F);
			Remain-=BlockSize;
		}
		fclose(F);		
		cryptClose();
		return 0;
		
	}
	int mapEncode(lua_State*L)
	{
		lua_settop(L,2);
		if (
			lua_type(L,1)!=LUA_TSTRING ||
			(lua_type(L,2)!=LUA_TSTRING)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		FILE *F=fopen(lua_tostring(L,1),"rb");
		if (F==NULL)
		{
			lua_pushstring(L,"mapEncode: unable to open src file!");
			lua_error_(L);
		}
		if (!cryptOpen(lua_tostring(L,2),0,0x13))
		{
			lua_pushstring(L,"mapEncode: unable to open dst file!");
			lua_error_(L);
		}

		fseek(F,0,SEEK_END);
		int FileSize=ftell(F);
		fseek(F,0,SEEK_SET);
		int Remain=FileSize;
		int X;
		const int BufSize=1024;
		char Buf[BufSize]={0};
		while(Remain>0)
		{
			int BlockSize=Remain>BufSize?BufSize:Remain;
			if (1!=fread(Buf,BlockSize,1,F))
			{
				lua_pushstring(L,"mapEncode: write error!");
				lua_error_(L);
			}
			if (0==fileReadWrite(Buf,BlockSize))
			{
				lua_pushstring(L,"mapEncode: write error!");
				lua_error_(L);
			}
			Remain-=BlockSize;
		}
		fclose(F);		
		cryptClose();
		return 0;
		
	}
*/
	int mapSave(lua_State*L)
	{
		lua_settop(L,1);
		if (
			lua_type(L,1)!=LUA_TSTRING 
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		char Buf[60];
		sprintf(Buf,"%s\\Maps\\%s",getGameFolder(),lua_tostring(L,1));
		_mkdir(Buf);
		sprintf(Buf,"%s\\Maps\\%s\\%s.map",getGameFolder(),lua_tostring(L,1),lua_tostring(L,1));
		wallRec *Wall=*((wallRec**)(noxWallGlobalList));
		*noxWallGlobalList=(void*)0;
		lua_pushboolean(L,genWriteMapMB(Buf,1));
		*noxWallGlobalList=(wallRec*)Wall;
		return 1;
		
	}
}
void mapUtilInit(lua_State*L)
{
	ASSIGN(cryptOpen,0x00426910);
	ASSIGN(cryptClose,0x004269F0);
	ASSIGN(fileReadWrite,0x00426AC0);
	ASSIGN(genWriteMapMB,0x0051E010);
	ASSIGN(getGameFolder,0x00409E10);

/*	lua_pushcfunction(L,&mapDecode);
	lua_setglobal(L,"mapDecode");

	lua_pushcfunction(L,&mapEncode);
	lua_setglobal(L,"mapEncode");
*/	
	registerclient("mapSave",&mapSave);
}