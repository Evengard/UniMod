#include "stdafx.h"
#include "unit.h"

extern void (__cdecl *netClientSend) (int PlrN,int Dir,//1 - клиенту
								void *Buf,int BufSize);

extern void (__cdecl *wndShowHide)(void *Wnd,int Hide);

extern DWORD *frameCounter;


void (__cdecl *sub_4C31D0) (int netCode);
void (__cdecl *sub_4C3147)();
void (__cdecl *sub_4C2BF0)();
void (__cdecl *cliSummondWndLoad) ();
void (__cdecl *wndSummonCreateList) (void*);

BYTE *wndSummonUsed;
int wndConjSummonMsg;
int *creatureSummonCommandAll;



struct creatureWhatDo 
{
	int commandAll;
	int mobNetCode[4];
	int command[4];
};

creatureWhatDo myCreatureList;


namespace
{
	void __declspec(naked) asmDeathBallBugGs() // дл€ GreatSword
	{
		__asm
		{
			call noxUnitSetOwner
			add esp,0Ch
			cmp [ebx+4],02C2h
			jnz exitL
			mov eax,[ebx+2DCh] // теперь это врем€ последнего коллайда
			mov ecx,frameCounter
			mov ecx,[ecx+0]
			test eax,eax // если у нас там 0, то пишем врем€ и выходим
			jnz l1
			obnul:
			mov [ebx+2DCh],ecx
			mov [ebx+2E0h],0
			jmp exitL
			l1: // если не ноль то смотрим что и действуем
			sub ecx,eax
			cmp ecx,4
			ja obnul // если прошло больше 4 кадров
			mov ecx,[ebx+2E0h]
			inc ecx
			mov [ebx+2E0h],ecx
			cmp ecx,3
			jna exitL
			push ebx
			call noxDeleteObject //удал€ем наконец
			add esp,4

			exitL:
			push 004E1C92h
			ret
		}
	}

	void __declspec(naked) asmDeathBallBugSh() // дл€ Shield
	{
		__asm
		{
			call noxUnitSetOwner
			add esp,0Ch
			cmp [ebx+4],02C2h
			jnz exitL
			mov eax,[ebx+2DCh] // теперь это врем€ последнего коллайда
			mov ecx,frameCounter
			mov ecx,[ecx+0]
			test eax,eax // если у нас там 0, то пишем врем€ и выходим
			jnz l1
			obnul:
			mov [ebx+2DCh],ecx
			mov [ebx+2E0h],0
			jmp exitL
			l1: // если не ноль то смотрим что и действуем
			sub ecx,eax
			cmp ecx,4
			ja obnul // если прошло больше 4 кадров
			mov ecx,[ebx+2E0h]
			inc ecx
			mov [ebx+2E0h],ecx
			cmp ecx,3
			jna exitL
			push ebx
			call noxDeleteObject //удал€ем наконец
			add esp,4

			exitL:
			push 004E1BDEh
			ret
		}
	}



	void __declspec(naked) asmConjSummonEnotherCmp() // добавл€ем ѕ ћ
	{
		__asm
		{
			cmp eax,9
			mov wndConjSummonMsg,eax
			jnz l1
			push 004C2B32h
			ret
			
			l1:
			xor eax,eax
			add esp,10h
			retn
		}
	}
	struct creatureCommand
	{
		BYTE msg;
		short netCodeMob;
		short whatDo;
	};

	void __cdecl conjSummonDo(int A,void *B)
	{
		BYTE *M=*((BYTE**)wndSummonUsed);
		int Top=lua_gettop(L);
		getServerVar("creatureSummonWhatDo");
		if (lua_type(L,-1)==LUA_TNIL)
		{
 			wndSummonCreateList(B);
			lua_settop(L,Top);
			return;
		}
		int ForHunt=0;
		if (lua_tointeger(L,-1)==1)
			ForHunt=1;
		creatureCommand P;
		P.msg=0x78;
		P.netCodeMob=(short)*((BYTE**)M);
		int command=0;
		if (A==1)
			P.whatDo=0; // испаритьс€
		else
		{
			for (int i=0;i<4;i++)
			{
				if (P.netCodeMob==myCreatureList.mobNetCode[i])
				{
					command=myCreatureList.command[i];
					if (command<3)
						P.whatDo=4;
					else if(command>=4+ForHunt)
						P.whatDo=3;
					else
						P.whatDo=myCreatureList.command[i]+1;
					myCreatureList.command[i]=P.whatDo;
				}
			}
		}
		netClientSend(0x1F,0,&P,4);
		lua_settop(L,Top);
	}
		
