#include "stdafx.h"

BYTE **clientPlayerInfoPtr;
int (__cdecl *spellGetValidMB)(int Spell);
void *(__cdecl *gLoadImg)(const char *Name);

void (__cdecl *drawSetTextColor)(DWORD Color);
void (__cdecl *drawGetStringSize)(void *FontPtr,const wchar_t*String,
			int *W,int *H,DWORD Flags);
void (__cdecl *drawString)(void *FontPtr,const wchar_t*String,int X,int Y);
void (__cdecl *drawUpdateMB)(int A,int B);
void (__cdecl *wndSetTooltip)(void *Wnd, const wchar_t *Str);
void (__cdecl *netSpellRewardCli)(int Spell, int newLevel,int ShowBook, int AddToBar);

int (__cdecl *audSyllStartSpeak)(int SpellN);/// �������� �������� �����


extern DWORD __cdecl mySpellGetFlags(int Spell);
extern int __cdecl mySpellIsEnabled(int Spell);
extern const wchar_t * __cdecl mySpellLoadName(int Spell);

DWORD *guiSpellListedNumbers;// ������ ������� - ����� �� ���� ������������ ������ ���� ������� - 3�

DWORD *bookWndState;
DWORD *bookIsMonster;
DWORD *bookColorNormal;
DWORD *bookColorDisabled;
DWORD *bookSpellSelected;

DWORD *bookSpellDnDType;  /// ���� ����� �����-�� �����
DWORD *bookSpellNumberDnD;/// ����� ������ ����� �����

int *bookSelSpell;/// ������� ������� �����

int *bookPagesList;/// ������� ������� �������� ������ �������
int *bookLineHeight;
int *bookListHidden;/// �������� ������� ������� - ������ ������� �� ����������
int *bookListTotalCount;/// ������� ����� ������

int mySelectedSpell;/// ������ ��� ���������� ����� �������� ��� ������� �������

/*
0100 0000  - ����� �����
0200 0000  - ����� ���� (1)
0400 0000  - ����� �����(2)

*/
/*
bookSpellList - ������ ������, ��������� ����� ������, � ������� ����������
spellData - ������ ���� ������, �� ������ �������, ����� �� ��������, ����� �� ��� �������� ������� ������

��� ��������� ���� - ����� ������� ������ � ������ ������ ����������� ����� 0x100 
� ���-������� ���������� ������ �� �������� ������ �� mySelectedSpell

��� ������ �� ����������� ������:
- ��������� DnD �� ��������� ������ ������
- � ������ ������� ���������������� ������
- �������������
- ��������� �������
- ��������� �������� ������ 
- ������� ������ �����
- ��������� ������ ���� � ��������


*/

namespace
{
	void *myDragIcon=NULL;///������ ��� ���������
	int nextSpellIndex=0x101; /// ����� ���������� ������
	
	bool spellIsSummon(int Spell)
	{
		//sfCreature
		DWORD F=mySpellGetFlags(Spell);
		const wchar_t *Name=mySpellLoadName(Spell);
		Name++;
		return (Spell >=75) && (Spell<114);
	}
	int __cdecl mySpellReward(int Spell,BYTE *PlayerInfo,int PlayerClass)
		/// �������� ������� 1 ���� ����� �����������
		/// 0 - ���� �� ����������� (�������� ��� ���)
	{
		int Top=lua_gettop(L);
		getClientVar("bookSpellList");
		if (PlayerClass==0) // ��������� ������ � ������
			Spell+=ABIL_TO_SPELL;
		int i,n=lua_objlen(L,-1);
		for (i=1;i<=n+1;i++)
		{
			lua_rawgeti(L,-1,i);
			if (lua_tointeger(L,-1)==Spell)
				break;
			lua_settop(L,Top+1);
		}
		if (i==n+1)
		{
			lua_rawseti(L,-2,i); // ������ ������ - ���������
			lua_pushinteger(L,Spell);
		}
		getClientVar("spellData");
		lua_pushinteger(L,Spell);
		lua_gettable(L,-2);
		if (lua_type(L,-1)==LUA_TNUMBER)/// ���� ��� ����� - ����� ������
		{
			/// todo - �������� �������
		}
		else
		{
			lua_getfield(L,-1,"level");
			lua_pushinteger(L,lua_tointeger(L,-1)+1);
			lua_getfield(L,-3,"level");
		}
		lua_settop(L,Top);
		return 1;
	}
	void __declspec(naked) mySpellRewardPre()
		// ���������� ����� ������ �������� ����� ����������� �����
		// ����� ����� ���� ���������� ������ ����������� �� ���� ����� ������
		// ���� �� ��� ��� ������
	{
		__asm
		{
			pop eax // ������� ����� ��������
			push ebx // ������ ������
			push ebp // ������ �����
			call mySpellReward
			add esp,0xC
			cmp ebp,0x22
			jnz l1
			mov ecx,eax /// ��������� ����
			push 0x0045D0D4
			ret
l1:
			test eax,eax
			jz l2
			cmp [esp+0x1C],edi // �������� - ������� ����� ��� ���
			jz l2
			push 0x0045D117 // ���������� �����
			ret
l2:
			push 0x0045D139 // ������ �� ����������
			ret
		};
	}

