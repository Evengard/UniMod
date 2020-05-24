#include "stdafx.h"

struct TileOne
{
	int image;
	int vari;
	int a,b;
	TileOne *subTilePtr;
};
struct TilePair
{
	DWORD flags;
	TileOne left,right;
};
tileDef_s *tileDefs;
TilePair ***tileRowData;
int *tileForceRedrawMB;

void (__cdecl *tileFreeSubtiles)(void *tileOne);// получает точно tileOne
TileOne* (__cdecl *tileListAddNew)(int A,int B,int C,int D);/// возможно Subtile

bool (__cdecl *tileCheckImageVari)(int Vari);

#define NOX_TILES_COUNT (0xB0)
namespace
{
	const char *tileNameByN(int N)
	{
		if (N<0 || N>=NOX_TILES_COUNT)
			return NULL;
		return tileDefs[N].Name;
	};
	int tileGetName(lua_State*L)
	{
		lua_settop(L,1);
		const char *R=tileNameByN(lua_tointeger(L,1));
		if (R==NULL)
			lua_pushnil(L);
		else
			lua_pushstring(L,R);
		return 1;
	}
	int tileMaxVari(lua_State*L)
	{
		int T=lua_tonumber(L,1);
		if (T<0 || T>=NOX_TILES_COUNT)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		// ПОКА НЕ ЗНАЮ КАК ВЫЯСНИТЬ,
		// надо ботать 0051D570 tileCheckImageVari
		lua_pushinteger(L,tileDefs[T].tileSizeX*tileDefs[T].tileSizeY);
		return 1;
	}
	int tileGet(lua_State*L)
	{
		lua_settop(L,2);
		int x=lua_tointeger(L,1);
		int y=lua_tointeger(L,2);
		x/=46;
		y/=23;
		int right=y&1;
		y>>=1;
		if (x<0 || x>0x7F || y<0 || y>0x7F)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		TilePair *tp=((*tileRowData)[x])+y;
		TileOne *One=0;
		if (right==1)
		{
			if(tp->flags&2)
			{
				One=&tp->right;
			}
		}
		else
		{
			if (tp->flags&1)
			{
				One=&tp->left;
			}
		}
		if (One==NULL)
		{
			lua_pushnil(L);
			return 1;
		}
		lua_newtable(L);
		int i=1;
		for (;One!=NULL;One=One->subTilePtr)
		{
			lua_pushinteger(L,One->image);
			lua_rawseti(L,-2,i++);			
			lua_pushinteger(L,One->vari);
			lua_rawseti(L,-2,i++);			
			lua_pushinteger(L,One->a);
			lua_rawseti(L,-2,i++);			
			lua_pushinteger(L,One->b);
			lua_rawseti(L,-2,i++);			
		}
		return 1;
	}
	int tileSet2(lua_State*L)
	{
		lua_settop(L,3);
		int x=lua_tointeger(L,1);
		int y=lua_tointeger(L,2);
		if (x<0 ||  y<0 || lua_type(L,3)!=LUA_TTABLE)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int N=luaL_getn(L,3);
		if (0!=(N&3))/// должно быть кратное четырем количество полей
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		if (N>4*6)
		{
			lua_pushstring(L,"wrong args - too many subtiles!");
			lua_error_(L);
		}
		x/=46;
		y/=23;
		int right=y&1;
		y>>=1;
		TilePair *tp=((*tileRowData)[x])+y;
		TileOne *One=NULL;
		if (right==1)
		{
			One=&tp->right;
			tp->flags|=2;
		}
		else
		{
			One=&tp->left;
			tp->flags|=1;
		}
		if (One==NULL) 
		{
			lua_pushstring(L,"Unknown error!");
			lua_error_(L);
		}
		TileOne *Prev=NULL;
		for (int i=1;i<N;)
		{
			if (One==NULL)
			{
				One=tileListAddNew(0,0,0,0);
				Prev->subTilePtr=One;
			}
			if (One==NULL) break; // вдруг при добавлении облом вышел
			lua_rawgeti(L,3,i++);
			One->image=lua_tointeger(L,-1);
			lua_rawgeti(L,3,i++);
			One->vari=lua_tointeger(L,-1);
			lua_rawgeti(L,3,i++);
			One->a=lua_tointeger(L,-1);
			lua_rawgeti(L,3,i++);
			One->b=lua_tointeger(L,-1);
			lua_pop(L,4);

			Prev=One;
			One=One->subTilePtr;
		}
		if (One!=0 && Prev!=NULL)
		{
			tileFreeSubtiles(Prev);
		}
		*tileForceRedrawMB=1;		/// Тут надо послать на перерисовку экрана
		return 0;
	}
	int tileSet(lua_State*L)
	{
		lua_settop(L,3);
		int x=lua_tointeger(L,1);
		int y=lua_tointeger(L,2);
		if (x<0 ||  y<0 || lua_type(L,3)!=LUA_TTABLE)
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		int N=luaL_getn(L,3);
		if (0!=(N&3))/// должно быть кратное четырем количество полей
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}