	void __declspec(naked) asmConjSummonDo() // когда нажали на моба
	{
		__asm
		{
			add esp,4
			push eax
			mov eax,wndConjSummonMsg
			cmp eax,9
			jz l1
			push 0
			call conjSummonDo
			jmp lex
			l1: // если ѕ ћ то убиваем моба
			push 1
			call conjSummonDo

			lex: // выходим из сабы
			add esp,8
			push 004C2BBEh
			ret
		}
	}

	void __cdecl conjSummonCreate(int netCode)
	{
		for (int i=0;i<4;i++)
		{
			if (myCreatureList.mobNetCode[i]==0)
			{
				myCreatureList.mobNetCode[i]=netCode;
				myCreatureList.command[i]=myCreatureList.commandAll;
				return;
			}
		}
	}
	void __declspec(naked) asmConjSummonCreate() // когда создали моба прогон€ем его
	{
		__asm
		{
			push esi 
			call conjSummonCreate
			add esp,4
			call cliSummondWndLoad
			push 0049179Fh
			ret
		}
	}

	void __cdecl ConjSummonDoAll()
	{
		int c=myCreatureList.commandAll;
		for (int i=0;i<4;i++)
			myCreatureList.command[i]=c;
		if (c==0) 
			myCreatureList.commandAll=4;
	}
	void __declspec(naked) asmConjSummonDoAll() // когда отпровл€ем команду всем
	{
		__asm
		{
			xor eax,eax
			mov al,cl
			mov [myCreatureList+0],eax
			call ConjSummonDoAll
			call netClientSend
			push 004C2AD1h
			ret
		}
	}

	void __cdecl ConjSummonLoadWnd()
	{
		myCreatureList.commandAll=4;
		for (int i=0;i<4;i++)
		{
			myCreatureList.command[i]=0;
			myCreatureList.mobNetCode[i]=0;
		}
	}
	void __declspec(naked) asmConjSummonLoadWnd() // когда заргружаетс€ окна все к черту обнул€ем
	{
		__asm
		{
			call wndShowHide
			call ConjSummonLoadWnd
			push 004C1FA6h
			ret
		}
	}

	void __cdecl ConjSummonDieOrBanish(int netCode)
	{
		for  (int i=0;i<4;i++)
		{
			if (myCreatureList.mobNetCode[i]==netCode)
				myCreatureList.mobNetCode[i]=0;
		}
	}
	void __declspec(naked) asmConjSummonDieOrBanish() // когда моб сдыхает
	{
		__asm
		{
			push edi
			call ConjSummonDieOrBanish
			add esp,4
			call sub_4C31D0
			push 004C314Ch
			ret
		}
	}
	void __cdecl fixCastFieball(float *A,float *B)
	{
		float *X=*((float**)A);
		int *XMouse=*((int**)B);
		float *Y=X+1;
		int *YMouse=XMouse+1;
		int cosA=*XMouse-*X;
		int sinA=*YMouse-*Y;
		float dist=sqrt((float)(cosA^2+sinA^2));
		*A=dist/cosA;
		*B=dist/sinA;
	}