	void drawDefAbil(int Spell,int plrClass,int X,int Y)
	{
	}
	void drawDefCreature(int Spell,int plrClass,int X,int Y)
	{
		drawSetTextColor(
			((2==plrClass) && !mySpellIsEnabled(Spell))? // ���� ����
				*bookColorDisabled:
				*bookColorNormal
			);
		const wchar_t *Name=mySpellLoadName(Spell);
		int W=0;
		drawGetStringSize(NULL,Name,&W,NULL,0x80);
		drawString(NULL,Name,X-W/2,Y);
	}

	void drawDefSpell(int Spell,int plrClass,int X,int Y)
	{
		drawSetTextColor(
			!mySpellIsEnabled(Spell)?
				*bookColorDisabled:
				*bookColorNormal
			);
		const wchar_t *Name=mySpellLoadName(Spell);
		int W=0;
		drawGetStringSize(NULL,Name,&W,NULL,0x80);
		drawString(NULL,Name,X-W/2,Y);
	}
	void drawUniItem(int X,int Y,bool isCreatures)
	{
		int i=-1;
		lua_getfield(L,i--,"flags");
		DWORD Flags=lua_tointeger(L,-1);
		if ( (0!=Flags&sfCreature) ^ isCreatures )
			return;
		DWORD Color=*bookColorNormal;
		lua_getfield(L,i--,"color");
		if (lua_type(L,-1)==LUA_TNUMBER)
			Color=lua_tointeger(L,-1);
		lua_getfield(L,i--,"disabled");
		drawSetTextColor(
			0!=lua_toboolean(L,-1)?
				*bookColorDisabled:
				Color
			);
		wchar_t Name[120];
		size_t Len=0;
		lua_getfield(L,i--,"name");
		const char *NameS=lua_tolstring(L,-1,&Len);
		if (Len!=0)
		{
			if (Len>119)
				Len=119;
			mbstowcs(Name,NameS,120);
			Name[Len]=0;
		}
		else
			wcscpy(Name,L"(wrong)");
		int W=0;
		drawGetStringSize(NULL,Name,&W,NULL,0x80);
		drawString(NULL,Name,X-W/2,Y);
	}
	void drawSpellList(int PageN,int LinesPerPage,int X,int Y)
	{ /// � ���� ������� ����� ���� ������������ ������ ��������������
		int Top=lua_gettop(L);
		getClientVar("bookSpellList");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return;
		}
		bool isCreatures=(1==*bookWndState);

		int Count=lua_objlen(L,-1);// ������� ������� ��� ��������
		*bookListTotalCount=Count;
		*bookPagesList=1 + (Count+LinesPerPage)/LinesPerPage;
		*bookListHidden=0;
		int i=1+PageN*LinesPerPage*2,right=i+LinesPerPage;
		if (i+2*LinesPerPage<Count)
			Count=i-1+2*LinesPerPage;
		X+=78;
		Y+=19;
		int Y0=Y;
		int plrClass=(*clientPlayerInfoPtr)[0x8CB];
		getClientVar("spellData");
		if (lua_type(L,-1)!=LUA_TTABLE)
			return;

