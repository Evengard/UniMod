#include "stdafx.h"

extern int (__cdecl *noxThingTypeByName)(char const *Name);
void *(__cdecl *unitDefGet)(int N);
void *(__cdecl *spriteDefGet)(int N);
const char *luaTypeName[]=
{
	"nil","boolean","userdata","number","string","table","function",
};
const int luaTypeLen[]=
{
	0,1,0,sizeof(lua_Number),0,0,0,
};
extern void *(__cdecl *noxAlloc)(int Size);
extern void (__cdecl *noxFree)(void *Ptr);
extern char *copyString(const char *Str);
extern wchar_t *copyStringW(const char *Str);
extern void netSendAll(void *Buf,int BufSize);
namespace
{
	struct DefData
	{
		const char *LuaStr;
		int Ofs;/// смещение в юнитдефе
		int Type;
		/// 1=int
		/// 2=byte
		/// 3=float
		/// 4=hexDword
		/// 5=imgId
		/// 6=word
		/// 7=stringPtr
		/// 8=stringWPtr // вайд-строка
		/// 9=FnTable 2 // таблицы с 2мя двордами
		// 10=FnTable 3 // таблицы с 3мя двордами
		// 11=FnTable 4 // таблицы с 4мя двордами
		DWORD TablePtr;
	};
	int X;
	/// данные брать из 005B82B0 unitDefParseTable
	const DefData ServerDef[]=
	{
		{"thingType",		0x00,6},	
		{"name",			0x04,7},
		{"menuIcon",		0x0C,5},
		{"class",			0x18,4},
		{"subclass",		0x1C,4},
		{"flags",			0x20,4},
		{"unitXP",          0x2c,3},
		{"unitWorth",       0x30,1},
		{"mass",			0x38,3},
		{"extentType",		0x3C,2},
		{"extentBoxX",		0x48,3},
		{"extentBoxY",		0x4C,3},
		{"capacity",		0x7A,6},
		{"collideFn",		0x8C,11,0x005CA1B8},//unitCollideProcTable
		{0},
	};
	// данные из 005A4CF8 spriteDefParseTable
	const DefData ClientDef[]=
	{
		{"prettyName",			0x4,8},
		{"description",			0x8,8},
		{"extentType",			0x14,2},
		{"class",				0x20,4},
		{"subclass",			0x24,4},
		{"flags",				0x28,4},
		{"extentBoxX",			0x4C,3},
		{"extentBoxY",			0x50,3},
		{"clientUpdateFn",		0x64,9,0x005B1BE0},// clientUpdateProcTable
		{"menuIcon",			0x74,5},
		{0},
	};
	int registerUnitDef(lua_State *L)
	{
		lua_settop(L,1);
		if (lua_type(L,1)!=LUA_TTABLE)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		lua_pushstring(L,"TODO!");
		lua_error(L);		
		return 1;
	}
	void dumpFnTable(BYTE *B,const DefData *D,lua_State *L,const int Delta)
	{
		char **Name=0;
		DWORD *Fn=0;
		DWORD Ptr=*((DWORD*)(B+D->Ofs));
		ASSIGN(Name,D->TablePtr);
		ASSIGN(Fn,D->TablePtr+4);
		for (;(*Name)!=0;Fn+=Delta,Name+=Delta)
		{
			if (*Fn==Ptr)
				break;
		}
		if (*Fn!=0)
		{
			lua_pushstring(L,(*Name));
			return;
		}
		if (NULL==Ptr)
		{		
			lua_pushnil(L);
			return;
		}
		/// TODO: Здесь надо добавить проверку на случай функции из юнимод
		char Buf[32]="";
		sprintf(Buf,"<unknown %p>",Ptr);
		lua_pushstring(L,Buf);

	}
	void dumpRecord(BYTE *B,const DefData *D,lua_State *L)
	{
		char Buf[16]="";
		switch (D->Type)
		{
		case 1:
			lua_pushinteger(L,*((int*)(B+D->Ofs)));
			break;
		case 2:
			lua_pushinteger(L,B[D->Ofs]);
			break;
		case 3:
			lua_pushnumber(L,*((float*)(B+D->Ofs)));
			break;
		case 5:	/// пока просто загружу число, а потом чего-нить придумаю

		case 4:
			sprintf(Buf,"0x%08X",*((int*)(B+D->Ofs)));
			lua_pushstring(L,Buf);
			break;
		case 6:
			lua_pushinteger(L,*((WORD*)(B+D->Ofs)));
			break;
		case 7:
			{
			char *Str=*((char **)(B+D->Ofs));
			if (Str!=NULL)
				lua_pushstring(L,Str);
			else
				lua_pushnil(L);
			}
			break;
		case 8:
			{
			wchar_t *WStr=*((wchar_t **)(B+D->Ofs));
			if (WStr!=NULL)
			{
				int Len=wcslen(WStr)+1;
				char *Str=new char[Len];
				wcstombs(Str,WStr,Len);
				lua_pushstring(L,Str);
				delete Str;
			}
			else
				lua_pushnil(L);
			}
			break;
		case 9: //функции на 2
			dumpFnTable(B,D,L,2);
			break;
		case 10: //функции на 3-е
			dumpFnTable(B,D,L,3);
			break;
		case 11: //функции на 4-е
			dumpFnTable(B,D,L,4);
			break;
		
		default:
			lua_pushnil(L);
		}
		lua_setfield(L,-2,D->LuaStr);
	}
	void setRecord2(BYTE *B,const DefData *D,int ValueType,const char *Value)
	{
		char Buf[64]="";
		int LuaType=(D->Type<=6)?LUA_TNUMBER:0;
		if (LuaType!=0 && (ValueType!=LuaType))
		{
			return;
		}
		switch (D->Type)
		{
		case 1:
			*((int*)(B+D->Ofs))=*((lua_Number*)Value);
			break;
		case 2:
			B[D->Ofs]=*((lua_Number*)Value);;
			break;
		case 3:
			*((float*)(B+D->Ofs))=*((lua_Number*)Value);;
			break;
		case 5:	/// пока просто загружу число, а потом чего-нить придумаю

		case 4:
			*((int*)(B+D->Ofs))=*((lua_Number*)Value);;
			break;
		case 6:
			*((WORD*)(B+D->Ofs))=*((lua_Number*)Value);;
			break;
		case 7:
			{
				char *Str=*((char **)(B+D->Ofs));
				if (Str!=NULL)
					noxFree(Str);
				if (ValueType==LUA_TNIL)
				{
					*((char **)(B+D->Ofs))=0;
				}
				else
				{
					*((char **)(B+D->Ofs))=copyString(Value);
				}
			}
			break;
		case 8:
			{
				wchar_t *Str=*((wchar_t **)(B+D->Ofs));
				if (Str!=NULL)
					noxFree(Str);
				if (ValueType==LUA_TNIL)
				{
					*((wchar_t **)(B+D->Ofs))=0;
				}
				else
				{
					*((wchar_t **)(B+D->Ofs))=copyStringW(Value);
				}
			}
			break;
		case 9: //функции на 2
		case 10: //функции на 3-е
		case 11: //функции на 4-е
		default:
			return;
		}
	}
	void setRecord(BYTE *B,const DefData *D,lua_State *L)
	{
		char Buf[64]="";
		int LuaType=(D->Type<=6)?LUA_TNUMBER:0;
		if (LuaType!=0 && (lua_type(L,3)!=LuaType))
		{
			sprintf(Buf,"wrong value for field %s, must be %s",lua_tostring(L,2),luaTypeName[LuaType]);
			lua_pushstring(L,Buf);
			lua_error(L);
		}
		switch (D->Type)
		{
		case 1:
			*((int*)(B+D->Ofs))=lua_tointeger(L,3);
			break;
		case 2:
			B[D->Ofs]=lua_tointeger(L,3);
			break;
		case 3:
			*((float*)(B+D->Ofs))=lua_tonumber(L,3);
			break;
		case 5:	/// пока просто загружу число, а потом чего-нить придумаю

		case 4:
			*((int*)(B+D->Ofs))=lua_tointeger(L,3);
			break;
		case 6:
			*((WORD*)(B+D->Ofs))=lua_tointeger(L,3);
			break;
		case 7:
			{
				char *Str=*((char **)(B+D->Ofs));
				if (Str!=NULL)
					noxFree(Str);
				if (lua_type(L,3)==LUA_TNIL)
				{
					*((char **)(B+D->Ofs))=0;
				}
				else
				{
					*((char **)(B+D->Ofs))=copyString(lua_tostring(L,3));
				}
			}
			break;
		case 8:
			{
				wchar_t *Str=*((wchar_t **)(B+D->Ofs));
				if (Str!=NULL)
					noxFree(Str);
				if (lua_type(L,3)==LUA_TNIL)
				{
					*((wchar_t **)(B+D->Ofs))=0;
				}
				else
				{
					*((wchar_t **)(B+D->Ofs))=copyStringW(lua_tostring(L,3));
				}
			}
			break;
		case 9: //функции на 2
		case 10: //функции на 3-е
		case 11: //функции на 4-е
		default:
			sprintf(Buf,"field %s is not yet supported",lua_tostring(L,2));
			lua_pushstring(L,Buf);
			lua_error(L);
		}
	}
	int dumpUnitDef(lua_State *L)
	{
		lua_settop(L,1);
		if (lua_type(L,1)==LUA_TSTRING)
		{
			lua_pushinteger(L,noxThingTypeByName(lua_tostring(L,-1)));
		}
		if (lua_type(L,-1)!=LUA_TNUMBER)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		int N=lua_tointeger(L,-1);
		BYTE *B=(BYTE*)unitDefGet(N);
		if (B==NULL)
			return 0;
		lua_newtable(L);
		for (const DefData *D=ServerDef;D->LuaStr!=0;D++)
		{
			dumpRecord(B,D,L);		
		}
		B=(BYTE*)spriteDefGet(N);
		if (B==NULL)
			return 0;
		for (const DefData *D=ClientDef;D->LuaStr!=0;D++)
		{
			dumpRecord(B,D,L);		
		}
		return 1;
	}
	struct PacketDef
	{
		BYTE UniHead[UNIPACKET_HEAD];
		int TT;
		BYTE ValType;
		BYTE ValueLen;
	};
	int unitDefSet(lua_State*L)
	{
		int DefN=0;
		if (lua_type(L,2)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		if (lua_type(L,1)==LUA_TSTRING)
		{
			DefN=noxThingTypeByName(lua_tostring(L,1));
			
		}else if (lua_type(L,1)!=LUA_TNUMBER)
			DefN=lua_tointeger(L,1);
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}
		BYTE *B=(BYTE*)unitDefGet(DefN);
		if (B==NULL)
			return 0;
		const char *Str=lua_tostring(L,2);
		for (const DefData *D=ServerDef;D->LuaStr!=0;D++)
		{
			if(0==stricmp(D->LuaStr,Str))
			{
				setRecord(B,D,L);
				break;
			}
		}
		/// TODO: стоит проверить, а есть ли такое свойство
		///тут отправляем на клиент и меняем свойства там

		const char *Value=0;
		size_t ValueLen=0;
		PacketDef Packet;
		Packet.TT=DefN;
		Packet.ValType=lua_type(L,3);
		lua_Number ValueN=0;
		if (Packet.ValType==LUA_TFUNCTION)
		{
			lua_pushvalue(L,lua_upvalueindex(1));
			lua_pushvalue(L,3);
			lua_call(L,1,1);
			Value=lua_tolstring(L,-1,&ValueLen);
		}else if (Packet.ValType==LUA_TNUMBER)
		{
			ValueN=lua_tonumber(L,3);
			Value=(const char*)&ValueN;
			ValueLen=sizeof(ValueN);
		}
		else if (Packet.ValType==LUA_TSTRING)
		{
			Value=lua_tolstring(L,3,&ValueLen);
			ValueLen++;
		}
		else if (Packet.ValType!=LUA_TNIL)
		{
			lua_pushstring(L,"wrong args!");
			lua_error(L);
		}

		int PackSize=sizeof(Packet)+ValueLen+strlen(Str)+1;
		Packet.ValueLen=ValueLen;

		BYTE *Buf=(BYTE*)_alloca(PackSize);// создает переменную в стэке, потому сама по выходе удалится
		memcpy(Buf,&Packet,sizeof(Packet));
		memcpy((char*)Buf+sizeof(Packet),Value,ValueLen);
		strcpy((char*)Buf+sizeof(Packet)+ValueLen,Str);
		BYTE *P2=Buf;
		netUniPacket(upUpdateDef,P2,PackSize);

		netSendAll(Buf,PackSize);
		return 1;
	}
}

void netOnUpdateUnitDef(BYTE *Buf,BYTE *End)
{
	PacketDef *Packet=(PacketDef*)Buf;
	if (End-Buf < (sizeof(PacketDef)+luaTypeLen[Packet->ValType]))
		return ;// ошибка в пакете
	BYTE *B=(BYTE*)spriteDefGet(Packet->TT);
	const char *Str=(const char*)(Buf+sizeof(PacketDef)+Packet->ValueLen);
	const char *Value=(const char*)(Buf+sizeof(PacketDef) );
	for (const DefData *D=ClientDef;D->LuaStr!=0;D++)
	{
		if(0==stricmp(D->LuaStr,Str))
		{
			setRecord2(B,D,Packet->ValType,Value);
			break;
		}
	}
}

void unitDefsInit()
{
	ASSIGN(unitDefGet,0x004E3B70);
	ASSIGN(spriteDefGet,0x0044CF10);

	int Top=lua_gettop(L);
	lua_getglobal(L,"string");
	lua_getfield(L,-1,"dump");
	registerserver("unitDefSet",&unitDefSet,1);
	registerserver("unitDefReg",&registerUnitDef);
	registerserver("unitDefGet",&dumpUnitDef);

	lua_settop(L,Top);
}