#include "stdafx.h"

/*
Исследовать:
+) Появление обелисков
) Респавны юнитов
+.) Свойства стен
+) Баффы оружия/брони (исследовал но не могу применять)

Eще поправки
+1) Чтобы юнит можно было двигать
+2) Чтбы юнит можно было пнуть в сторону
) определение типа и сабтипа
+3) добавить функцию для карты, ведущую в юнимод
4) чтобы она могла читать/писать переменные по именам
+5) сетевой пакет

-) сделать в консоли больше место для инфы, а то после скана все и не найдешь, потеряеться
-) сделать так что бы при написании теста в консоли были копипаст хотя бы)
+) что бы по этому тексту можно было пермещаться стрелосками, а то удалялять 300 симболов что бы исправить 1 ошибку не удобно...

*/

#define AddCodeLine(a,b) conPrintI(a)


void *(__cdecl *noxCAlloc)(int NumElements,int Size);
void *(__cdecl *noxAlloc)(int Size);
void (__cdecl *noxFree)(void *Ptr);

int (__cdecl *consolePrint)(int Color, wchar_t *Text);
extern bool serverUpdate();

char *copyString(const char *Str)
{
	if (Str==NULL)
		return NULL;
	char *Ret=(char*)noxAlloc(strlen(Str)+1);
	strcpy(Ret,Str);
	return Ret;
}
wchar_t *copyStringW(const char *Str)
{
	if (Str==NULL)
		return NULL;
	wchar_t *Ret=(wchar_t *)noxAlloc(2*(strlen(Str)+1));
	mbstowcs(Ret,Str,strlen(Str)+1);
	return Ret;
}
namespace
{
	int lastConColor=2;
}
void conSetNextColor(int C)
{
	lastConColor=C;
}
void conPrintI(const char *S)
{
	wchar_t Buf[200];
	if(200==mbstowcs(Buf,S,200))
		return;
	consolePrint(lastConColor,Buf);//3-white
	if (lastConColor!=2)
		lastConColor=2;
};
int (__cdecl *printCentered)(wchar_t *Text);
void printI(const char *S)
{
	wchar_t Buf[200];
	if(200==mbstowcs(Buf,S,200))
		return;
	printCentered(Buf);
};
DWORD *GameFlags=(DWORD*)0x5D53A4;
int setGameFlagsL(lua_State *L)
{
	DWORD Old=*GameFlags;
	if(lua_type(L,1)==LUA_TNUMBER)
		*GameFlags=(DWORD)lua_tointeger(L,1);
	lua_pushnumber(L,Old);
	return 1;
}
int bitOrL(lua_State *L)
{
	int R;
	if(lua_type(L,1)!=LUA_TNUMBER)
		return 0;
	if(lua_type(L,2)!=LUA_TNUMBER)
		return 0;
	R=(lua_tointeger(L,1) | lua_tointeger(L,2)) ;
	lua_pushinteger(L,R);
	return 1;
}
int bitAndL(lua_State *L)
{
	int R;
	if(lua_type(L,1)!=LUA_TNUMBER)
		return 0;
	if(lua_type(L,2)!=LUA_TNUMBER)
		return 0;
	R=lua_tointeger(L,1) & lua_tointeger(L,2) ;
	lua_pushnumber(L,R);
	return 1;
}

