#include "stdafx.h"
//#include <fcntl.h>
//#include <io.h>
#pragma pack(1)

int (__cdecl *noxWriteToFile)(void *Data,int Size,int Count,void *File);
void* (__cdecl *noxFopen)(char *Name,char *Access);
void (__cdecl *noxFclose)(void *File);
extern void *(__cdecl *noxAlloc)(int Size);
extern char *(__cdecl *mapGetName)();
extern int (__cdecl *netSendBySock)(int Player,void *Data,int Size, int Type);

int (__cdecl *mapValidateMB)(char *FileName,DWORD Crc);
DWORD *mapFileSizeServ;
BYTE **mapFileDataServ;

struct TarBlock
{
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag;
	char linkname[100];
	char magic[8];
	char version[2];
	union 
	{
		struct
		{
			char uname[32];
			char gname[32];
			char devmajor[8];
			char devminor[8];
		} TarDefault;
		struct 
		{
			int Check;
			int Size;
			void *Data;
			TarBlock* nextBlock;
		} Info;
	};
	char prefix[155];
	char Padding[10];

	~TarBlock()
	{
		if (this->Info.Check==123456)
		{
			if (this->Info.Data!=0)
				delete this->Info.Data;
			if (this->Info.nextBlock!=0)
				delete this->Info.nextBlock;
		}
	}
	static int __cdecl Comparer(const void *A_,const void *B_)
	{
		TarBlock *A=*((TarBlock**)A_);
		TarBlock *B=*((TarBlock**)B_);
		int Ret=strcmp(A->prefix,B->prefix);
		if (Ret==0)
			Ret=strcmp(A->name,B->name);
		return Ret;
	}
	struct PathSearcherData
	{
		const char *String;
		int StrLen;
	};
	static int __cdecl PathSearcher(const void *A_,const void *B_)
	{
		PathSearcherData *A=((PathSearcherData*)A_);
		TarBlock *B=*((TarBlock**)B_);
		int Ret;
		const char *P=A->String;
		int Len=0;
		if (Len=strlen(B->prefix))
		{
			Ret=strncmp(P,B->prefix,Len);
			if (Ret!=0)
				return Ret;
			P+=Len;
		}
		Ret=strcmp(P,B->name);
		return Ret;
	}
};
#include "bzip2\bzlib.h"
#include <vector>
typedef std::vector<TarBlock *>  TarList;
namespace
{
	TarList MapList; //туда все куски от карты - выгрузятся вместе с нею
	TarList GlobalList;

