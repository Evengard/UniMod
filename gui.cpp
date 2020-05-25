#include "stdafx.h"
#include "math.h"
#include "windowUniMod.h"

int (__cdecl *noxGuiFontHeightMB) (void* FontPtr);
int (__cdecl *noxSub_43F670) (int Flag); // не уверен
int (__cdecl *noxDrawGetStringSize) (void *FontPtr, const wchar_t*String,int *Width,int *H,int);
int (__cdecl *noxSetRectColorMB) (int);
int (__cdecl *noxDrawRect) (int xLeft,int yTop,int width,int height);
int (__cdecl *noxDrawRectAlpha) (int xLeft,int yTop,int width,int height);
void *(__cdecl *guiFontPtrByName)(const char *FontName);

int (__cdecl *noxDrawListBoxWithImage) (void *Window,void *Wdd);
int (__cdecl *noxDrawListBoxNoImage) (void *Window,void *Wdd);

void (__cdecl *screenGetSize)(int &X,int &Y);

void (__cdecl *noxRasterPoint)(int X,int Y);
void (__cdecl *noxRasterPointRel)(int X,int Y);
void (__cdecl *noxRasterDrawLines)();
void (__cdecl *noxGuiDrawCursor) ();

extern void (__cdecl *drawSetTextColor)(DWORD Color);
extern void (__cdecl *drawString)(void *FontPtr,const wchar_t*String,int X,int Y);
extern void *(__cdecl *gLoadImg)(const char *Name);
extern DWORD parseColor(const char *Color);
extern void (__cdecl *drawImage)(void *ImgH,int X,int Y);

const int *noxScreenX=		(int*)0x0069A5E0;
const int *noxScreenY=		(int*)0x0069A5E4;
const int *noxScreenWidth=	(int*)0x0069A5F0;
const int *noxScreenHieght=	(int*)0x0069A5F4;
const int *noxDrawXLeft=	(int*)0x0069A5D0;
const int *noxDrawYTop=		(int*)0x0069A5D4;

int *cursorScreenX=(int*)0x006990B0;
int *cursorScreenY=(int*)0x006990B4;

DWORD parseColor(const char *Color)
{
	DWORD Ret=0,R=0,G=0,B=0;
	const char *P=Color;
	if (Color==NULL || strcmpi(Color,"TRANSPARENT")==0)
		return 0x80000000;
	if (*P!='#')
		return Ret;
	P++;
	for (int i=1;*P!=0;P++,i++)
	{
		Ret<<=4;
		if (*P>='0' && *P<='9')
			Ret+=(*P)-'0';
		else if (*P>='A' && *P<='F')
			Ret+=(*P)-'A'+10;
		if (i==2)
		{
			R=Ret;Ret=0;
		}else if(i==4)
		{
			G=Ret;Ret=0;
		}else if(i==6)
		{
			B=Ret;Ret=0;
		}
	}
	Ret=(R>>3)<<11 | (G>>2)<<5 |(B>>3);///долбаный 16битный цвет
	Ret|=Ret<<16;
	return Ret;
}
void wstringFromLua(lua_State *L,wchar_t *Dst,int MaxLen)
{
	size_t Len=0;
	const char *Src=lua_tolstring(L,-1,&Len);
	if (Src==NULL)
	{
		wcscpy(Dst,L"(null)");
		return;
	}
	if (Len>MaxLen)
	{
		wcscpy(Dst,L"(long)");
		return;
	}
	mbstowcs(Dst,Src,MaxLen);
}
void *imageFromLua(lua_State *L)
{
	void *Ret=NULL;
	if (lua_type(L,-1)==LUA_TSTRING)
	{
		Ret=gLoadImg(lua_tostring(L,-1));
	}else if (lua_type(L,-1)==LUA_TLIGHTUSERDATA)
	{
		Ret=lua_touserdata(L,-1);
	}
	return Ret;
}
void parseAttr(lua_State *L,int KeyIdx,int ValIdx,void *Struct,const ParseAttrib *Attrs)
{
	byte *H=(byte *)Struct;
	const char *Key=lua_tostring(L,KeyIdx);
	size_t Len;
	const char *Val=lua_tolstring(L,ValIdx,&Len);
	for (ParseAttrib const *P=Attrs;P->name!=NULL;P++)
	{
		if (0!=strcmp(P->name,Key))
			continue;
		switch (P->type)
		{
		case 2: //string
			{
			char *NewString=(char*)noxAlloc(Len+1);
			strcpy(NewString,Val);
			*((char**)( H + P->ofs))=NewString;
			}
			break;
		case 3: //color
			*((DWORD*)( H + P->ofs))=parseColor(Val);
			break;
		case 4:// wstring
			{
				wchar_t *NewString=(wchar_t *)noxAlloc(2*(Len+1));
				mbstowcs(NewString,Val,Len+1);
				*((wchar_t**)( H + P->ofs))=NewString;
			}
			break;
		case 5: //bitfield
			break;//TODO
		}
	}
}

	int __cdecl uniWindowDrawFn(void *Window,void *WindowDD) // Специальная фн рисования, что бы была и прозрачность
	{
		BYTE *Wnd=(BYTE*) Window;
		wddControl *Wdd=(wddControl*) WindowDD;
		int x; int y;
		int *px=&x; int *py=&y;
		noxWndGetPostion(Window,px,py);
		if ((*(int*)(Wnd+4)) & 0x80) // если есть этот флаг => рисуем картинку
		{
			x+=Wdd->offsetX; y+=Wdd->offsetY;
			if (Wdd->flags0 & 2)
				drawImage(Wdd->imageHiliteH,x,y);
			else
				drawImage(Wdd->imageH,x,y);
			return 1;
		}
		if (Wdd->BgColor==0x80000000)
			return 1; //
		noxSetRectColorMB(Wdd->BgColor);
		int Top=lua_gettop(L);
		lua_pushlightuserdata(L,&noxWndLoad);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L,Window);
		lua_gettable(L,-2);
		if (lua_type(L,-1)!=LUA_TTABLE)
		{
			noxDrawRect(x,y,*((int*)(Wnd+8)),*((int*)(Wnd+0xC)));
			return 1;
		}
		lua_getfield(L,-1,"alpha");
		if (lua_toboolean(L,-1))
			noxDrawRectAlpha(x,y,*((int*)(Wnd+8)),*((int*)(Wnd+0xC)));
		else
			noxDrawRect(x,y,*((int*)(Wnd+8)),*((int*)(Wnd+0xC)));
		lua_settop(L,Top);
		return 1;
	}

	int __cdecl uniListBoxDrawFn(wndStruct *Window,void *WindowDD)
	{
		wndStruct *WndChild=0;
		listBoxDataStruct *ListboxData=(listBoxDataStruct*)Window->someData;
		int ret=0;
		if (ListboxData->slider!=0)
			WndChild=ListboxData->slider;
		else if (ListboxData->buttonUp!=0)
			WndChild=ListboxData->buttonUp;
		else 
			WndChild=ListboxData->buttonDown;		
		int width=Window->width;
		if (WndChild!=0)
		{
			if (!(WndChild->screenLeft>Window->screenRight) && !(WndChild->screenLeft<Window->screenLeft))
				Window->width=width-(Window->screenRight-WndChild->screenRight);
		}
		if ((Window->flags & wfImage)==0)
			ret=noxDrawListBoxNoImage(Window,WindowDD);
		else 
			ret=noxDrawListBoxWithImage(Window,WindowDD);
		Window->width=width;
		return ret;
	}

