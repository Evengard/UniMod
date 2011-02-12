#include "stdafx.h"
#include <math.h>

BYTE *spellDefPtr=0;
extern void *(__cdecl *gLoadImg)(const char *Name);
extern int mySelectedSpell;// спелл, выбранный в книге в данный момент
extern float (__cdecl *getFloatByName)(const char *Name);

void *(__cdecl *spellAbilGetIcon)(int Spell,int Enabled);

DWORD __cdecl mySpellGetFlags(int Spell)
{
	DWORD Ret=0;
	int Top=lua_gettop(L);
	if (Spell==0x100)
	{
		getClientVar("bookSpellList");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return 0;
		}
		lua_pushinteger(L,mySelectedSpell+1);
		lua_gettable(L,-2);
		Spell=lua_tointeger(L,-1);
		lua_settop(L,Top);
	}
	if (Spell<0x100)
	{
		Ret=*((DWORD*)(spellDefPtr + 0x10 + Spell*0x50));
		return Ret;
	}
	getClientVar("spellData");
	if (lua_type(L,-1)!=LUA_TTABLE)
	{
		lua_settop(L,Top);
		return 0;
	}
	lua_pushinteger(L,Spell);
	lua_gettable(L,-2);
	if (lua_type(L,-1)!=LUA_TTABLE)
	{
		lua_settop(L,Top);
		return 0;
	}
	lua_getfield(L,-1,"flags");
	Ret=lua_tointeger(L,-1);
	lua_settop(L,Top);
	return Ret;
}
int __cdecl mySpellIsEnabled(int Spell)
{
	int Ret=0;
	int Top=lua_gettop(L);
	if (Spell==0x100)
	{
		getClientVar("bookSpellList");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return 0;
		}
		lua_pushinteger(L,mySelectedSpell+1);
		lua_gettable(L,-2);
		Spell=lua_tointeger(L,-1);
		lua_settop(L,Top);
	}
	getClientVar("spellData");
	if (lua_type(L,-1)!=LUA_TTABLE)
	{
		lua_settop(L,Top);
		return 0;
	}
	lua_pushinteger(L,Spell);
	lua_gettable(L,-2);
	if (lua_type(L,-1)!=LUA_TTABLE)
	{
		if (lua_type(L,-1)==LUA_TNUMBER)
			Ret=*((int*)(spellDefPtr + 0x14 + Spell*0x50));
		lua_settop(L,Top);
		return Ret;
	}

	lua_getfield(L,-1,"disabled");
	Ret=(0==lua_toboolean(L,-1) )?0:1;
	lua_settop(L,Top);
	return Ret;
}
const wchar_t* __cdecl mySpellLoadName(int Spell)
{
	const wchar_t* Ret=0;
	int Top=lua_gettop(L);
	if (Spell==0x100)
	{
		getClientVar("bookSpellList");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return 0;
		}
		lua_pushinteger(L,mySelectedSpell+1);
		lua_gettable(L,-2);
		Spell=lua_tointeger(L,-1);
		lua_settop(L,Top);
	}
	if (Spell<0x100)
	{
		if (Spell<0 || Spell>ABIL_TO_SPELL )
			return L"(null)";
		if (0==*((int**)(spellDefPtr + 0x18 + Spell*0x50)))
			return L"(not set)";
		return *((wchar_t**)(spellDefPtr + 0x0 + Spell*0x50));
	}
	getClientVar("spellData");
	lua_pushinteger(L,Spell);
	lua_gettable(L,-2);
	if (lua_type(L,-1)!=LUA_TTABLE)
	{
		lua_settop(L,Top);
		return L"(wrong)";
	}
	lua_getfield(L,-1,"name");
	static wchar_t Buf[200]=L"";/// ох ненадежна€ иде€ эти статики....
	size_t Len=0;
	const char *S=lua_tolstring(L,-1,&Len);
	if (Len>0)
	{
		mbstowcs(Buf,S,min(Len,199));
		Ret=Buf;
	}
	else
		Ret=L"(null)";
	lua_settop(L,Top);
	return Ret;
}
namespace
{
	DWORD __cdecl mySpellHaveFlags(int Spell,DWORD Flags)
	{
		return ((mySpellGetFlags(Spell) & Flags) == 0)?0:1;
	}
	const wchar_t* __cdecl mySpellLoadDesc(int Spell)
	{
		const wchar_t* Ret=0;
		int Top=lua_gettop(L);
		if (Spell==0x100)
		{
			getClientVar("bookSpellList");
			if (lua_type(L,-1)!=LUA_TTABLE)
			{
				lua_settop(L,Top);
				return 0;
			}
			lua_pushinteger(L,mySelectedSpell+1);
			lua_gettable(L,-2);
			Spell=lua_tointeger(L,-1);
			lua_settop(L,Top);
		}
		if (Spell<0x100)
		{
			if (Spell<0 || Spell>ABIL_TO_SPELL )
				return L"(null)";
			wchar_t *Ret=*((wchar_t**)(spellDefPtr + 0x4 + Spell*0x50));
			if (Ret==NULL)
				Ret=L"(null)";
			return Ret;
		}
		getClientVar("spellData");
		lua_pushinteger(L,Spell);
		lua_gettable(L,-2);
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return L"(wrong)";
		}
		lua_getfield(L,-1,"description");
		static wchar_t Buf[200]=L"";/// ох ненадежна€ иде€ эти статики....
		size_t Len=0;
		const char *S=lua_tolstring(L,-1,&Len);
		if (Len>0)
		{
			mbstowcs(Buf,S,min(Len,199));
			Ret=Buf;
		}
		else
			Ret=L"(null)";
		lua_settop(L,Top);
		return Ret;
	}
	int __cdecl mySpellManaCost(int Spell,int Type)//1=ok 2=trap
	{
		int Ret=0;
		int Top=lua_gettop(L);
		if (Spell==0x100)
		{
			getClientVar("bookSpellList");
			if (lua_type(L,-1)!=LUA_TTABLE)
			{
				lua_settop(L,Top);
				return 0;
			}
			lua_pushinteger(L,mySelectedSpell+1);
			lua_gettable(L,-2);
			Spell=lua_tointeger(L,-1);
			lua_settop(L,Top);
		}
		if (Spell<0x100)
		{
			if (Spell>=0 || Spell<ABIL_TO_SPELL )
				Ret=*((BYTE*)(spellDefPtr + 0x3E + Spell*0x50));
			if (Type==2)
			{
				if (Spell==0x18)
					Ret=floor(getFloatByName("EnergyBoltTrapCost"));
				else if(Spell==0x2B)
					Ret=floor(getFloatByName("LightningTrapCost"));
				else if(Spell==0x38)
					Ret=floor(getFloatByName("ManaBombTrapCost"));
			}
			return Ret;
		}
		getClientVar("spellData");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return 0;
		}
		lua_pushinteger(L,Spell);
		lua_gettable(L,-2);
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return 0;
		}
		lua_getfield(L,-2,"mana");
		if (Type==2)
		{
			lua_getfield(L,-2,"manaTrap"); // если спрашивали дл€ ловушки
			if (lua_type(L,-1)==LUA_TNIL)
				lua_pop(L,1);
		}
		Ret=lua_tointeger(L,-1);
		lua_settop(L,Top);
		return Ret;
	}
	void *__cdecl mySpellLoadIcon(int Spell)
	{
		void *Ret=spellAbilGetIcon(1,1);
		int Top=lua_gettop(L);
		if (Spell==0x100)
		{
			getClientVar("bookSpellList");
			if (lua_type(L,-1)!=LUA_TTABLE)
			{
				lua_settop(L,Top);
				return Ret;
			}
			lua_pushinteger(L,mySelectedSpell+1);
			lua_gettable(L,-2);
			Spell=lua_tointeger(L,-1);
			lua_settop(L,Top);
		}

		if (Spell<0x100)
		{
			if (Spell<0 || Spell>ABIL_TO_SPELL )
				return Ret;
			return *((void**)(spellDefPtr + 0x8 + Spell*0x50));
		}
		getClientVar("spellData");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return Ret;
		}
		lua_pushinteger(L,Spell);
		lua_gettable(L,-2);
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return NULL;
		}

		lua_getfield(L,-1,"icon");
		if (lua_type(L,-1)==LUA_TLIGHTUSERDATA)
			Ret=lua_touserdata(L,-1);
		else 
		{
			if (lua_type(L,-1)==LUA_TNUMBER)
				Ret=mySpellLoadIcon(lua_tointeger(L,-1) );
			else if (lua_type(L,-1)==LUA_TSTRING)
				Ret=gLoadImg(lua_tostring(L,-1));
			if (Ret!=0)
			{
				lua_pushlightuserdata(L,Ret);
				lua_setfield(L,-3,"icon");
			}
		}
		lua_settop(L,Top);
		return Ret;
	}

	BYTE *mySpellSyllList(int Spell) // возвращает последовательность дл€ показа слогов
	{
		int Top=lua_gettop(L);
		BYTE Ret[10]={0};
		if (Spell==0x100)
		{
			getClientVar("bookSpellList");
			if (lua_type(L,-1)!=LUA_TTABLE)
			{
				lua_settop(L,Top);
				return Ret;
			}
			lua_pushinteger(L,mySelectedSpell+1);
			lua_gettable(L,-2);
			Spell=lua_tointeger(L,-1);
			lua_settop(L,Top);
		}

		if (Spell<0x100)
		{
			if (Spell<0 || Spell>ABIL_TO_SPELL )
				return Ret;
			return (spellDefPtr + 0x1C + Spell*0x50);
		}
		getClientVar("spellData");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return Ret;
		}
		lua_pushinteger(L,Spell);
		lua_gettable(L,-2);
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return Ret;
		}

		lua_getfield(L,-1,"syll");
		if (lua_type(L,-1)==LUA_TSTRING)
		{
			const char *S=lua_tostring(L,-1);
			if (S!=NULL)
			{

				for (BYTE *B=Ret;*S!=0;S++,B++)
				{
					if (*S>='0' && *S<='9')
						*B=*S-'0';
				}
			}
		}
		lua_settop(L,Top);
		return Ret;		
	}

	void *__cdecl myAbilLoadIcon(int Spell,int Recently)
	{
		void *Ret=0;
		int Top=lua_gettop(L);
		if (Spell==0x100)
		{
			getClientVar("bookSpellList");
			if (lua_type(L,-1)!=LUA_TTABLE)
			{
				lua_settop(L,Top);
				return 0;
			}
			lua_pushinteger(L,mySelectedSpell+1);
			lua_gettable(L,-2);
			Spell=lua_tointeger(L,-1);
			lua_settop(L,Top);
		}

		if (Spell<0x100)
		{
			return spellAbilGetIcon(Spell,Recently);
		}
		getClientVar("spellData");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return NULL;
		}
		lua_pushinteger(L,Spell);
		lua_gettable(L,-2);
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return NULL;
		}

		lua_getfield(L,-1,"icon");
		if (lua_type(L,-1)==LUA_TLIGHTUSERDATA)
			Ret=lua_touserdata(L,-1);
		else 
		{
			if (lua_type(L,-1)==LUA_TNUMBER)
				Ret=mySpellLoadIcon(lua_tointeger(L,-1) );
			else if (lua_type(L,-1)==LUA_TSTRING)
				Ret=gLoadImg(lua_tostring(L,-1));
			if (Ret!=0)
			{
				lua_pushlightuserdata(L,Ret);
				lua_setfield(L,-3,"icon");
			}
		}
		lua_settop(L,Top);
		return Ret;
	}
	void *__cdecl mySpellLoadIconH(int Spell)
	{
		void *Ret=spellAbilGetIcon(2,1);
		int Top=lua_gettop(L);
		if (Spell==0x100)
		{
			getClientVar("bookSpellList");
			if (lua_type(L,-1)!=LUA_TTABLE)
			{
				lua_settop(L,Top);
				return Ret;
			}
			lua_pushinteger(L,mySelectedSpell+1);
			lua_gettable(L,-2);
			Spell=lua_tointeger(L,-1);
			lua_settop(L,Top);
		}

		if (Spell<0x100)
		{
			if (Spell<0 || Spell>ABIL_TO_SPELL )
				return Ret;
			return *((void**)(spellDefPtr + 0x0C + Spell*0x50));
		}
		getClientVar("spellData");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return Ret;
		}
		lua_pushinteger(L,Spell);
		lua_gettable(L,-2);
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return Ret;
		}
		lua_getfield(L,-1,"iconH");
		if (lua_type(L,-1)==LUA_TLIGHTUSERDATA)
			Ret=lua_touserdata(L,-1);
		else 
		{
			if (lua_type(L,-1)==LUA_TNUMBER)
				Ret=mySpellLoadIconH(lua_tointeger(L,-1) );
			else if (lua_type(L,-1)==LUA_TSTRING)
				Ret=gLoadImg(lua_tostring(L,-1));
			if (Ret!=0)
			{
				lua_pushlightuserdata(L,Ret);
				lua_setfield(L,-3,"iconH");
			}
		}
		lua_settop(L,Top);
		return Ret;
	}
	// возвращает в луа инфу о спелле
	int spellInfo(lua_State *L)
	{
		if (lua_type(L,1)!=LUA_TNUMBER && lua_type(L,1)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int Spell=lua_tointeger(L,1);
		lua_newtable(L);
		int Top=lua_gettop(L);
		if (Spell==0x100)
		{
			getClientVar("bookSpellList");
			if (lua_type(L,-1)!=LUA_TTABLE)
			{
				lua_settop(L,Top);
				return 0;
			}
			lua_pushinteger(L,mySelectedSpell+1);
			lua_gettable(L,-2);
			Spell=lua_tointeger(L,-1);
			lua_settop(L,Top);
		}
		if (Spell>0x100)
		{
			getClientVar("spellData");
			lua_pushinteger(L,Spell);
			lua_gettable(L,-2);
			if (!lua_isnil(L,-1))
			{
				lua_pushnil(L);
				while(lua_next(L,-2)!=0)
				{
					lua_pushvalue(L,-2);
					lua_insert(L,-2);
					lua_settable(L,-6); // value,key,key,src,spellData,dst
				}
			}
			lua_pop(L,3);
			return 1;
		}
		char Name[60];
		const wchar_t *WName=mySpellLoadName(Spell);
		if (WName!=NULL)
		{
			wcstombs(Name,WName,wcslen(WName));;
			lua_pushstring(L,Name);
			lua_setfield(L,-2,"name");
		}
		lua_pushnumber(L,mySpellGetFlags(Spell));
		lua_setfield(L,-2,"flags");

		return 1;
	}
}
extern void InjectJumpTo(DWORD Addr,void *Fn);
extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectAddr(DWORD Addr,void *Fn);
void spellGetInit()
{
	ASSIGN(spellAbilGetIcon,0x00425310);

	/// абилки в просмотре отдельной книги за воина -- потом поправить
	InjectOffs(0x0045CB5C+1,&myAbilLoadIcon); 

	// проверка на наличие флагов в спелле
	InjectJumpTo(0x00424A50,&mySpellHaveFlags);
	InjectJumpTo(0x00424A70,&mySpellGetFlags);
	InjectJumpTo(0x00424A90,&mySpellLoadIcon); // правим функцию обычной иконки
	InjectJumpTo(0x00424AB0,&mySpellLoadIconH); // и подсвеченной иконки
	InjectJumpTo(0x00424930,&mySpellLoadName); // правим функцию определени€ имени спелла
	InjectJumpTo(0x00424A30,&mySpellLoadDesc); // и описани€
	InjectJumpTo(0x004249A0,&mySpellManaCost); // и цены в мане
	InjectJumpTo(0x00424A20,&mySpellSyllList); // правим функцию показа слогов
	InjectJumpTo(0x00424B70,&mySpellIsEnabled);// и разрешенности
	
	// правим произношение на длинные номера спелов
	InjectAddr(0x0049BBC0,(void*)0x2964A151);
	InjectAddr(0x0049BBC4,(void*)0xC0850071);
	InjectAddr(0x0049BBD4,(void*)0xFFFFFFFF);
	InjectAddr(0x0049BC70,(void*)0x39FFFFFF);
	InjectAddr(0x0049BB80,(void*)0x0424448B);
	InjectAddr(0x0049BB88,(void*)0x64A30084);
	InjectAddr(0x0049BBB0,(void*)0x64A3C033);
	InjectAddr(0x0049BBB4,(void*)0xC3007129);
//	InjectAddr(0x0049BBB0,(void*)0x296405C4);


	registerclient("spellInfo",spellInfo);
}