#include "stdafx.h"

extern void (__cdecl *netClientSend) (int PlrN,int Dir,//1 - �������
								void *Buf,int BufSize);

extern void (__cdecl *noxUnitDelete) (void *Unit);
extern void (__cdecl *noxUnitSetOwner) (void *NewOwner,void *Owner);
extern void (__cdecl *noxDeleteObject)(void *Unit);
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
	void __declspec(naked) asmDeathBallBugGs() // ��� GreatSword
	{
		__asm
		{
			call noxUnitSetOwner
			add esp,0Ch
			cmp [ebx+4],02C2h
			jnz exitL
			mov eax,[ebx+2DCh] // ������ ��� ����� ���������� ��������
			mov ecx,frameCounter
			mov ecx,[ecx+0]
			test eax,eax // ���� � ��� ��� 0, �� ����� ����� � �������
			jnz l1
			obnul:
			mov [ebx+2DCh],ecx
			mov [ebx+2E0h],0
			jmp exitL
			l1: // ���� �� ���� �� ������� ��� � ���������
			sub ecx,eax
			cmp ecx,4
			ja obnul // ���� ������ ������ 4 ������
			mov ecx,[ebx+2E0h]
			inc ecx
			mov [ebx+2E0h],ecx
			cmp ecx,3
			jna exitL
			push ebx
			call noxDeleteObject //������� �������
			add esp,4

			exitL:
			push 004E1C92h
			ret
		}
	}

	void __declspec(naked) asmDeathBallBugSh() // ��� Shield
	{
		__asm
		{
			call noxUnitSetOwner
			add esp,0Ch
			cmp [ebx+4],02C2h
			jnz exitL
			mov eax,[ebx+2DCh] // ������ ��� ����� ���������� ��������
			mov ecx,frameCounter
			mov ecx,[ecx+0]
			test eax,eax // ���� � ��� ��� 0, �� ����� ����� � �������
			jnz l1
			obnul:
			mov [ebx+2DCh],ecx
			mov [ebx+2E0h],0
			jmp exitL
			l1: // ���� �� ���� �� ������� ��� � ���������
			sub ecx,eax
			cmp ecx,4
			ja obnul // ���� ������ ������ 4 ������
			mov ecx,[ebx+2E0h]
			inc ecx
			mov [ebx+2E0h],ecx
			cmp ecx,3
			jna exitL
			push ebx
			call noxDeleteObject //������� �������
			add esp,4

			exitL:
			push 004E1BDEh
			ret
		}
	}



	void __declspec(naked) asmConjSummonEnotherCmp() // ��������� ���
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
		creatureCommand P;
		P.msg=0x78;
		P.netCodeMob=(short)*((BYTE**)M);
		int command=0;
		if (A==1)
			P.whatDo=0; // ����������
		else
		{
			for (int i=0;i<4;i++)
			{
				if (P.netCodeMob==myCreatureList.mobNetCode[i])
				{
					command=myCreatureList.command[i];
					if (command<3)
						P.whatDo=4;
					else if(command>=5)
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
		
	void __declspec(naked) asmConjSummonDo() // ����� ������ �� ����
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
			l1: // ���� ��� �� ������� ����
			push 1
			call conjSummonDo

			lex: // ������� �� ����
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
	void __declspec(naked) asmConjSummonCreate() // ����� ������� ���� ��������� ���
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
	void __declspec(naked) asmConjSummonDoAll() // ����� ���������� ������� ����
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
	void __declspec(naked) asmConjSummonLoadWnd() // ����� ������������ ���� ��� � ����� ��������
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
	void __declspec(naked) asmConjSummonDieOrBanish() // ����� ��� �������
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



}
extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);
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

	InjectJumpTo(0x004C2BC7,&asmConjSummonEnotherCmp); // ������ ����������� ���
	InjectJumpTo(0x004C2BB6,&asmConjSummonDo); // ������ ������ ������ ���� 2 ���������� �����
	InjectJumpTo(0x0049179A,&asmConjSummonCreate);
	InjectJumpTo(0x004C2ACC,&asmConjSummonDoAll);
	InjectJumpTo(0x004C3147,&asmConjSummonDieOrBanish);
	InjectJumpTo(0x004C1FA1,&asmConjSummonLoadWnd);


}