namespace 
{

	int stringGetSizeL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args");
			lua_error_(L);
		}
		const char *V=lua_tostring(L,1);
		wchar_t W[500];
		mbstowcs(W, V,strlen(V));
		W[strlen(V)]=0;
		wchar_t *pW=W;
		int Width=0;
		int Height=0;
		noxDrawGetStringSize(NULL,pW,&Width,&Height,0);
		lua_pushnumber(L,Width);
		lua_pushnumber(L,Height);
		return 2;
	}


	int stringDrawL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TSTRING || lua_type(L,2)!=LUA_TNUMBER || lua_type(L,3)!=LUA_TNUMBER)
		{
			lua_pushstring(L,"wrong args");
			lua_error_(L);
		}
		DWORD color=0xFFFFFF;
		if (lua_type(L,4)==LUA_TSTRING)
			color=(int)parseColor(lua_tostring(L,4));
		const char *V=lua_tostring(L,1);
		wchar_t W[500];
		mbstowcs(W, V,strlen(V));
		W[strlen(V)]=0;
		drawSetTextColor(color);
		drawString(NULL,W,lua_tonumber(L,2),lua_tonumber(L,3));
		return 0;
	}

	int screenGetPosL(lua_State*L)
	{
		lua_pushinteger(L,*noxScreenX);
		lua_pushinteger(L,*noxScreenY);
		return 2;
	}

	int screenGetSizeL(lua_State*L)
	{
		lua_pushinteger(L,*noxScreenWidth);
		lua_pushinteger(L,*noxScreenHieght);
		return 2;
	}
	int posWorldToScreenL(lua_State*L)
	{
		if (lua_type(L,1)!=LUA_TNUMBER || lua_type(L,2)!=LUA_TNUMBER)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		float x1=lua_tonumber(L,1)-*noxScreenX;
		x1+=*noxDrawXLeft;
		float y1=*noxDrawYTop-*noxScreenY;
		y1+=lua_tonumber(L,2);
		lua_pushinteger(L,x1);
		lua_pushinteger(L,y1);
		return 2;
	}

	int cliPlayerMouseL(lua_State *L)
	{
		if (lua_type(L,1)==LUA_TNUMBER && lua_type(L,2)==LUA_TNUMBER)
		{
			int x1=lua_tointeger(L,1);
			int y1=lua_tointeger(L,2);
			if ((x1>0 && x1<*noxScreenWidth) && (y1>0 && y1<*noxScreenHieght))
			{
				*cursorScreenX=x1;
				*cursorScreenY=y1;
			}
		}
		else
		{
			int x=*cursorScreenX+*noxScreenX;
			x-=*noxDrawXLeft;
			int y=*noxDrawYTop-*noxScreenY;
			y-=*cursorScreenY;
			lua_pushnumber(L,x);
			lua_pushnumber(L,y*-1);
			return 2;
		}
		return 0;
	}
	int drawLinesNow(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<3 || lua_type(L,1)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		noxSetRectColorMB(parseColor(lua_tostring(L,1)));
		for (int i=2;i<n;i+=2)
		{
			noxRasterPoint(lua_tointeger(L,i),lua_tointeger(L,i+1));
		}
		noxRasterDrawLines();
		return 0;
	}
	int drawFloorLines(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<3 || lua_type(L,1)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int Id=0;
		FxBuffer_t * B = FxBuffer_t::addBlock(n+6,&Id);
		B->addItem(11);
		B->addItem(8);
		B->addItemD(parseColor(lua_tostring(L,1)));
		B->addItem(9);
		B->addItem(n-1);
		for (int i=2;i<n;i+=2)
		{
			int X=lua_tointeger(L,i);
			int Y=lua_tointeger(L,i+1);
			B->addItem(X);
			B->addItem(Y);
		}
		B->addItem(10);
		lua_pushinteger(L,Id);
		return 1;
	}
	int drawFloorToCursor(lua_State *L)
	{
		int n=lua_gettop(L);
		if ((n!=3 && n!=4) || lua_type(L,1)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_settop(L,4);
		int Id=0;
		FxBuffer_t * B = FxBuffer_t::addBlock(8,&Id);
		B->addItem(11);
		B->addItem(8);
		B->addItemD(parseColor(lua_tostring(L,1)));
		B->addItem(14);
		B->addItem(lua_tointeger(L,2));
		B->addItem(lua_tointeger(L,3));
		B->addItem(lua_isnil(L,4)?31:lua_tointeger(L,4));
		B->addItem(11);
		lua_pushinteger(L,Id);
		return 1;
	}
	int drawLines(lua_State *L)
	{
		int n=lua_gettop(L);
		if (n<3 || lua_type(L,1)!=LUA_TSTRING)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int Id=0;
		FxBuffer_t * B = FxBuffer_t::addBlock(n+4,&Id);
		B->addItem(8);
		B->addItemD(parseColor(lua_tostring(L,1)));
		B->addItem(9);
		B->addItem(n-1);
		for (int i=2;i<n;i+=2)
		{
			B->addItem(lua_tointeger(L,i));
			B->addItem(lua_tointeger(L,i+1));
		}
		lua_pushinteger(L,Id);
		return 1;
	}
	int delLines(lua_State *L)
	{
		int Id=lua_tonumber(L,1);
		FxBuffer_t::delBlock(Id);
		return 0;
	}
	int __declspec(naked) myWndDrawAll()
	{
		__asm
		{
			call FxBuffer_t::drawBuffers
			push 0x0046C2E0
			ret
		}
	}
}

