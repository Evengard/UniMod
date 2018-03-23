//
// webserver.c
//
// Simple HTTP server sample for sanos
//
#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <winsock.h>
#include <winsock2.h>
#pragma comment(lib,"wsock32.lib")
#pragma comment (lib, "Ws2_32.lib")
#define S_ISREG(x) (0!=((x) & S_IFREG ))
#define S_ISDIR(x) (0!=((x) & S_IFDIR ))

#else
#include <os.h>
#include <dirent.h>
#define closesocket close
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <varargs.h>
#include <sys/stat.h>
#include <process.h>
#include <queue>
//#include "stdafx.h"
using namespace std ;
#define SERVER "Nox UniMod/0.4.1"
#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"
#define PORT 80
//extern lua_State *L;
extern void getServerVar(const char *VarName);
void httpGetCallback();
unsigned __stdcall httpGetThread(void *Src);
HANDLE httpGetThreadH;
char httpGetResult[0x400]={0};
char httpGetSrc[0x100]={0};
int currentHttpGetReference;
bool alreadyActive=false;
queue<int> httpGetQueue;



extern byte authorisedState[0x20];
extern char* authorisedLogins[0x20];
extern DWORD* currentIP;
extern unsigned __int16 *currentPort;
std::pair<int, char*> httpGetInternal(char* uri);

bool tgets(char *Buf, int size,SOCKET s,char *&Remain)
{
	int R;
	R=recv(s,Buf,size,0);
	if (R==SOCKET_ERROR)
		return false;
	if (R<size)
		Buf[R]=0;
	char *P=strchr(Buf,'\r');
	if (P)
	{
		P+=1;
		*P=0;
	}
	Remain=P+1;
	return true;
}
void tputs(int s,const char *S)
{
	send(s,S,strlen(S),0);
}
int tprintf(SOCKET s,const char *Format,...)
{
	va_list l;
	va_start(l,Format);
	char Buf[2048];
	int Len=vsnprintf(Buf,2047,Format,l);
	if (Len<=0)
		return 0;
	send(s,Buf,Len,0);
	return Len;
}
char *get_mime_type(char *name)
{
  char *ext = strrchr(name, '.');
  if (!ext) return NULL;
  if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";
  if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
  if (strcmp(ext, ".gif") == 0) return "image/gif";
  if (strcmp(ext, ".png") == 0) return "image/png";
  if (strcmp(ext, ".css") == 0) return "text/css";
  if (strcmp(ext, ".au") == 0) return "audio/basic";
  if (strcmp(ext, ".wav") == 0) return "audio/wav";
  if (strcmp(ext, ".avi") == 0) return "video/x-msvideo";
  if (strcmp(ext, ".mpeg") == 0 || strcmp(ext, ".mpg") == 0) return "video/mpeg";
  if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
  return NULL;
}

void send_headers(int f, int status, char *title, char *extra, char *mime, 
                  int length, time_t date)
{
  time_t now;
  char timebuf[128];

  tprintf(f, "%s %d %s\r\n", PROTOCOL, status, title);
  tprintf(f, "Server: %s\r\n", SERVER);
  now = time(NULL);
  strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));
  tprintf(f, "Date: %s\r\n", timebuf);
  if (extra) tprintf(f, "%s\r\n", extra);
  if (mime) tprintf(f, "Content-Type: %s\r\n", mime);
  if (length >= 0) tprintf(f, "Content-Length: %d\r\n", length);
  if (date != -1)
  {
    strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&date));
    tprintf(f, "Last-Modified: %s\r\n", timebuf);
  }
	tprintf(f, "Cache-Control: no-cache\r\n");
  //tprintf(f, "Connection: close\r\n");
  tprintf(f, "\r\n");
}

void send_error(SOCKET f, int status, char *title, char *extra, char *text)
{
  send_headers(f, status, title, extra, "text/html", -1, -1);
  tprintf(f, "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\r\n", status, title);
  tprintf(f, "<BODY><H4>%d %s</H4>\r\n", status, title);
  tprintf(f, "%s\r\n", text);
  tprintf(f, "</BODY></HTML>\r\n");
}

void send_file(SOCKET f, char *path, struct stat *statbuf)
{
  char data[4096];
  int n;

  FILE *file = fopen(path, "r");
  if (!file)
    send_error(f, 403, "Forbidden", NULL, "Access denied.");
  else
  {
    int length = S_ISREG(statbuf->st_mode) ? statbuf->st_size : -1;
    send_headers(f, 200, "OK", NULL, get_mime_type(path), length, statbuf->st_mtime);

    while ((n = fread(data, 1, sizeof(data), file)) > 0) 
		send(f,data,n,0);
		//fwrite(data, 1, n, f);
    fclose(file);
  }
}
extern bool serverRequest(int f,char *path);	