	void __declspec(naked) asmFixCastFireball() // 0052C7CD
	{
		__asm
		{
			// edi - unit
			test byte ptr [edi+8],4 // игрок ли? ћало-ли что, мб у мен€ парано€
			jz l1
		/*	mov edx,edi
			add edx,0x38
			push edx
			mov edx,esp
			mov ecx,[edi+0x2ec]
			mov ecx,[ecx+0x114]
			add ecx,0x8ec
			push ecx
			push esp
			push edx
			call fixCastFieball
			add esp,8
			mov edx,[esp]
			mov ecx,[esp+4]
			add esp,8 */

			mov ecx,[edi+0x2ec]
			mov ecx,[ecx+0x114]
			sub esp,4
			fild [ecx+0x8ec]
			fsub [edi+0x38]
			fld st //  x-x1
			fmul st,st
			fstp [esp]
			fild [ecx+0x8f0]
			fsub [edi+0x3c]
			fld st //  y-y1
			fmul st,st
			fadd [esp]
			fsqrt 
			fld st
			fdivp st(2),st
			fdivp st(2),st
			fstp [esp] //cos
			mov edx,[esp]
			fstp [esp] //sin 
			mov ecx,[esp]
			add esp,4 
			
l1: // exit, как то блин так и назвал, удивл€лс€ почему нокс закрываетс€
			push 0052C7D9h
			ret
		}
	}
}
extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);

void topicOverrideInit()
{
	byte nop[2] = { 0x90, 0x90 };
	//InjectData(0x0040C29B, nop, 2);
}

int (__cdecl *netOnPacketRecvServ)(int playerId, char *packet, int length);
// Validates incoming player data in order to prevent rogue players crashing the server
int __cdecl netOnPacketRecvServ_Hook(int playerId, char *packet, int length)
{
	if (*packet == 0x20 && length < 0x9A)
	{
		char test[60];
		sprintf(test,"[UniMod] Wrong player data len detected: 0x%X", length);
		conPrintI(test);
		// Go away, little bugger
		return 0;
	}

	packet++;
	// Check if nickname starts with invalid character
	if (packet[0] <= 0x1F)
	{
		wcscpy((wchar_t*)packet, L"Jack\0");
		conPrintI("[UniMod] Bugged player name was detected!");
	}

	// Check if player class is invalid
	if (packet[0x42] >= 3)
	{
		packet[0x42] = 0;
		conPrintI("[UniMod] Bugged player class was detected!");
	}

	// Check if player object requested by client is invalid
	if (packet[0x43] > 0)
	{
		packet[0x43] = 0;
		conPrintI("[UniMod] Bugged player object was detected!");
	}
	packet--;

	// Carry on
	return netOnPacketRecvServ(playerId, packet, length);
}

void bugsInit()
{
	ASSIGN(wndSummonUsed,0x00716E88);

	ASSIGN(creatureSummonCommandAll,0x005B4080);

	ASSIGN(sub_4C2BF0,0x004C2BF0);
	ASSIGN(sub_4C3147,0x004C3147);
	ASSIGN(sub_4C31D0,0x004C31D0);

	ASSIGN(wndSummonCreateList,0x004C2560);
	ASSIGN(cliSummondWndLoad,0x004C2E50);

	InjectJumpTo(0x004E1C8A,&asmDeathBallBugGs);
	InjectJumpTo(0x004E1BD6,&asmDeathBallBugSh);

	bool conjurerSummonCmdImprovements=false;
	if (conjurerSummonCmdImprovements)
	{
		InjectJumpTo(0x004C2BC7,&asmConjSummonEnotherCmp); // делаем возможность ѕ ћ
		InjectJumpTo(0x004C2BB6,&asmConjSummonDo); // ставим вместо списка наши 2 прикольные штуки
		InjectJumpTo(0x0049179A,&asmConjSummonCreate);
		InjectJumpTo(0x004C2ACC,&asmConjSummonDoAll);
		InjectJumpTo(0x004C3147,&asmConjSummonDieOrBanish);
		InjectJumpTo(0x004C1FA1,&asmConjSummonLoadWnd);
	}

	//InjectJumpTo(0x0052C7CD,&asmFixCastFireball);

	bool filterPlayerJoinData=true; // TODO: convert all similar hardcoded-switches into #defines in a separate file
	if (filterPlayerJoinData)
	{
		InjectOffs(0x4DEC40 + 1, &netOnPacketRecvServ_Hook);
	}
	ASSIGN(netOnPacketRecvServ, 0x51BAD0);
}