extern void InjectAddr(DWORD Addr,void *Fn);
extern void InjectOffs(DWORD Addr,void *Fn);
extern void InjectJumpTo(DWORD Addr,void *Fn);
extern void ImageUtilInit();
void guiInit()
{
	ImageUtilInit();
	ASSIGN(noxGuiDrawCursor,0x00477830);
	ASSIGN(screenGetSize,0x00430C50);
	ASSIGN(guiFontPtrByName,0x0043F360);
	ASSIGN(noxGuiFontHeightMB,0x0043F320);
	ASSIGN(noxSub_43F670,0x43F670);
	ASSIGN(noxDrawGetStringSize,0x0043F840);
	ASSIGN(noxSetRectColorMB,0x00434460);
	ASSIGN(noxDrawRect,0x0049CE30);
	ASSIGN(noxDrawRectAlpha,0x0049CF10);

	ASSIGN(noxDrawListBoxWithImage,0x004A3FC0);
	ASSIGN(noxDrawListBoxNoImage,0x004A3C50);

	ASSIGN(noxRasterDrawLines,0x0049E4B0);
	ASSIGN(noxRasterPointRel,0x0049F570);
	ASSIGN(noxRasterPoint,0x0049F500);
	
	InjectOffs(0x0043E71A+1,&myWndDrawAll);
	
	registerclient("cliDrawLines",&drawLines);
	registerclient("cliFloorLines",&drawFloorLines);
	registerclient("cliDelLines",&delLines);
	registerclient("cliFloorToCursor",&drawFloorToCursor);
	
	
	registerclient("cliPlayerMouse",&cliPlayerMouseL);
	registerclient("screenGetPos",screenGetPosL);
	registerclient("screenGetSize",screenGetSizeL);
	registerclient("posWorldToScreen",posWorldToScreenL);
	registerclient("stringDraw",stringDrawL);
	registerclient("stringGetSize",stringGetSizeL);

}