int process(SOCKET f)
{
  char buf[4096];
  char *method;
  char *path;
  char *protocol;
  char *Headers;
  struct stat statbuf;
  char pathbuf[4096];
  int len;
  if (!tgets(buf, sizeof(buf), f,Headers)) return -1;

  method = strtok(buf, " ");
  path = strtok(NULL, " ");
  protocol = strtok(NULL, "\r");
  if (!method || !path || !protocol) return -1;

	if (stricmp(method, "GET") != 0)
		send_error(f, 501, "Not supported", NULL, "Method is not supported.");
	if (!serverRequest(f,path))
		send_error(f, 404, "Not Found", NULL, "File not found.");
#if 0
	else if (stat(path, &statbuf) < 0)
		send_error(f, 404, "Not Found", NULL, "File not found.");
  else if (S_ISDIR(statbuf.st_mode))
  {
    len = strlen(path);
    if (len == 0 || path[len - 1] != '/')
    {
		_snprintf_s(pathbuf, sizeof(pathbuf), "Location: %s/", path);
      send_error(f, 302, "Found", pathbuf, "Directories must end with a slash.");
    }
    else
    {
		_snprintf_s(pathbuf, sizeof(pathbuf), "%sindex.html", path);
      if (stat(pathbuf, &statbuf) >= 0)
        send_file(f, pathbuf, &statbuf);
      else
      {
/*        DIR *dir;
        struct dirent *de;

        send_headers(f, 200, "OK", NULL, "text/html", -1, statbuf.st_mtime);
        fprintf(f, "<HTML><HEAD><TITLE>Index of %s</TITLE></HEAD>\r\n<BODY>", path);
        fprintf(f, "<H4>Index of %s</H4>\r\n<PRE>\n", path);
        fprintf(f, "Name Last Modified Size\r\n");
        fprintf(f, "<HR>\r\n");
        if (len > 1) fprintf(f, "<A HREF=\"..\">..</A>\r\n");

        dir = opendir(path);
        while ((de = readdir(dir)) != NULL)
        {
          char timebuf[32];
          struct tm *tm;

          strcpy(pathbuf, path);
          strcat(pathbuf, de->d_name);

          stat(pathbuf, &statbuf);
          tm = gmtime(&statbuf.st_mtime);
          strftime(timebuf, sizeof(timebuf), "%d-%b-%Y %H:%M:%S", tm);

          fprintf(f, "<A HREF=\"%s%s\">", de->d_name, S_ISDIR(statbuf.st_mode) ? "/" : "");
          fprintf(f, "%s%s", de->d_name, S_ISDIR(statbuf.st_mode) ? "/</A>" : "</A> ");
          if (de->d_namlen < 32) fprintf(f, "%*s", 32 - de->d_namlen, "");

          if (S_ISDIR(statbuf.st_mode))
            fprintf(f, "%s\r\n", timebuf);
          else
            fprintf(f, "%s %10d\r\n", timebuf, statbuf.st_size);
        }
        closedir(dir);

        fprintf(f, "</PRE>\r\n<HR>\r\n<ADDRESS>%s</ADDRESS>\r\n</BODY></HTML>\r\n", SERVER);
*/
      }
    }
  }
  else
    send_file(f, path, &statbuf);
#endif
  return 0;
}
namespace
{
	int sock=0;
}
bool serverStart(int port)
{
  sockaddr_in sin;

	WSADATA WsaData;
	WSAStartup(0x101,&WsaData);
	bool Ret=true;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	if (0!=bind(sock, (struct sockaddr *) &sin, sizeof(sin)) )
	{
		closesocket(sock);
		sock=0;
		return false;
	}

	if (0!=listen(sock, 5))
	{
		closesocket(sock);
		sock=0;
		return false;
	}
	return true;
}
bool serverUpdate()
{
	if (sock==0)
		return true;
	int s;
	timeval wait={0,0};
	fd_set R={0};
	R.fd_count=1;
	R.fd_array[0]=sock;
	s = select(R.fd_count+1,&R,NULL,NULL,&wait);
	if (s==0) return true;
	if (s<0) return false;

	s = accept(sock, NULL, NULL);
	if (s < 0) {return false;}
	process(s);
	closesocket(s);
	return true;
}
void serverClose()
{
  closesocket(sock);
}
extern "C"
{
#include "lauxlib.h"
#include "lualib.h"
}

int httpGet(lua_State *L)
{
	lua_settop(L,1);
	//lua_pushstring(L,"(http://)?([^%/%:]+)(%:[^%/]+)?%/?(.*)");
	const char* Src[2]={0};
	//Src=lua_tostring(L,1);
	if (lua_type(L,1)==LUA_TSTRING)
	{
		lua_newtable(L);
		lua_pushvalue(L,1);
		lua_setfield(L,-2,"uri");
		lua_remove(L,1);
	}
	if (lua_type(L,1)!=LUA_TTABLE)
	{
		return 1;
	}
	lua_pushvalue(L,1);
	int reference = luaL_ref(L, LUA_REGISTRYINDEX);
	httpGetQueue.push(reference);
		//httpGetQueue.push(Src[1]);
		//lua_pushstring(L,"already in use");
		//lua_error_(L);
	return 1;
}

