#include "stdafx.h"

int (__cdecl *spellAccept)(int SpellType,void *Caster,void *CasterMB2,void *Carrier,
								SpellTargetBlock *Block,int Power);
int (__cdecl *spellGetPower)(int Spell,void *Caster);
// выключает спелл постоянного действия
void (__cdecl *spellCancelDurSpell)(int Spell,void *Caster);
// снимает какой-либо бафф
void (__cdecl *spellBuffOff)(void *Target,int BuffN);
int (__cdecl *spellDefHasFlags)(int Spell,DWORD Flags);
void (__cdecl *createSpellFly)(void *Caster,void *Target,int Spell);
void (__cdecl *applyToTarget)(void *Target,int Buff,int Delay,int SpellPower);
int (__cdecl *myCastSpellByUser)(int Spell, void *Caster, SpellTargetBlock *TargetBlock);
//
extern int (__cdecl *audSyllStartSpeak)(int SpellN);
extern bigUnitStruct **unitCreatedList;
namespace
{
	/// колдует спелл кем-то (списано с нокса)
	/// когда моб чего-нить колдует - мы об этом узнаем
	int myCastSpellByUserPlayer(int Spell, void *Caster, SpellTargetBlock *TargetBlock)
	{
		bool check=true;
		if(spellDefHasFlags(Spell,0x200400)==1 && Spell!=34 && (Spell<75 || Spell>114))
		{
			if(Caster!=TargetBlock->target && spellDefHasFlags(Spell,0x600)==1)
				check=false;
			if(Caster==TargetBlock->target && spellDefHasFlags(Spell,0x600)==0)
				check=false;
		}
		if(check)
			return myCastSpellByUser(Spell, Caster, TargetBlock);
		else
		{
			// Здесь можно будет заодно кикать или банить нарушителя
			return 1;
		}
	}
	int myCastSpellByUserMob(int Spell, void *Caster, SpellTargetBlock *TargetBlock)
	{
		int R,Top=lua_gettop(L);
		
		lua_getglobal(L,"unitOnCastList");
		lua_pushlightuserdata(L,Caster);
		if (lua_type(L,-2)!=LUA_TNIL)
			lua_gettable(L,-2);

		if (spellDefHasFlags(Spell,0x20))
		{
			spellBuffOff(Caster,0x00);
			spellBuffOff(Caster,0x17);
			spellCancelDurSpell(0x43,Caster);//выключаем щит
		}
		if (
			spellDefHasFlags(Spell,0x04) &&// нужно делать муху
			TargetBlock->target!=Caster // мы не в себя колдуем
			) 
		{
			createSpellFly(Caster,TargetBlock->target,Spell);

//Ща мы найдем эту муху пока она не улетела
			if (lua_type(L,-1)==LUA_TFUNCTION)
			{
				bigUnitStruct *B=*unitCreatedList;
				for (;B!=NULL;B=(bigUnitStruct *)B->nextUnit)
				{
					if (B->parentUnit!=Caster)
						continue; // не наша
					if (B->thingType!=0x3BB)
						continue; // не муха
					break;/// 99% что она вообще первая в списке, т.к. ее только что создали
				}
				if (B) // если муху не нашли - так считаем что не сколдовалось
				{
					lua_pushinteger(L,Spell);
					lua_pushlightuserdata(L,Caster);
					lua_pushlightuserdata(L,TargetBlock->target);
					lua_pushlightuserdata(L,B);
					lua_pushnumber(L,TargetBlock->targX);
					lua_pushnumber(L,TargetBlock->targY);
					lua_pcall(L,6,0,0);
				}
			}
			R=1;
		}
		else
		{
			R=spellAccept(Spell,Caster,Caster,Caster,
				TargetBlock, spellGetPower(Spell,Caster));
			lua_pushinteger(L,Spell);
			lua_pushlightuserdata(L,Caster);
			lua_pushlightuserdata(L,TargetBlock->target);
			lua_pushnil(L);
			lua_pushnumber(L,TargetBlock->targX);
			lua_pushnumber(L,TargetBlock->targY);
			lua_pcall(L,6,0,0);

		}
		lua_settop(L,Top);
		return R;
	}
	int spellBuff(lua_State *L)
	{
		return 0;
	}
	int spellApplyL(lua_State *L)
	{
		/// Кому,Чего,Чем, [ОтКого,ГдеX,ГдеY,[SpellPower=3], Промежуточный]
		if (
			(lua_type(L,1)!=LUA_TLIGHTUSERDATA)||
			(lua_type(L,2)!=LUA_TNUMBER)||
			(lua_type(L,3)!=LUA_TLIGHTUSERDATA)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void *Targ=lua_touserdata(L,1);
		int Type=lua_tointeger(L,2);
		void *Carrier=lua_touserdata(L,3);
		void *Source=0,*Thru=0;
		SpellTargetBlock Bk;
		int i=4;
		if (lua_type(L,i)==LUA_TLIGHTUSERDATA)
			Source=lua_touserdata(L,i);
		i++;
		if (lua_type(L,i)==LUA_TNUMBER)
		{
			Bk.targX=lua_tonumber(L,i);
			i++;
			if(lua_type(L,i)!=LUA_TNUMBER)
			{
				lua_pushstring(L,"wrong args!");
				lua_error_(L);
			}
			Bk.targY=lua_tonumber(L,i);
			i++;
		}
		else
		{
			BYTE *B=(BYTE *)Targ;
			if(Targ!=NULL)
			{
				if (( 0!=(0x4 & B[8]) ))
				{
					B+=0x2EC;//контроллер
					B=*((BYTE**)B);B+=0x114;//плэеринфо
					B=*((BYTE**)B);B+=0x8EC;
					Bk.targX=*((int*)B);B+=4;
					Bk.targY=*((int*)B);
				}
				else
				{
					B+=0x38;
					Bk.targX=*((float*)B);B+=4;
					Bk.targY=*((float*)B);
				}
			}
			else
			{
				Bk.targX=Bk.targY=0;
			}
		
		}
		int Power=3;
		if (lua_type(L,i)==LUA_TNUMBER)
		{
			Power=lua_tointeger(L,i);
			i++;
		}
		if (lua_type(L,i)==LUA_TLIGHTUSERDATA)
			Thru=lua_touserdata(L,i);
		if(Thru==NULL)
			Thru=Source;
		if(Source==NULL)
		{
			Source=Thru=Carrier;
		}
		Bk.target=Targ;
		int R=spellAccept(Type,Source,Thru,Carrier,&Bk,Power);
		lua_pushinteger(L,R);
		return 1;
	}
	int spellSync(lua_State *L)
	{
		int Top=lua_gettop(L);
		byte Buf[250],*P=Buf;
		int Size;
		P+=UNIPACKET_HEAD;
		Size=0;
		
		getClientVar("spellData");
		lua_pushnil(L);
		SendBuffer Out;
		while (0!=lua_next(L,-2))
		{
			if (lua_type(L,-2)!=LUA_TNUMBER)
			{
				lua_pop(L,1);
				continue;
			}
			int Spell=lua_tointeger(L,-2);
			*((DWORD*)P)=Spell;
			if (lua_type(L,-1)==LUA_TNUMBER)
			{
			}
			netUniPacket(upSpellSync,P,Size);
			netSendNotOne(Buf,P-Buf,NULL); /// посылает всем клиентам  кроме одного
		}
		lua_settop(L,Top);
		return 0;
	}
}
extern void InjectOffs(DWORD Addr,void *Fn);
extern void (__cdecl *netPriMsg)(void *PlayerUnit,char *String,int Flag);
extern DWORD *GameFlags;
/// для кастомной ловушки надо будет просто юзать другой спелл
/// Еще надо разослать netReportSpellStat всем вокруг
void spellServDoCustom(int SpellArr[5],bool OnSelf,BYTE *MyPlayer,BYTE *MyUc)
{
	int Top=lua_gettop(L);
	getClientVar("spellData");
	lua_pushinteger(L,SpellArr[0]);
	lua_gettable(L,-2);
	if (lua_type(L,-1)!=LUA_TTABLE)
	{
		lua_settop(L,Top);
		return;
	}
	lua_getfield(L,Top+2,"mana");
	int ManaCost=lua_tointeger(L,-1);
	char TextBuf[80]="";
	BYTE *PlayerInfo=*((BYTE**)(MyUc+0x114));
	do 
	{
		if ( 1 & *((DWORD*)(PlayerInfo+0xE60)))
		{
			lua_getfield(L,Top+2,"mayObserver");
			if (0==lua_toboolean(L,-1))
			{
				netPriMsg(MyPlayer,"GeneralPrint:NoSpellWarningGeneral",0);
				break;
			}
		} else if ( 2 & *((DWORD*)(PlayerInfo+0xE60)))
		{
			lua_getfield(L,Top+2,"mayPosessed");
			if (0==lua_toboolean(L,-1))
			{
				netPriMsg(MyPlayer,"GeneralPrint:ConjureNoSpellWarning1",0);
				break;
			}
		}
		if ( 0x80 & *GameFlags)
		{
			lua_getfield(L,Top+2,"mayInChat");
			if (0==lua_toboolean(L,-1))
			{
				break;
			}
		}
		BYTE *Target=*((BYTE**)(MyUc+0x120));
		lua_newtable(L); /// внутренние данные спела - для продолжительных и т.п.
		lua_getfield(L,Top+2,"flags");
		lua_getfield(L,Top+2,"servOnStart");
		int X=lua_type(L,-1);
		if (lua_type(L,-1)!=LUA_TFUNCTION)
		{
			sprintf(TextBuf,"Unimod: 'spellData[0x%x].servOnStart' not a function",SpellArr[0]);
			conPrintI(TextBuf);
			break;
		}
		lua_getfield(L,LUA_REGISTRYINDEX,"server");
		lua_setfenv(L,-2);
		if (lua_tointeger(L,-2) & 0x20) /// Наш спелл имеет цель - надо делать муху
		{
			if (Target!=NULL)
			{
				if ( 0x10000000  & (*(DWORD*)(Target+0x154)) ) /// цель защищена от магии - ничего не делаем
				{
					break;
				}
			}
		}
		else
		{
			lua_pushlightuserdata(L,MyPlayer);
			lua_pushnumber(L,*((int*)(PlayerInfo+0x8EC))); // x
			lua_pushnumber(L,*((int*)(PlayerInfo+0x8F0))); // y
			if (Target)	lua_pushlightuserdata(L,Target);
				else lua_pushnil(L);
			lua_pushvalue(L,-7); // data
			if (0!=lua_pcall(L,5,1,0))// player,targX,targY,targetUnit,data -> success
			{
				conPrintI(lua_tostring(L,-1));
			}
			if (0!=lua_toboolean(L,-1)) //спелл удался
			{
				/// надо сохранить данные, отнять ману и т.п.
				
				byte Buf[10],*P=Buf;
				netUniPacket(upSpellStart,P,sizeof(DWORD));
				*((DWORD*)P)=SpellArr[0];
				netSendNotOne(Buf,P-Buf,MyUc); /// посылает всем клиентам  кроме одного
			}
		}
	} while(0);
	lua_settop(L,Top);
}


void spellsInit()
{
	ASSIGN(spellAccept,0x004FD400);
	ASSIGN(spellGetPower,0x004FE7B0);
	ASSIGN(spellCancelDurSpell,0x004FEB10);
	ASSIGN(spellBuffOff,0x004FF5B0);
	ASSIGN(spellDefHasFlags,0x00424A50);
	ASSIGN(createSpellFly,0x004FDDA0);

	ASSIGN(applyToTarget,0x004FF380);

	ASSIGN(myCastSpellByUser,0x004FDD20);

	InjectOffs(0x00541337+1,&myCastSpellByUserMob);
	InjectOffs(0x004FB425+1,&myCastSpellByUserPlayer);

	registerserver("spellSync",&spellSync);
	registerserver("spellApply",&spellApplyL);
	registerserver("spellBuff",&spellBuff);

	lua_newtable(L);// в таблицу будем класть  интересных нам мобов
	registerServerVar("unitOnCastList");
}