		for (;i<=Count;i++)
		{
			if (i==right)// ��������� �� ������ ��������
			{
				X+=199-78;
				Y=Y0;
			}

			lua_settop(L,Top+2);// spellData bookList
			lua_rawgeti(L,-2,i);
			int V=0;
			V=lua_tointeger(L,-1);
			lua_gettable(L,-2);
			if (lua_type(L,-1)==LUA_TNUMBER) // ����������� �����
			{
				int Spell=lua_tointeger(L,-1);
				if (spellIsSummon(Spell) ^ isCreatures)
					continue;
				if (isCreatures) 
					drawDefCreature(Spell,plrClass,X,Y);
				else if (0==plrClass)
					drawDefAbil(Spell,plrClass,X,Y);
				else
					drawDefSpell(Spell,plrClass,X,Y);
			}
			else if(lua_type(L,-1)==LUA_TTABLE)
				drawUniItem(X,Y,isCreatures);
			Y+=*bookLineHeight;
		}
		lua_settop(L,Top);
	}
	int __cdecl myBookUpdateList(int PlayerClass)
	{
		int Top=lua_gettop(L);
		int Ret=0;
		getClientVar("bookSpellList");
		if (lua_type(L,-1)==LUA_TTABLE)
			Ret=lua_objlen(L,-1);
		lua_settop(L,Top);
		return Ret;
	}
	void mySpellSelectOne()
	{
		*bookSelSpell=3;
		guiSpellListedNumbers[3]=1;
		int plrClass=(*clientPlayerInfoPtr)[0x8CB];
		if (plrClass==0)
			return; //TODO: ��� ����� ���� �� ����� ���� ������
		guiSpellListedNumbers[3]=0x100; // ��� ��������� - ����������� ����������� �����

	}

	void __declspec(naked) mySpellSelectOnePre()
	{
		__asm
		{
			push eax
			call mySpellSelectOne
			pop eax
			cmp eax,1
			jne l1
			push 0x0045C00E
			ret
l1:
			push 0x0045C287
			ret 
		}
	}
/*
	void *__cdecl getAbilIconForDnDPre(int Spell,int Enabled)
	{
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
			return spellAbilGetIcon(Spell,Enabled);
		return myDragIcon;
	}

	void *__cdecl getSpellIconForDnDPre(int Spell)
	{
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
			return mySpellLoadIcon(Spell);
		return myDragIcon;
	}
*/
		/// =1 - �����
		/// =2 - ������
		/// =3 - ����� ��� �������
	void getSpellForDnD(int SelectedId,int Type)
	{
		int Top=lua_gettop(L);
		int Spell=0;

		getClientVar("bookSpellList");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return;
		}
		getClientVar("spellData");
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			lua_settop(L,Top);
			return;
		}
		lua_pushinteger(L,SelectedId+1);
		lua_gettable(L,-3);
		if (lua_type(L,-1)!=LUA_TNUMBER)
		{
			lua_settop(L,Top);
			return;
		}
		int X=0;
		X=lua_tonumber(L,-1);
		lua_gettable(L,-2);
		if (lua_type(L,-1)==LUA_TNUMBER) // ����������� �����
		{
			lua_pushlightuserdata(L,&getSpellForDnD);
			lua_pushnil(L);
			lua_settable(L,LUA_REGISTRYINDEX);/// ������� ��������� �����

			Spell=lua_tointeger(L,-1)+((Type==3)?0x4A:0);
			/// ����� ���� ��������� ����� ��� ����� ������� �������
		}
		else
		{
			lua_pushlightuserdata(L,&getSpellForDnD);
			lua_pushvalue(L,-2);
			lua_settable(L,LUA_REGISTRYINDEX);/// ������� ��������� �����

			lua_getfield(L,-2,"index");
			Spell=lua_tointeger(L,-1);
			if (Spell<0x101)// ������� ����� �����
			{
				lua_pushinteger(L,nextSpellIndex);
				lua_setfield(L,-4,"index");

				getClientVar("spellData");
				lua_pushinteger(L,nextSpellIndex);
				lua_pushvalue(L,-5);// ������, �������,������ ������,������, ������� �������
				lua_settable(L,-3);
				Spell=nextSpellIndex++;
			}

		}

		*bookSpellSelected=Spell;
		*bookSpellDnDType=1;
		*bookSpellNumberDnD=Spell;

		lua_settop(L,Top);
		drawUpdateMB(793,100);/// ���������� �����, �� ���� ������ �����
		return;
	}
