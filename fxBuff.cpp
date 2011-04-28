#include "stdafx.h"

extern void (__cdecl *noxRasterPoint)(int X,int Y);
extern void (__cdecl *noxRasterPointRel)(int X,int Y);
extern void (__cdecl *noxRasterDrawLines)();

extern int (__cdecl *noxSetRectColorMB) (int);

FxBuffer_t *FxFirstBuffer=0;
void FxBuffer_t::getValues(int First,int Len)
{
	//еще не готово
}
bool FxBuffer_t::delBlock(int Id) //удаляем блок
{
	int State=0;
	int Cnt=0;
	int Free;
	DWORD *CntPtr=0;
	for (FxBuffer_t *Buf=FxFirstBuffer;Buf!=NULL;Buf=Buf->Next)
	{
		Free=0;
		for (DWORD *P=Buf->Data,*E=P + (Buf->Size - Buf->FreeSize) ;P<E;)
		{
			switch (State)
			{
			case 0:
				if (0==*P)
				{
					*P=1;// пропуск
					State=1;
				}
				break;
			case 1:
				CntPtr=P;
				Cnt=*P;
				Free+=Cnt+2;
				State=2;
				break;
			case 2:
				if (Cnt>0)
					Cnt--;
				else 
				{
					if (*P==1)
						State=1;
					else 
						return true;
				}
				
				break;
			}//switch
			P++;
		}
		if (State==2)
		{
			if (Cnt==0) // съели последний кусок
			{
				Buf->FreeSize+=Free;
			}
			else if (Cnt>0 && Buf->Next!=0)
			{
				Buf->FreeSize+=Free;
				Buf=Buf->Next;
				DWORD *P=Buf->Data;
				*P=1;
				P++;
				*P=Cnt-2;
			}
		}
	}
	return false;
}
FxBuffer_t *FxBuffer_t::addBlock(int Size,int *Id)
{
	if (Size<1)
		return NULL;
	FxBuffer_t *Ret=0;
	int FreeSize=0;
	int Idx = 0;
	Size+=2;
	if (FxFirstBuffer==0)
	{
		FreeSize = (Size + 0xF)&~0xF;
		Ret = (FxBuffer_t *)new byte[ 4*FreeSize + sizeof(FxBuffer_t)];
		Ret->Size = FreeSize;
		Ret->SelIdx = 0;
		Ret->Next = 0;
		Ret->FreeSize = FreeSize;
		FxFirstBuffer = Ret;
	}
	else
	{
		FxBuffer_t *Buf2=0;
		for (Ret = FxFirstBuffer;Ret!=NULL && Ret->FreeSize < Size;Ret=Ret->Next)
		{
			Idx += Ret->Size;
			Buf2=Ret;
		}
		if (Ret==NULL)
		{
			FreeSize = (Size + 0xF)&~0xF;
			Ret = (FxBuffer_t *)new byte[ 4*FreeSize + sizeof(FxBuffer_t)];
			Ret->Size = FreeSize;
			Ret->SelIdx = 0;
			Ret->Next = 0;
			Ret->FreeSize = FreeSize;
			Buf2->Next = Ret;
		}
	}
	Idx += Ret->Size - Ret->FreeSize;
	Ret->FreeSize -= Size;
	Ret->addItem(0);
	Ret->addItem(Size-2);
	return Ret;
}

// еще надо относительно курсора сделать
void FxBuffer_t::drawBuffers()
{
	int State=0;
	int Cnt=0;
	int OfsX=0,OfsY=0;
	int X1,Y1,X2,Y2;
	int Idx=0;
	char Stop[sizeof(FxBuffer_t)]={0};
	FxBuffer_t *StopBuf=(FxBuffer_t *)&Stop;

	for (FxBuffer_t *Buf=FxFirstBuffer;Buf!=NULL;Buf=Buf->Next)
	{
		for (DWORD *P=Buf->Data,*E=P + (Buf->Size - Buf->FreeSize) ;P<E; Idx++)
		{
			switch(State)///!!! не менее 2х
			{
			case 0: // читаем данные 
				State=1 + *(P++);
				break;
			case 1:
				State = 0;
				P++;
				break;
			case 2:
				Cnt=*(P++);
				State = 103;
				break;
			case 103:
				if ((--Cnt)==0)
					State = 0;
				P++;
				break;
			case 9:
				noxSetRectColorMB(*(P++));
				State = 0;
				break;
			case 10: // начинаем рисовать линию
				State=105;
				Cnt=*(P++);
				break;
			case 105: // совсем первая точка X
				if (--Cnt==0)
					State = 0;
				else
					State = 106;
				X1 = *((int*)(P++)) - OfsX;
				break;
			case 106: // совсем первая точка Y
				if (--Cnt==0)
					State = 0;
				else
					State = 107;
				Y1 = *((int*)(P++)) - OfsY;
				break;
			case 107: // совсем первая точка X
				if (--Cnt==0)
					State = 0;
				else
					State = 108;
				X2 = *((int*)(P++)) - OfsX;
				break;
			case 108: // совсем первая точка Y
				if (--Cnt==0)
					State = 0;
				else
					State = 107;
				Y2 = *((int*)(P++)) - OfsY;
				noxRasterPoint(X1,Y1);
				noxRasterPoint(X2,Y2);
				noxRasterDrawLines();
				X1=X2;Y1=Y2;
				break;
			case 11:// нулевое смещение
				OfsX=0;
				OfsY=0;
				State = 0;
				break;
			case 12:// смещение от вьюпорта
				OfsX=*noxScreenX;
				OfsY=*noxScreenY;
				State = 0;
				break;
			case 13:// остановка
				Buf=StopBuf;
				P=E;
				State = 0;
				break;
			case 14: // сдвиг [сколько, dX, dY , откуда, куда], проверить
				State=110;
				Cnt=*(P++);
				break;
			case 110:
				State=111;
				Y1=*(P++);
				break;
			case 111:
				State=112;
				Y2=*(P++);
				break;
			case 112:
				State=113;
				X1=*(P++);
				break;
			case 113:
				State = 0;
				X2=*(P++);
				{
					FxBuffer_t *db=FxFirstBuffer;
					DWORD *sp=0,*dp=0;
					for( int Base=0 ; db!=NULL ; db= db->Next,Base+=db->Size)
					{
						if ( X1 > Base && X1 < Base + db->Size ) // в этом буфере лежит источник
						{
							X1 -=Base;
							sp = db->Data;
						}
						if ( X2 > Base && X2 < Base + db->Size ) // в этом буфере лежит назначение
						{
							X2 -=Base;
							dp = db->Data;
						}
					}
					while ( Cnt-- )
					{
						*dp = *sp + Y1;
						dp++;
						sp++;
						if (!Cnt--)
							break;
						*dp = *sp + Y2;
						dp++;
						sp++;

					}
				}
				break;
			default:
				P++;
				break;
			}
		}
	}
}