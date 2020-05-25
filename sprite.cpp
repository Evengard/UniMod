#include "stdafx.h"

sprite_s *(__cdecl *spriteLoadAdd) (int thingType,int coordX,int coordY);
void (__cdecl *spriteDeleteStatic)(sprite_s *Sprite);

void (__cdecl *drawImage)(void *ImgH,int X,int Y);

extern int (__cdecl *noxDrawGetStringSize) (int FontPtr, wchar_t *String,int *Width,int,int);
extern void (__cdecl *drawString)(void *FontPtr,const wchar_t*String,int X,int Y);
extern tileDef_s *tileDefs;

namespace
{
/*
надо продумать способ отрисовки спрайтов с кастомными переменными
*/
	void customSpriteDraw(void *DrawData,void *Sprite)
	{

	}

	int drawTileImage(lua_State *L)
	{
		lua_settop(L,4);
		int i=1;
		if (
			(lua_type(L,i++)!=LUA_TNUMBER)||
			(lua_type(L,i++)!=LUA_TNUMBER)||
			(lua_type(L,i++)!=LUA_TNUMBER)||
			(lua_type(L,i++)!=LUA_TNUMBER)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void **imageH=0;/// тут бы проверок
		imageH=(void **)tileDefs[lua_tointeger(L,1)].ImgPtr;
		if (imageH==NULL)
			return 0;
		if ((imageH[lua_tointeger(L,2)])==NULL)
			return 0;
		drawImage(imageH,lua_tointeger(L,3),lua_tointeger(L,4));
		return 0;
	}
	int tileGetImage(lua_State *L)
	{
		lua_settop(L,2);
		int i=1;
		if (
			(lua_type(L,i++)!=LUA_TNUMBER)||
			(lua_type(L,i++)!=LUA_TNUMBER)
			)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		void **imageH=0;/// тут бы проверок
		imageH=(void **)tileDefs[lua_tointeger(L,1)].ImgPtr;
		if (imageH==NULL)
			return 0;
		void *Ret=imageH[lua_tointeger(L,2)];
		if (Ret==NULL)
			return 0;
		lua_pushlightuserdata(L,Ret);
		return 1;
	}
}
void spriteInit()
{
	ASSIGN(spriteLoadAdd,0x0045A360);
	ASSIGN(spriteDeleteStatic,0x0045A4E0);
	ASSIGN(drawImage,0x0047D2C0);
	registerclient("tileDraw",&drawTileImage);
	registerclient("tileImage",&tileGetImage);
	
}