/*	void __cdecl mySpellKeyPackSetSpell(keyPack *KeyPack,int NewSpell,int KeyN)
	{
		KeyPack->keyRowPtr[KeyN].Spell=NewSpell;
		BYTE *P=(BYTE*)KeyPack->spellWndItemMB[KeyN];
		if (NewSpell<0x100)
		{
			if (P!=NULL)
				wndSetTooltip(P+24,spellLoadName(NewSpell));
			if (clientPlayerInfoPtr!=0)
				drawUpdateMB(794,100);/// ���������� �����, �� ���� ������ �����
			return;
		}
		//������ ������
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L,&getSpellForDnD);
		
		lua_gettable(L,LUA_REGISTRYINDEX);/// ������� ��������� �����
		lua_getfield(L,-1,"name");
		size_t Len;
		const char *S=lua_tolstring(L,-1,&Len);
		wchar_t Buf[200]=L"(Uni spell)";
		if (Len>0)
			mbstowcs(Buf,S,min(Len,199));
		wndSetTooltip(P+24,Buf);
		if (clientPlayerInfoPtr!=0)
			drawUpdateMB(794,100);/// ���������� �����, �� ���� ������ �����
		
		lua_settop(L,Top);
		return;
	}*/
	void __declspec(naked) getAbilForDnDPre()
	{
		__asm{
			push 2
			push ecx
			call getSpellForDnD
			add esp,8
			mov ecx,eax
			push 0x0045B85D
			ret
		};
	}
	void __declspec(naked) getCreaForDnDPre()
	{
		__asm{
			push 3
			push ecx
			call getSpellForDnD
			add esp,8
			push 0x0045B85D
			ret
		};
	}		
	void __declspec(naked) getSpellForDnDPre()
	{
		__asm{
			push 1
			push ecx
			call getSpellForDnD
			add esp,8
			push 0x0045B85D
			ret
		};
	}		
	void __declspec(naked) drawSpellListPre()
	{
		__asm{
		push [esp+0x22C-0x214]
		push [esp+0x230-0x218]
		push ecx
		push esi
		call drawSpellList
		add esp,0x10
		push 0x0045C78D
		ret
		}
	}

	void onSpellStart(BYTE *Packet)
	{
		audSyllStartSpeak(*((DWORD*)Packet));
	}
	void onSpellSync(BYTE *Packet)
	{
		DWORD StartIdx=*((DWORD *)(Packet));
	}
}
extern void InjectJumpTo(DWORD Addr,void *Fn);
extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectAddr(DWORD Addr,void *Fn);

void mapLoadSpells()
{
	int Top=lua_gettop(L);
	int plrClass=(*clientPlayerInfoPtr)[0x8CB];
	luaL_loadstring(L,
		"local l={...}; "
		"table.sort(l[1],"
		"function (a,b) "
		"return spellInfo(a).name < spellInfo(b).name "
		"end)"
		);
	lua_getfield(L,LUA_REGISTRYINDEX,"client");
	lua_setfenv(L,-2);
	getClientVar("bookSpellList");
	int Cnt=1;
		lua_pushinteger(L,0x101);
		lua_rawseti(L,-2,Cnt++);
		lua_pushinteger(L,0x102);
		lua_rawseti(L,-2,Cnt++);

	if (plrClass != 0)
	{
		for (int i=0;i<ABIL_TO_SPELL;i++) /// ��� ������� ������
		{
			DWORD DW=mySpellGetFlags(i);
			if ( (DW & 0x01000000) ||
					((plrClass==1) && (DW & 0x02000000)) ||
					((plrClass==2) && (DW & 0x04000000))
					)
			{
				lua_pushinteger(L,i);
				lua_rawseti(L,-2,Cnt++);
			}
		}
	}
	else
	{
		for (int i=0;i<5;i++) /// ��� ������� ������
		{
			lua_pushinteger(L,i);
			lua_rawseti(L,-2,Cnt++);
		}
	}
	if (0!=lua_pcall(L,1,0,0))
	{
		const char *C=NULL;
		C=lua_tostring(L,-1);
		C++;
	};
	getClientVar("spellData");
	lua_pushinteger(L,0x101);
	lua_newtable(L);
		lua_pushstring(L,"testSpell");
		lua_setfield(L,-2,"name");
		lua_pushinteger(L,40);
		lua_setfield(L,-2,"mana");
		lua_pushinteger(L,0x3080A0);
		lua_setfield(L,-2,"color");
		lua_pushinteger(L,0x01000008);// �����, ���������
		lua_setfield(L,-2,"flags");
		//player,targX,targY,targetUnit,data -> success
		if (0==luaL_loadstring(L,
			"local me,tx,ty=... \n"
			"local x,y = unitPos(me) \n"
			"if not mapTraceRay(x,y,tx,ty,5) then return false end \n"
			"soundMake(0x28,tx,ty) " // ���� ������������ �������
			"netPointFx(0x89,tx,ty) "
//			"soundSyll(me,1) "
			"local r=createObject('waterbarrel',tx,ty) \n"
			"unitDecay(r,600) return true"))
		{
			lua_setfield(L,-2,"servOnStart");
		}
		else
		{
			conPrintI(lua_tostring(L,-1));
			lua_pop(L,1);
		}
	lua_settable(L,-3);
	lua_settop(L,Top);
}
void spellGetInit();