		BYTE B[80];
		BYTE *P=B;
		if (4+1+(N*2)>80)
		{
			lua_pushstring(L,"wrong args - too many subtiles!");
			lua_error_(L);
		}
		netUniPacket(upChangeTile,P,4+1+(N*2) );
		*((short*)P)=x;
		*((short*)P+1)=y;
		P+=4;
		*(P++)=0;
		*(P++)=0;/// туда мы еще чего-нить запишем
		*(P++)=N;
		short *S=(short*)P;
		for (int i=1;i<=N;i++,S++)
		{
			lua_rawgeti(L,3,i);
			*S=lua_tointeger(L,-1);
			lua_pop(L,1);
		}
		netSendAll(B,((BYTE*)S)-B);
		return 0;
	}
}
void netOnTileChanged(BYTE *Buf,BYTE *End)
{
	if (End-Buf<=5)
		return;
	int i=0;
	int x=*((short*)Buf+0),y=*((short*)Buf+1),N=Buf[4];
	x/=46;
	y/=23;
	int right=y&1;
	y>>=1;
	TilePair *tp=((*tileRowData)[x])+y;
	TileOne *One=NULL;
	if (right==1)
	{
		One=&tp->right;
		tp->flags|=2;
	}
	else
	{
		One=&tp->left;
		tp->flags|=1;
	}
	if (One==NULL) return;// какая-то аццкая ошибка
	BYTE *P=Buf;
	P+=6;
	N=*(P++);
	short *S=(short*)P;
	TileOne *Prev=NULL;
	for (int i=0;i<N;i+=4)
	{
		if (One==NULL)
		{
			One=tileListAddNew(0,0,0,0);
			Prev->subTilePtr=One;
		}
		if (One==NULL) break; // вдруг при добавлении облом вышел
		One->image=*(S++);
		One->vari=*(S++);
		One->a=*(S++);
		One->b=*(S++);
		Prev=One;
		One=One->subTilePtr;
	}
	if (One!=0 && Prev!=NULL)
	{
		tileFreeSubtiles(Prev);
	}
	*tileForceRedrawMB=1;		/// Тут надо послать на перерисовку экрана
}

void tilesInit()
{
	ASSIGN(tileFreeSubtiles,0x00422200)
	ASSIGN(tileListAddNew,0x00422160);
	ASSIGN(tileDefs,0x008632E0);
	ASSIGN(tileRowData,0x0085B7C0 );
	ASSIGN(tileForceRedrawMB,0x006F7A78);
	ASSIGN(tileCheckImageVari,0x0051D570);

	registerserver("tileGet",tileGet);
	registerserver("tileSet",tileSet);
	registerserver("tileSet2",tileSet2);
	registerclient("tileGetName",tileGetName);
	registerclient("tileMaxVari",tileMaxVari);
}