unsigned __stdcall httpGetThread(void *Src1)
{
	std::pair<int, char*> res=httpGetInternal((char*)Src1);
	strcpy(httpGetResult, res.second);
	delete[] res.second;
	return 0;
}

void httpGetCallback(lua_State *L)
{
	if(WAIT_OBJECT_0==WaitForSingleObject(httpGetThreadH, 0) && alreadyActive==true)
	{
		int Top=lua_gettop(L);
		lua_rawgeti(L, LUA_REGISTRYINDEX, currentHttpGetReference);
		do
		{
			lua_getfield(L,Top+1,"callback");
			if(!lua_isfunction(L,-1))
			{
				getServerVar("onHttpGet");
				if(!lua_isfunction(L,-1))
				{
					alreadyActive=false;
					break;
				}
			}
			lua_pushstring(L,httpGetResult);
			lua_pushstring(L,httpGetSrc);
			alreadyActive=false;
			lua_pcall(L,2,0,0);
			break;
		}
		while(0);
		luaL_unref(L, LUA_REGISTRYINDEX, currentHttpGetReference);
		lua_settop(L,Top);
	}
	if(httpGetQueue.empty()==false && alreadyActive==false)
	{
		int Top=lua_gettop(L);
		int reference = httpGetQueue.front();
		httpGetQueue.pop();
		lua_rawgeti(L, LUA_REGISTRYINDEX, reference);
		currentHttpGetReference=reference;
		if(lua_type(L,Top+1)!=LUA_TTABLE)
			return;
		lua_getfield(L,Top+1,"uri");
		//int type=lua_type(L,-1);
		if(lua_type(L,-1)!=LUA_TSTRING)
			return;
		const char* uri = lua_tostring(L, -1);
		httpGetThreadH = (HANDLE)_beginthreadex(NULL, 0, &httpGetThread, (void*)uri, 0, NULL);
		alreadyActive=true;
		lua_settop(L,Top);
	}
	return;
}

std::pair<int, char*> httpGetInternal(char* uri)
{
	char* returnData = new char[0x400];
	const char *Src=0,*Host=0,*Port=0,*Href=0,*P;
	Src=(char*)uri;
	if (0==strncmp(Src,"http://",7))
		Src+=7;
	Host=Src;
	Port=strchr(Src,':');
	P=strchr(Src,'/');
	if (Port!=NULL)
	{
		if (P!=NULL)
		{
			if (Port>P)
			{
				Port=NULL;
			}
		}
		else
			P=Port;
	}
	if (P==NULL)
		P=Host+strlen(Host);
	char Buf[0x400]={0};// нефиг юзать более длинные страницы
	int PortV=80;
	sockaddr_in so;
	memset(&so,0,sizeof(so));
	if (Port!=NULL)
	{
		strncpy(Buf,&Port[1],P-Port);
		Buf[P-Port-1]=NULL;
		PortV=atoi(Buf);
		strncpy(Buf,Host,Port-Host);
	}
	else
		strncpy(Buf,Host,P-Host);

	struct addrinfo *result;
	char HostAddr[0x100] = {0};
	strcpy(HostAddr, Buf);
	if (0!=getaddrinfo(Buf,NULL,NULL,&result))
	{
		//lua_pushstring(L,"httpGet - unable to resolve address");
		//lua_error(L);
		return std::make_pair(0, returnData);
	}
	memcpy(&so,result->ai_addr,result->ai_addrlen);
	so.sin_port=htons(PortV);
	so.sin_family=AF_INET;
	freeaddrinfo(result);
	SOCKET sock=0;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int R=0;
	if ( 0!= connect(sock,(sockaddr*)&so,sizeof(so)))
	{
		R=WSAGetLastError();
		closesocket(sock);
		//lua_pushstring(L,"httpGet - connect failed");
		//lua_error(L);
	}
	strcpy(Buf,"GET ");
	if (*P==0)
		P="/";
	strcat(Buf,P);
	strcat(Buf," HTTP/1.0\r\nHost: ");
	strcat(Buf, HostAddr);
	strcat(Buf,"\r\n\r\n");
	R=send(sock,Buf,strlen(Buf),0);
	char httpGetResultData[0x400];
	int resultCode=0;
	if (R<0)
	{
		R=WSAGetLastError();
		if (R==WSANOTINITIALISED)
			R++;

	}
	else
	{
		shutdown(sock,SD_SEND);
		int R=0;
		if ((R=recv(sock,Buf,sizeof(Buf),0))>=0)
		{
			char* statCodeSearch=strstr(Buf, "\r\n");
			if(statCodeSearch!=NULL && (statCodeSearch-Buf)<50)
			{
				char* statCode = strstr(Buf, "200");
				if(statCode!=NULL)
					resultCode=200;
			}
			P=strstr(Buf,"\r\n\r\n");
			if (P!=NULL)
				strcpy(httpGetResultData, P+4);
		}
		else
		{
			R=WSAGetLastError();
			//httpGetResult=NULL;
		}
	}
	closesocket(sock);
	//strcpy(httpGetSrc, Src);
	strcpy(returnData, httpGetResultData);
	return std::make_pair(resultCode, returnData);
}