void spellListInit()
{
	ASSIGN(audSyllStartSpeak,0x0049BB80);

	ASSIGN(clientPlayerInfoPtr,0x00853BB0);
	ASSIGN(spellGetValidMB,0x00424B50);

	ASSIGN(bookPagesList,0x006D3F30);

	ASSIGN(bookIsMonster,0x006D3EEC);
	ASSIGN(bookWndState,0x006D3EE8);
	ASSIGN(bookColorNormal,0x006D3EF4);
	ASSIGN(bookColorDisabled,0x006D3EF8);
	ASSIGN(bookSpellSelected,0x006D417C);
	ASSIGN(bookSelSpell,0x006D3F28);
	
	ASSIGN(bookSpellNumberDnD,0x006E037C);
	ASSIGN(bookSpellDnDType,0x006E0380);

	ASSIGN(bookLineHeight,0x006D3E14);
	ASSIGN(bookListTotalCount,0x006D4168);
	ASSIGN(bookListHidden,0x006D416C);

	ASSIGN(drawSetTextColor,0x00434390);
	ASSIGN(drawGetStringSize,0x0043F840);
	ASSIGN(drawString,0x0043F6E0);
	ASSIGN(drawUpdateMB,0x00452D80);
	ASSIGN(wndSetTooltip,0x0046B000);
	ASSIGN(gLoadImg,0x0042F970);

	ASSIGN(guiSpellListedNumbers,0x006D3F44);

	bool useMySpells=false;
	if (useMySpells) 
	{
		//////// ��������
		/// 0045DCA0 - ��� �������, ������� ���������� ��� ������� ������� ����� ����-���� 
		///    - �������������� �� ������� ���� ���� ������ ������ � ������ ����

		///��������� � ������� ������� ���� ������ � �.�. 
		InjectJumpTo(0x0045ADF0,myBookUpdateList);
		///// ��������� ������ �������/������
		InjectJumpTo(0x0045BE60,drawSpellListPre);
		InjectJumpTo(0x0045B76A,getAbilForDnDPre);
		InjectJumpTo(0x0045B801,getCreaForDnDPre);
		InjectJumpTo(0x0045B83A,getSpellForDnDPre);
		//InjectJumpTo(0x0045DC40,mySpellKeyPackSetSpell);

		/// ������ � ��������� ��������� ����� �� ����� � ���� 
		/// ����� ���� ��� ������ +74

		InjectOffs(0x0045D0C1+1,&mySpellRewardPre);
		

//		InjectOffs(0x004778E6+1,&getAbilIconForDnDPre);
//		InjectOffs(0x004778FA+1,&getSpellIconForDnDPre);

		InjectAddr(0x0045B93E+1,&mySelectedSpell);//����� bookMoveToPage ������� ���� ����������
		InjectAddr(0x0045B2A7+1,&mySelectedSpell);//����� bookLeftProc ������� ���� ����������
		InjectAddr(0x0045B2D6+1,&mySelectedSpell);//����� bookLeftProc ������� ���� ����������
		InjectAddr(0x0045B103+2,&mySelectedSpell);//����� bookRightProc ������� ���� ����������
		InjectAddr(0x0045B177+2,&mySelectedSpell);//����� bookRightProc ������� ���� ����������
		InjectAddr(0x0045B18C+2,&mySelectedSpell);//����� bookRightProc ������� ���� ����������

		InjectJumpTo(0x0045C005,mySpellSelectOnePre); // ����� �� ��������� ������������ ����� � ������ ������ �������

		InjectJumpTo(0x0045FE6F,(void*)0x0045FE81);
		
		spellGetInit();

		netRegClientPacket(upSpellStart,onSpellStart);
		netRegClientPacket(upSpellSync,onSpellSync);
		
		//InjectJumpTo(0x0045C3A2,);
	}
	lua_createtable(L,256,0);
	for (int i=1;i<ABIL_TO_SPELL+ 5 ;i++)
	{
		lua_pushinteger(L,i);	
		lua_rawseti(L,-2,i);
	}
	registerClientVar("spellData");/// ���� ��������� ��� ������ ����� ����

	lua_createtable(L,256,0);
	registerClientVar("bookSpellList");

};