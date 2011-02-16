#include "stdafx.h"
#include <list>

int (__cdecl *getTTByNameSpriteMB)(void *Key);

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
		lua_pushlightuserdata(L,cliSetTimeoutL);/// функции
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
		lua_pushlightuserdata(L,cliSetTimeoutL);/// таблица функций
		lua_gettable(L,LUA_REGISTRYINDEX);

		for (std::list<cliTimeoutListRec>::iterator I=cliTimeoutList.begin();I!=cliTimeoutList.end();)
		{
			if ( Time < I->Frame )
				break;
			lua_pushinteger(L,I->Id);
			if (I->Frame == Time)
			{
				lua_gettable(L,-2);
				if(lua_type(L,-1)==LUA_TFUNCTION)
				{
				/*	lua_getfenv(L,-1);
					lua_pushvalue(L,-2);
					lua_getfield(L,-2,"conOutput");// conOutput функция енв функция
					lua_insert(L,-2);// функция conOutput енв функция
					lua_pushnil(L);
					lua_setfield(L,-4,"conOutput"); */	

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
			call getTTByNameSpriteMB
			call cliOnEachFrame
			push 0x47582C 
			ret
		}
	}


}

extern void InjectJumpTo(DWORD Addr,void *Fn);

void cliUntilInit()
{
	ASSIGN(getTTByNameSpriteMB,0x044CFC0);



	lua_pushcfunction(L,&cliSetTimeoutL); 
	// очень важная функция, ее надо в реестр луа класть 
	// чтобы нельзя было удалить случайно
	lua_pushvalue(L,-1); 
	lua_setfield(L,LUA_REGISTRYINDEX,"CliSetTimeout");
	registerClientVar("cliSetTimeout");

	InjectJumpTo(0x475827,&asmToCliTimer);

}