	struct IFile
	{
		virtual int fread(void *Buf,size_t Size)=0;
		virtual DWORD getCrc()=0;
		virtual void fclose()=0;
	};
	struct BZFile:public IFile
	{
		BZFILE *Data;
		int Fd;
		int fread(void *Buf,size_t Size)
		{
			int Error=0;
			int Ret=BZ2_bzread(Data,(char*)Buf,Size);
			if (Ret==Size)
				return 1;
			return 0;
		}
		BZFile(){ }
		~BZFile(){fclose();}
		void fclose()
		{
			int Error=0;
			if (Data) {BZ2_bzReadClose(&Error,Data); Data=0;} 
		}
		DWORD getCrc(){ return 0; }
		static IFile* fopen(const char *Filename)
		{
			
			BZFile *Ret=new BZFile();
			Ret->Data=BZ2_bzopen(Filename,"r");
			if (Ret->Data==NULL)
			{
				delete Ret;
				return 0;
			}
			return Ret;

		}
	};
	struct TarFile :public IFile
	{
		FILE *F;
		int fread(void *Buf,size_t Size)
		{
			return ::fread(Buf,Size,1,F);
		}
		DWORD getCrc(){ return 0; }
		TarFile(FILE *F_):F(F_){}
		~TarFile(){ fclose(); }
		void fclose()
		{
			if (F) {::fclose(F);F=0;}
		}
		static IFile* fopen(const char *Filename)
		{
			FILE *F=::fopen(Filename,"rb");
			if (F==NULL)
				return NULL;
			return new TarFile(F);
		}
	};
	int oct2i(const char*Str)
	{
		int Ret=0;
		const char *P=Str;
		for (;*P==' ';)
			P++;
		for( ;(*P>='0') && *P<='7'; P++)
		{
			Ret<<=3;
			Ret+= (*P - '0');
		}
		return Ret;
	}
	bool readFile(const char *Filename,TarList &SearchList,DWORD *Crc=NULL)
	{
		if (sizeof(TarBlock)!=512)
			return false;
		IFile *F=0;
/*		if (0==strcmpi(Filename+strlen(Filename)-4,".tar"))
		{
			F=TarFile::fopen(Filename);
		}else*/ /// тарфайлы были нужны только для тестов
		if( 
			(0==strcmpi(Filename+strlen(Filename)-5,".tbz2"))||
			(0==strcmpi(Filename+strlen(Filename)-8,".tar.bz2"))
			)
		{
			F=BZFile::fopen(Filename);
		}
		if (F==NULL)
			return false;
		
		TarBlock *Block=new TarBlock,*Start=Block;
		bool Error=true;
		int FileCount=0;
		while(1)
		{
			if (1!=F->fread(Block,512))
				break;
			Block->Info.Size=oct2i(Block->size);/// число записано В ВОСЬМЕРИЧНОЙ системе!
			Block->Info.Data=0;
			Block->Info.nextBlock=0;
			Block->Info.Check=123456;
			char *Name=Block->name;
			int nameLen=strlen(Name);
			if (nameLen<1)
			{
				Error=false;
				break; //думаю конец архива
			}
			FileCount++;
			if (Block->Info.Size==0)
			{
				if (Name[nameLen-1]!='/')
					continue;/// зачем нам пустой файл?
				Block->Info.nextBlock=new TarBlock;/// это директория, пущай будет
				Block=Block->Info.nextBlock;
				continue;
			}
			int ReadSize=(Block->Info.Size+0x1FF)&0xFFFFFE00;
			BYTE *B=new BYTE[ReadSize];
			Block->Info.Data=B;
			if (1!=F->fread(B,ReadSize))
				break;/// ошибка чтения
			Block->Info.nextBlock=new TarBlock;
			Block=Block->Info.nextBlock;
		}
		if (Crc!=NULL)
			*Crc=F->getCrc();
		F->fclose();
		delete F;
		if (FileCount>0)
		{
			SearchList.reserve(FileCount);
			TarBlock *N=NULL;
			for (TarBlock *P=Start;P!=NULL;P=N)
			{
				N=P->Info.nextBlock;
				P->Info.nextBlock=0;
				if (0==*P->name) // просто финальный
				{
					delete P;
					break;
				}
				if (123456!=P->Info.Check)// невалидный блок
				{
					delete P;
					break;
				}
				SearchList.push_back(P);/// складываем все в массив
			}
			qsort(&SearchList.front(),SearchList.size(),sizeof(TarBlock *),TarBlock::Comparer);
		}
		return !Error;
	}
	TarBlock* dirSearch(const char *File,TarList *List)
	{
		if ( (List==NULL) || List->empty() )
			return NULL;
		TarBlock::PathSearcherData D={File,strlen(File)};
		TarBlock** Ret=(TarBlock**)bsearch(&D,&List->front(),List->size(),sizeof(TarBlock*),
			TarBlock::PathSearcher);
		return (Ret!=NULL)?*Ret:NULL;
	}
	int loadFileL(lua_State*L)
	{
		if (!lua_isstring(L,1))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}