int printL(lua_State *L)
{
	int n=lua_gettop(L);
	int i=0;
	TString Rz;
	const char *c;
//	WCHAR Char;
	for(i=1;i<=n;i++)
	{
		lua_pushvalue(L,i);
		c=lua_tostring(L,-1);
		lua_pop(L,1);
/*		if(c!=NULL)
		{
			while(*c!=0)
			{
				mbtowc(&Char,c,1);
				Rz+=Char;
				c++;
			}
		}*/
		if(c!=NULL)
			Rz.append(c);

		//Rz+=mbstowcs
		//Rz
	}
	lua_Debug ar;
	if (0==lua_getstack (L, 1, &ar))  /// Достаем прошлую переменную окружения, чтобы читать данные оттуда
		lua_pushnil(L);
	else
		lua_getinfo(L, "f", &ar);
    if (!lua_isnil(L, -1)) 
		lua_getfenv(L,-1);
	if (lua_type(L,-1)==LUA_TTABLE)
	{
		lua_getfield(L,-1,"conOutput");
	}
	if (lua_type(L,-1)==LUA_TNIL)
	{
		printI(Rz.c_str());
	}
	else if(lua_type(L,-1)==LUA_TFUNCTION)
	{
		lua_pushlstring(L,Rz.c_str(),Rz.size());
		if (0!=lua_pcall(L,1,0,0))
		{
			Rz.assign("Error in print:");
			Rz.append(lua_tostring(L,-1));
			printI(Rz.c_str());
		}
	}
	else
	{
		lua_getfield(L,-1,"print");
		lua_pushlstring(L,Rz.c_str(),Rz.size());
		if (0!=lua_pcall(L,1,0,0))
		{
			Rz.assign("Error in print:");
			Rz.append(lua_tostring(L,-1));
			printI(Rz.c_str());
		}
	}
	return 0;
}
void InjectJumpTo(DWORD Addr,void *Fn)// Пишем по данному адресу переход на нашу функцию
{
	BYTE *To=(BYTE *)Addr; 
	DWORD *Dw=(DWORD*)(To+1);
	DWORD Delta=(DWORD)Fn;
	Delta-=4+(DWORD)Dw;
	DWORD OldProtect;
	VirtualProtect(To,5,PAGE_EXECUTE_READWRITE,&OldProtect);
	*Dw=Delta;
	*(To++)=0xE9;
	VirtualProtect(To,5,OldProtect,&OldProtect);
};
void InjectOffs(DWORD Addr,void *Fn)// Пишем по данному адресу - адрес нашей функции c вычислением
{
	DWORD *Dw=(DWORD*)(Addr);
	DWORD Delta=(DWORD)Fn;
	Delta-=4+(DWORD)Dw;
	DWORD OldProtect;
	VirtualProtect(Dw,4,PAGE_EXECUTE_READWRITE,&OldProtect);
	*Dw=Delta;
	VirtualProtect(Dw,4,OldProtect,&OldProtect);
}
void InjectAddr(DWORD Addr,void *Fn)// Пишем по данному адресу - адрес нашей функции
{
	DWORD *Dw=(DWORD*)(Addr);
	DWORD Delta=(DWORD)Fn;
	DWORD OldProtect;
	VirtualProtect(Dw,4,PAGE_EXECUTE_READWRITE,&OldProtect);
	*Dw=Delta;
	VirtualProtect(Dw,4,OldProtect,&OldProtect);
}
#include <list>
int (__cdecl *noxGetUnitsInRect)(FloatRect *Rect, void (__cdecl *SomeFn)(void *Unit, void *Arg), void *Arg);

DWORD *frameCounter=(DWORD*)0x0084EA04;
DWORD *idGold=(DWORD*)0x00750714;

#include <vector>
int *scriptStackSize=(int *)0x0075AE40;
std::vector<void *> scriptValues;

int (__cdecl *noxThingTypeByName)(char const *Name);
const char *(__cdecl *noxThingNameByType)(int Type);

namespace 
{