		readFile(lua_tostring(L,1),lua_toboolean(L,2)?GlobalList:MapList);
		return 0;
	}
	TarBlock *itemByName(const char *P,bool CalledFromCon,char *Buf)
	{
		if (P==NULL)
			return NULL;
		if (strlen(P)>200)
			return NULL;
		TarList *List=&MapList;

		if (!CalledFromCon)
		{
			if (0==strncmp(P,"map/",4))
			{
				P+=4;
			}else
				List=&GlobalList;
		}
		else
		{
			if (*P=='/') P++;
			if (0==strncmp(P,"global/",7))  /// Из карты вызывается глобальные скрипты через /global/
			{
				P+=7;
				List=&GlobalList;
			}
			else
			{
				if (0!=strncmp(P,"map/",4))		/// В карте вызвано как /map/...
					strcpy(Buf,"map/");			/// В карте вызвано просто как ...
				else
					P+=4;
			}
		}
		strcat(Buf,lua_tostring(L,1));

		return dirSearch(P,List);
	}
	int bz2Loader(lua_State*L) /// loadfile из архивов
	{

		const char *P=lua_tostring(L,1);
		char Buf[220]="";

		TarBlock *B=itemByName(P,(lua_objlen(L,lua_upvalueindex(1))!=0),Buf);

		if ( (B==NULL)|| B->Info.Size<=0)
		{
			lua_pushstring (L," is not a valid lua file");
			lua_concat(L,2);
			lua_pushboolean(L,0);
			lua_insert(L,1);
			return 2;
		}
		if (0!=luaL_loadbuffer(L,(char*)B->Info.Data,B->Info.Size,Buf))
		{
			lua_pushstring (L," is not a valid lua file");
			lua_concat(L,2);
			lua_pushboolean(L,0);
			lua_insert(L,1);
			return 2;
		}
		return 1;
	}
	int cliMapSize=0;/// размер файла карты для клиента
	int CurrentSize;/// сколько уже приехало
	char *MapPath=0;
	char *ArchiveName=0;
	void **mapFileHandle;

	DWORD servTBZ2Crc;
	int servArchiveSize=0;
	char servArchiveName[MAX_PATH]="";

	void *clientOpenFile(char *Name,char *Access)
	{
		int Size=strlen(Name)+8;// tar.bz2\n
		MapPath=new char[Size];
		strcpy(MapPath,Name);
		CurrentSize=0;
		return noxFopen(Name,Access);
	}
	void clientWriteChunk(BYTE *Data,int Size,int Count,void *File)
	{
		if (cliMapSize>0)
		{
			if (CurrentSize+Size>cliMapSize)
			{
				int DSize=cliMapSize-CurrentSize;
				noxWriteToFile(Data,DSize,Count,File);
				Size-=DSize;
				Data+=DSize;
				cliMapSize=0;
				char *P=strrchr(MapPath,'.');
				if (P!=NULL)
				{
					noxFclose(File);///по идее всегда File==mapFileHandle
					strcpy(P,".tbz2");
					*mapFileHandle=noxFopen(MapPath,"wb");
				}
			}
		}
		CurrentSize+=Size;
		if (Size>0)
			noxWriteToFile(Data,Size,Count,File);
	}
	int __cdecl mapValidate(char *Name,DWORD Size)
	{
		int Ret=mapValidateMB(Name,Size);
/*		if (0!=servTBZ2Crc)
		{
			char Buf[MAX_PATH]="";
			sprintf(Buf,"maps/%s/%s.",Name,Name);
			char *P=strrchr(Buf,'.');
			FILE *F=NULL;
			strcpy(P,".tbz2");
			F=fopen(Buf,"rb");
			if (F==NULL)
			{
				strcpy(P,".tar.bz2");
				F=fopen(Buf,"rb");
			}
			if (F!=0)
			{
				int Size=0;
				fseek(F,0,SEEK_END);
				Size=ftell(F);
				fseek(F,0,SEEK_SET);
				BYTE *B=new BYTE[Size];
				fread(B,Size,1,F);
				DWORD Crc=0xAAAAAAAA;
				fclose(F);
				{
					DWORD *S=(DWORD*)(B);
					Size>>=2;
					DWORD *E=S+Size;
					for(;S!=E;S++)
					{
						Crc<<=4;
						Crc^=*S;
					}
					servTBZ2Crc=Crc;
				}
				delete B;
			}
		}*/
		return Ret;
	}
	void * __cdecl serverTransferPrepair(int FileSize)
	{
		servTBZ2Crc=0;
		void *Ret;
		*servArchiveName=0;
		char *Buf=servArchiveName;
		sprintf(Buf,"maps/%s/%s.",mapGetName(),mapGetName());
		char *P=strrchr(Buf,'.');
		FILE *F=NULL;
		if (P==NULL)
			return noxAlloc(FileSize);
		strcpy(P,".tbz2");
		F=fopen(Buf,"rb");
		if (F==NULL)
		{
			strcpy(P,".tar.bz2");
			F=fopen(Buf,"rb");
		}
		if (F==NULL)
			return noxAlloc(FileSize);
		servArchiveSize=0;
		fseek(F,0,SEEK_END);
		servArchiveSize=ftell(F);
		fseek(F,0,SEEK_SET);
		Ret=noxAlloc(FileSize+servArchiveSize);
		*mapFileSizeServ=FileSize+servArchiveSize;

		BYTE *B=(BYTE*)Ret;
		B+=FileSize;
		fread(B,servArchiveSize,1,F);
		DWORD Crc=0xAAAAAAAA;
/*		{
			DWORD *S=(DWORD*)(B+FileSize);
			Size>>=2;
			DWORD *E=S+Size;
			for(;S!=E;S++)
			{
				Crc<<=4;
				Crc^=*S;
			}
			servTBZ2Crc=Crc;
		}*/
		fclose(F);
		*mapFileDataServ=B;
		return Ret;
	}
	void __cdecl serverTransferStart(int Player,void *Data,int Size,int Mode)
	{
		BYTE Buf[MAX_PATH+sizeof(int)+4];
		BYTE *P=Buf;
		int Len=strlen(servArchiveName);
		netUniPacket(upSendArchive,P,sizeof(int)+Len);
		*((int*)P)=*mapFileSizeServ-servArchiveSize;/// размер только карты
		P+=4;
		memcpy(P,servArchiveName,Len);
		P+=Len;
		netSendBySock(Player,Buf,P-Buf,Mode);
		netSendBySock(Player,Data,Size,Mode);
	}
}
bool fsRead(const char *File,void *&Data, size_t &Size)
{
	char Buf[220]="";
	Data=NULL;Size=0;
	TarBlock *B = itemByName(File,true,Buf);
	if (!B)
		return false;
	Data=B->Info.Data;
	Size=B->Info.Size;
	return true;
}
void netOnSendArchive(int Size,char *Name,char *NameE)
{
	cliMapSize=Size;/// размер карты
	if (ArchiveName)
	{
		delete ArchiveName;
		ArchiveName=0;
	}
	ArchiveName=new char[NameE-Name+1];
	strncpy(ArchiveName,Name,NameE-Name);
	ArchiveName[NameE-Name]=0;
}
void netOnAbortDownload()
{
	if (MapPath)
	{
		delete MapPath;
		MapPath=0;
	}
	if (ArchiveName)
	{
		delete ArchiveName;
		ArchiveName=0;
	}
	cliMapSize=0;
}
void mapLoadFilesystem(const char *MapName)
{
	char Buf[200];
	do
	{
		sprintf(Buf,"maps/%s/%s.tbz2",MapName,MapName);
		if (readFile(Buf,MapList))
			break;
		sprintf(Buf,"maps/%s/%s.tar.bz2",MapName,MapName);
		if (readFile(Buf,MapList))
			break;
		return; //не нашли файла
	}while (0);
	TarBlock *B=dirSearch("server.lua",&MapList);
	if (B==NULL)
		return; // нету луафайла для сервера
	if (B->Info.Size>0)
	{
		int Top=lua_gettop(L);
		bool Ok=false;
		do
		{
			if (0!=luaL_loadbuffer(L,(char*)B->Info.Data,B->Info.Size,Buf))
				break;
			lua_getfield(L,LUA_REGISTRYINDEX,"server");
			lua_setfenv(L,-2);
			if (0!=	lua_pcall(L,0,0,0))
				break;
			Ok=true;
		}while(0);
		if (!Ok)
		{
			conPrintI(lua_tostring(L,-1));
		}
		lua_settop(L,Top);
	}
}
void mapUnloadFilesystem()
{
	for (TarList::iterator I=MapList.begin();I!=MapList.end();I++)
	{
		delete (*I);
	}
	MapList.clear();
}
extern void InjectOffs(DWORD Addr,void *Fn);
void initFilesystem()
{
/*
при смене карты
MSG_USE_MAP приходит всем, клиентам включая сервак
устанавливает в gameFlags флаг необходимости загрузки
затем в главном цикле при проверке флага загружает данные

*/
	InjectOffs(0x004AB7FD+1,&clientWriteChunk);
	InjectOffs(0x004AB8BC+1,&clientWriteChunk);
	InjectOffs(0x004ABD64+1,&clientOpenFile);
	InjectOffs(0x0043DEF6+1,&mapValidate);
	
	InjectOffs(0x0051A0A1+1,&serverTransferPrepair);
	
	InjectOffs(0x00519AA7+1,&serverTransferStart);

	ASSIGN(mapFileHandle,0x007141D4);
	ASSIGN(noxWriteToFile,0x00565A02);
	ASSIGN(noxFopen,0x0056586C);
	ASSIGN(noxFclose,0x00407533);
	ASSIGN(mapValidateMB,0x004CF470);
	ASSIGN(mapFileDataServ,0x0081B834);
	ASSIGN(mapFileSizeServ,0x0081B838);

	lua_newtable(L);/// в эту таблицу будем что-нить класть если ведем глобальный поиск
	lua_pushcclosure(L,&bz2Loader,1);
	lua_setglobal(L,"bz2Loader");
	lua_register(L,"fileLoad",&loadFileL);
	
}