	void (__cdecl *oldCreateAtPart)();
	void __cdecl noxMyCreateAt(unitBigStructPtr Obj,unitBigStructPtr Parent, float X,float Y)
	{
		__asm{   
			push Y
			push X
			push Parent
			push Obj
			push offset l1
			mov eax,0x00750714
			mov eax,[eax]
			jmp oldCreateAtPart
	l1:
			add esp,16 // 4-е параметра
		}
		lua_getglobal(L,"noxOnCreateAt");
		if(lua_type(L,-1)!=LUA_TFUNCTION)
		{
			lua_pop(L,1);
			return ;
		}
		lua_pushlightuserdata(L,Obj);
		lua_pushlightuserdata(L,Parent);
		lua_pushnumber(L,X);
		lua_pushnumber(L,Y);
		if(0!=lua_pcall(L,4,0,0))
		{
			lua_pop(L,1);
		}
		return ;
	}
/*
	DWORD __cdecl newScriptPushValue(void *Value)
	{
		int SP=*(scriptStackSize);
		if(scriptValues.size()<=SP)
		{
			scriptValues.resize(SP+1);
		}
		scriptValues[SP]=Value;
		(*scriptStackSize)++;
		return SP;
	}
	void *__cdecl newScriptPopValue()
	{
		int SP=(*scriptStackSize);
		if(SP>0)
		{
			(*scriptStackSize)--;
			SP--;
		}
		if(scriptValues.size()<=SP)
		{
			DebugBreak();
			return NULL;
		}

		return scriptValues[SP];
	}
*/
	void getUnitsAroundImpl(void *Unit, void *L_)
	{
		lua_State *LL=(lua_State *)L_;
		lua_pushvalue(L,1);
		lua_pushlightuserdata(LL,Unit);
		if(0!=lua_pcall(LL,1,0,0))
			lua_error_(LL);
	}
	int getPtrPtrL(lua_State *L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA)||(lua_type(L,2)!=LUA_TNUMBER) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		lua_pushnil(L);
		if(P==NULL)
			return 1;
		P=*((BYTE**)(P+lua_tointeger(L,2) ));
		if(P==NULL)
			return 1;
		lua_pushlightuserdata(L,  P);
		return 1;
	}
	int getPtrByteL(lua_State *L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA)||(lua_type(L,2)!=LUA_TNUMBER) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
			return 0;
		P+=lua_tointeger(L,2);
		lua_pushinteger(L,  *((BYTE*)P));
		return 1;
	}
	int getPtrIntL(lua_State *L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA)||(lua_type(L,2)!=LUA_TNUMBER) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
			return 0;
		P+=lua_tointeger(L,2);
		lua_pushinteger(L,  *((int*)P));
		return 1;
	}
	int getPtrShortL(lua_State *L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA)||(lua_type(L,2)!=LUA_TNUMBER) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
			return 0;
		P+=lua_tointeger(L,2);
		lua_pushinteger(L,  *((USHORT*)P));
		return 1;
	}
	int getPtrFloatL(lua_State *L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA)||(lua_type(L,2)!=LUA_TNUMBER) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
			return 0;
		P+=lua_tointeger(L,2);
		lua_pushnumber(L,  *((float*)P));
		return 1;
	}
	int setPtrFloatL(lua_State *L)
	{
		if ( ((lua_type(L,1)!=LUA_TLIGHTUSERDATA)&& (lua_type(L,1)!=LUA_TNIL))
			||(lua_type(L,2)!=LUA_TNUMBER) 
			||(lua_type(L,3)!=LUA_TNUMBER))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)	return 0;
		P+=lua_tointeger(L,2);
		DWORD Pt=(DWORD)P;
		DWORD OldProtect;
		if( ((lua_type(L,1)==LUA_TNIL)) &&( (Pt<0x400000) || (Pt>0x600000) ) )
		{
			lua_pushstring(L,"wrong offset!");
			lua_error_(L);
		}
		VirtualProtect(P,4,PAGE_EXECUTE_READWRITE,&OldProtect);
		*((float*)P) = lua_tonumber(L,3 );
		VirtualProtect(P,4,OldProtect,&OldProtect);
		return 0;
	}
	int setPtrByteL(lua_State *L)
	{
		if ( ((lua_type(L,1)!=LUA_TLIGHTUSERDATA)&& (lua_type(L,1)!=LUA_TNIL))
			||(lua_type(L,2)!=LUA_TNUMBER) 
			||(lua_type(L,3)!=LUA_TNUMBER))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)	return 0;
		P+=lua_tointeger(L,2);
		DWORD Pt=(DWORD)P;
		DWORD OldProtect;
		if( ((lua_type(L,1)==LUA_TNIL)) &&( (Pt<0x400000) || (Pt>0x600000) ) )
		{
			lua_pushstring(L,"wrong offset!");
			lua_error_(L);
		}
		VirtualProtect(P,4,PAGE_EXECUTE_READWRITE,&OldProtect);
		*((BYTE*)P) = lua_tointeger(L,3 );
		VirtualProtect(P,4,OldProtect,&OldProtect);
		return 1;
	}
	int setPtrIntL(lua_State *L)
	{
		if ( ((lua_type(L,1)!=LUA_TLIGHTUSERDATA)&& (lua_type(L,1)!=LUA_TNIL))
			||(lua_type(L,2)!=LUA_TNUMBER) 
			||(lua_type(L,3)!=LUA_TNUMBER))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if (P==NULL)
			return 0;
		P+=lua_tointeger(L,2);
		DWORD Pt=(DWORD)P;
		DWORD OldProtect;
		if( ((lua_type(L,1)==LUA_TNIL)) &&( (Pt<0x400000) || (Pt>0x600000) ) )
		{
			lua_pushstring(L,"wrong offset!");
			lua_error_(L);
		}
		VirtualProtect(P,4,PAGE_EXECUTE_READWRITE,&OldProtect);
		*((int*)P) = lua_tointeger(L,3 );
		VirtualProtect(P,4,OldProtect,&OldProtect);
		return 0;
	}
	int setPtrShortL(lua_State *L)
	{
		if ( ((lua_type(L,1)!=LUA_TLIGHTUSERDATA)&& (lua_type(L,1)!=LUA_TNIL))
			||(lua_type(L,2)!=LUA_TNUMBER) 
			||(lua_type(L,3)!=LUA_TNUMBER))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)	return 0;
		P+=lua_tointeger(L,2);
		DWORD Pt=(DWORD)P;
		DWORD OldProtect;
		if( ((lua_type(L,1)==LUA_TNIL)) &&( (Pt<0x400000) || (Pt>0x600000) ) )
		{
			lua_pushstring(L,"wrong offset!");
			lua_error_(L);
		}
		VirtualProtect(P,4,PAGE_EXECUTE_READWRITE,&OldProtect);
		*((USHORT*)P) = lua_tointeger(L,3 );
		VirtualProtect(P,4,OldProtect,&OldProtect);
		return 0;
	}
	int setPtrPtrL(lua_State *L)
	{
		if ( ((lua_type(L,1)!=LUA_TLIGHTUSERDATA) && (lua_type(L,1)!=LUA_TNIL))
			||(lua_type(L,2)!=LUA_TNUMBER)
			||(lua_type(L,3)!=LUA_TLIGHTUSERDATA))
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if (P==NULL)
			return 0;
		P+=lua_tointeger(L,2);
		DWORD Pt=(DWORD)P;
		DWORD OldProtect;
		if( ((lua_type(L,1)==LUA_TNIL)) &&( (Pt<0x400000) || (Pt>0x600000) ) )
		{
			lua_pushstring(L,"wrong offset!");
			lua_error_(L);
		}
		VirtualProtect(P,4,PAGE_EXECUTE_READWRITE,&OldProtect);
		*((void**)P) = lua_touserdata(L,3 );
		VirtualProtect(P,4,OldProtect,&OldProtect);
		return 0;
	}
	int getUnitClassL(lua_State *L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
		{
			lua_pushnil(L);
			return 0;
		}
		if((lua_gettop(L)>1) && (lua_type(L,2)==LUA_TNUMBER))
		{
			lua_pushboolean(L, lua_tointeger(L,2) & (*((int*)(P+0x8))) );
			return 1;
		}
		lua_pushinteger(L, *((int*)(P+0x8)) );
		return 1;
	}
	int getUnitFlagsL(lua_State *L)
	{
		if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
		{
			lua_pushinteger(L,0);
			return 0;
		}
		if((lua_gettop(L)>1) && (lua_type(L,2)==LUA_TNUMBER))
		{
			lua_pushboolean(L, lua_tointeger(L,2) & (*((int*)(P+0x10))) );
			return 1;
		}
		lua_pushinteger(L, *((int*)(P+0x10)) );
		return 1;
	}
	int getThingTypeL(lua_State *L)
	{
		if((lua_type(L,1)==LUA_TSTRING) )
		{
			int R=noxThingTypeByName(lua_tostring(L,1));
			if(R>=0)
				lua_pushinteger(L,R);
			else
				lua_pushnil(L);
			return 1;
		}
		else if ( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
		{
			lua_pushnil(L);
			return 1;
		}
		lua_pushinteger(L, *((WORD*)(P+4)) );
		return 1;
	}

	int setThingTypeL(lua_State *L)
	{
		int R=0;
		if (lua_type(L,1)!=LUA_TLIGHTUSERDATA) 
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		if (lua_type(L,2)==LUA_TNUMBER)
		{
			R=lua_tointeger(L,2);
		}
		else if(lua_type(L,2)==LUA_TNUMBER)
			R=noxThingTypeByName(lua_tostring(L,2));
		if(R<=0)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}

		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
			return 0;
		*((WORD*)(P+4))=(WORD)R;
		return 0;
	}

	int getClassNameL(lua_State *L)
	{
		const char *R;
		if (lua_type(L,1)==LUA_TNUMBER)
		{
			R=noxThingNameByType(lua_tointeger(L,1));
			if(R!=0)
				lua_pushstring(L, R);
			else
				lua_pushnil(L);
			return 1;
		}
		else if( (lua_type(L,1)!=LUA_TLIGHTUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		BYTE* P=(BYTE*) lua_touserdata(L,1);
		if(P==NULL)
			return 0;
		R=noxThingNameByType(*((WORD*)(P+4))) ;
		if(R!=0)
			lua_pushstring(L, R);
		else
			lua_pushnil(L);
		return 1;
	}
	int getUnitsAround(lua_State *L)
	{
		if (
			(lua_gettop(L)!=5)||
			(lua_type(L,1)!=LUA_TFUNCTION) ||
			(lua_type(L,2)!=LUA_TNUMBER) ||
			(lua_type(L,3)!=LUA_TNUMBER) ||
			(lua_type(L,4)!=LUA_TNUMBER) ||
			(lua_type(L,5)!=LUA_TNUMBER) 
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
			return 0;
		}
		noxGetUnitsInRect(
				&FloatRect(
					lua_tonumber(L,2),
					lua_tonumber(L,3),
					lua_tonumber(L,4),
					lua_tonumber(L,5) )
				,&getUnitsAroundImpl,L);
		return 0;
	}
	

	int timeoutNextId=1;
	struct TimeoutListRec
	{
		int Id;
		DWORD Frame;
		TimeoutListRec(int Id_,DWORD Frame_):Id(Id_),Frame(Frame_)
		{}
	};
	std::list<TimeoutListRec> timeoutList;
	int setTimeoutL(lua_State *L) /// теперь получает 3-й аргумент - таблицу
	{
		lua_settop(L,3);
		if ((lua_type(L,1)!=LUA_TFUNCTION) ||(lua_type(L,2)!=LUA_TNUMBER) 
			|| ((lua_type(L,3)!=LUA_TTABLE) && (lua_type(L,3)!=LUA_TNIL))
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushlightuserdata(L,setTimeoutL);/// функции
		lua_gettable(L,LUA_REGISTRYINDEX);
			lua_pushinteger(L,timeoutNextId);
			lua_pushvalue(L,1);
			lua_settable(L,-3);
		lua_pushlightuserdata(L,&timeoutNextId);/// сюда положим таблицу
		lua_gettable(L,LUA_REGISTRYINDEX);
			lua_pushinteger(L,timeoutNextId);
			lua_pushvalue(L,3);
			lua_settable(L,-3);

		
		lua_pushinteger(L,timeoutNextId);
		
		DWORD Time=*frameCounter +(DWORD)lua_tointeger(L,2);
		
		std::list<TimeoutListRec>::iterator I;
		for (I=timeoutList.begin();I!=timeoutList.end();I++)
		{
			if(I->Frame > Time)
				break;
		}
		timeoutList.insert(I,TimeoutListRec(timeoutNextId++,Time) );
		return 1;
	}
	void (__cdecl *sub51ADF0)();/// событие проверки скриптов карты по таймауту
	int getFrameCounterL(lua_State *L)
	{
		lua_pushinteger(L,(int)*frameCounter);
		return 1;
	}
	void __cdecl onEachFrame()
	{
		serverUpdate();
		DWORD Time=*frameCounter;
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L,&timeoutNextId);/// таблица аргументов
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,setTimeoutL);/// таблица функций
		lua_gettable(L,LUA_REGISTRYINDEX);

		for (std::list<TimeoutListRec>::iterator I=timeoutList.begin();I!=timeoutList.end();)
		{
			if ( Time < I->Frame )
				break;
			lua_pushinteger(L,I->Id);
			if (I->Frame == Time)
			{
				lua_gettable(L,-2);
				if(lua_type(L,-1)==LUA_TFUNCTION)
				{
					lua_getfenv(L,-1);
					lua_pushvalue(L,-2);
					lua_getfield(L,-2,"conOutput");// conOutput функция енв функция
					lua_insert(L,-2);// функция conOutput енв функция
					lua_pushnil(L);
					lua_setfield(L,-4,"conOutput");

					lua_pushinteger(L,Time);
					lua_pushinteger(L,I->Id);
					// таблица с аргументом
					lua_gettable(L,-8); // id,Time,Fn, conOutput, env, fn, {Fns},{Args}
					if (0!=lua_pcall(L,2,0,0))
					{
						char Err[250];
						sprintf(Err,"Error: %240s",lua_tostring(L,-1));
						conPrintI(Err);
						lua_pop(L,1);
					}
					///conOutput,env, fn, {Fns},{Args}
					lua_getfield(L,-2,"conOutput");// conOutput функция енв функция
					if (lua_type(L,-1)==LUA_TNIL) // если задали другую функцию - то так и оставим, но как ее обнулить?
					{
						lua_pop(L,1);
						lua_setfield(L,-2,"conOutput");
						lua_pop(L,2);
					}
					else
						lua_pop(L,3);

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

			I=timeoutList.erase(I);
		}
		lua_settop(L,Top);
		sub51ADF0();
	}

	int memFreeL(lua_State *L)
	{
		if ((lua_type(L,1)!=LUA_TUSERDATA) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *P=lua_touserdata(L,1);
		if(P!=0)
			noxFree(P);
		return 0;
	}

	int memAllocL(lua_State *L)
	{
		if ((lua_type(L,1)!=LUA_TNUMBER) )
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int Size=lua_tonumber(L,1);
		if (Size<=0)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *P=noxAlloc(Size);
		lua_pushlightuserdata(L,P);
		return 1;
	}
};
void mapUnloadUtil()
{
	timeoutList.clear();
}

void lua_error_(lua_State*L)
{
	int T=lua_gettop(L);
	lua_getglobal(L,"debug");
	lua_getfield(L,-1,"traceback");
	if (lua_type(L,-1)==LUA_TFUNCTION)
		lua_call(L,0,0);
	lua_settop(L,T);
	lua_error(L);
}

extern void windowsInit();
extern void unitInit();
extern void mapInit();
extern void reactInit();
extern void spellsInit();
extern void netInit();
extern void keysInit();
extern void consoleInit();
extern void playerInit();
extern void spriteInit();
extern void spellListInit();
extern void clientViewInit();
extern void unitDefsInit();
extern void tilesInit();
extern void cliUntilInit();
extern bool initAuthData();
extern "C" void scoreInit(lua_State *L);
extern "C" void initAudServer(lua_State *L);

extern void guiInit();
extern void waypointsInit();
extern char *(__cdecl *mapGetName)();

extern DWORD __cdecl onConCmd(wchar_t *A,DWORD B);
extern void initModLib2();
extern void initFilesystem();

extern "C" void adminInit(lua_State *L);
extern "C" int luaopen_lpeg (lua_State *L);
extern "C" void loadJson(lua_State *L);
extern "C" void mapUtilInit(lua_State*L);
extern bool serverUpdate();

void injectCon()
{
	initModLib2();
	luaopen_lpeg (L);
	loadJson(L);
	ASSIGN(consolePrint,0x00450B90);
	ASSIGN(printCentered,0x00445490);
	ASSIGN(sub51ADF0,0x0051ADF0);
	ASSIGN(noxGetUnitsInRect,0x00517C10);
	ASSIGN(oldCreateAtPart,0x004DAA55);// Возврат
	ASSIGN(noxAlloc,0x00403560);
	ASSIGN(noxCAlloc,0x004041D0);
	ASSIGN(noxFree,0x0040425D);

	InjectJumpTo(0x004DAA50,&noxMyCreateAt);

//	InjectJumpTo(0x00507250,&newScriptPopValue);
//	InjectJumpTo(0x00507230,&newScriptPushValue);

	ASSIGN(noxThingTypeByName,0x4E3AA0);
	ASSIGN(noxThingNameByType,0x4E3A80);

	int Top=lua_gettop(L);
	lua_pushcfunction(L,&bitOrL);
	lua_setglobal(L,"bitOr");
	lua_pushcfunction(L,&bitAndL);
	lua_setglobal(L,"bitAnd");
	
	luaL_dostring(L,"math.randomseed( os.time() )");

	lua_createtable(L,0,40);/// Client enviroment
		lua_newtable(L);
			lua_pushvalue(L,LUA_GLOBALSINDEX);
			lua_setfield(L,-2,"__index");
		lua_setmetatable(L,-2);/// теперь клиент пронаследован от _G
		lua_pushvalue(L,-1);
		lua_setfield(L,LUA_REGISTRYINDEX,"client");

	lua_newtable(L); /// таблица meta2
		lua_pushvalue(L,-2);
		lua_setfield(L,-2,"__index");
	// meta2,client		
	lua_createtable(L,0,40);/// Server enviroment
		lua_pushvalue(L,-2);
		lua_setmetatable(L,-2);/// теперь сервер пронаследован от клиента
	lua_setfield(L,LUA_REGISTRYINDEX,"server");

	lua_pushcfunction(L,&setTimeoutL); 
	// очень важная функция, ее надо в реестр луа класть 
	// чтобы нельзя было удалить случайно
	lua_pushvalue(L,-1); 
	lua_setfield(L,LUA_REGISTRYINDEX,"setTimeout");
	registerServerVar("setTimeout");

	registerserver("unitGetAround",&getUnitsAround);
	
	lua_pushcfunction(L,&printL);
	lua_setglobal(L,"print");
	lua_pushcfunction(L,&getFrameCounterL);
	lua_setglobal(L,"getFrameCounter");
	lua_pushcfunction(L,&setGameFlagsL);
	lua_setglobal(L,"gameFlags");
	lua_getglobal(L,"string");
	lua_getfield(L,-1,"format");
	lua_setglobal(L,"printf");
	lua_pop(L,1);
	lua_pushlightuserdata(L,&setTimeoutL);
	lua_newtable(L);
	lua_settable(L,LUA_REGISTRYINDEX);
	lua_pushlightuserdata(L,&timeoutNextId);
	lua_newtable(L);
	lua_settable(L,LUA_REGISTRYINDEX);

	lua_pushcfunction(L,&getThingTypeL);
	lua_setglobal(L,"getThingType");
	lua_pushcfunction(L,&setThingTypeL);
	lua_setglobal(L,"setThingType");
	
	lua_pushcfunction(L,&getClassNameL);
	lua_setglobal(L,"getThingName");

	registerserver("unitClass",&getUnitClassL);
	registerserver("unitFlags",&getUnitFlagsL);


	lua_pushcfunction(L,&getPtrPtrL);
	lua_setglobal(L,"getPtrPtr");
	lua_pushcfunction(L,&getPtrIntL);
	lua_setglobal(L,"getPtrInt");
	lua_pushcfunction(L,&getPtrFloatL);
	lua_setglobal(L,"getPtrFloat");
	lua_pushcfunction(L,&getPtrShortL);
	lua_setglobal(L,"getPtrShort");
	lua_pushcfunction(L,&setPtrFloatL);
	lua_setglobal(L,"setPtrFloat");
	lua_pushcfunction(L,&setPtrIntL);
	lua_setglobal(L,"setPtrInt");
	lua_pushcfunction(L,&setPtrPtrL);
	lua_setglobal(L,"setPtrPtr");
	lua_pushcfunction(L,&setPtrShortL);
	lua_setglobal(L,"setPtrShort");
	lua_pushcfunction(L,&setPtrByteL);
	lua_setglobal(L,"setPtrByte");
	lua_pushcfunction(L,&getPtrByteL);
	lua_setglobal(L,"getPtrByte");

	lua_pushcfunction(L,&memAllocL);
	lua_setglobal(L,"memAlloc");
	lua_pushcfunction(L,&memFreeL);
	lua_setglobal(L,"memFree");

	mapUtilInit(L);
	initFilesystem();
	consoleInit();
	adminInit(L);
	initAudServer(L);
	reactInit();

	windowsInit();
	clientViewInit();
	unitInit();
	mapInit();
	tilesInit();
	unitDefsInit();
	spellsInit();
	netInit();
	keysInit();
	playerInit();
	spriteInit();
	spellListInit();
	guiInit();
	scoreInit(L);
	waypointsInit();
	cliUntilInit();
	initAuthData();

	InjectJumpTo(0x00443C80,&onConCmd);// Функция реакции на консольную команду
	InjectOffs(0x4D2AB5,&onEachFrame);

#include "lua/binClient/clientOnJoin.lua.inc"

	lua_getfield(L,LUA_REGISTRYINDEX,"client");
	lua_setfenv(L,-2);
	lua_pcall(L,0,0,0);
#include "lua/binServer/chatMode.lua.inc"
	lua_getfield(L,LUA_REGISTRYINDEX,"server");
	lua_setfenv(L,-2);
	lua_pcall(L,0,0,0);
#include "lua/binGlobal/dofile.lua.inc"
	
#include "lua/binGlobal/filesystem.inc.lua.inc"

	lua_settop(L,Top);

	//lua_pushnil(L);
	//lua_setglobal(L,"os");/// выкинуть вон небезопасную таблицу

	if (0==luaL_loadfile(L, "autoexec.lua"))
	{
		lua_getfield(L,LUA_REGISTRYINDEX,"server");
		lua_setfenv(L,-2);
		lua_pcall(L, 0, 0, 0);
	}
	//MessageBox(0,"!",0,0);
};