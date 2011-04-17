#include "stdafx.h"
#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <iterator>
#include <process.h>
#include "Libs/csha1/SHA1.h"


#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <winsock.h>
#include <winsock2.h>
#pragma comment(lib,"wsock32.lib")
#pragma comment (lib, "Ws2_32.lib")
#define snprintf _snprintf_s
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
//#include <process.h>
//#include <queue>
//#include "stdafx.h"
using namespace std ;
#define SERVER "Nox UniMod/0.4.1"
#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"
#define PORT 80


using namespace std;

extern byte authorisedState[0x20];
extern char* authorisedLogins[0x20];
extern bool specialAuthorisation; //Отключение альтернативной авторизации
extern char authSendWelcomeMsg[0x20];


namespace
{
	queue<bool> updateAuthDB;
	
	map<byte, char*> notLoggedIn;

	bool authUpdating=false;
	HANDLE authUpdate;

	char* remoteCommand;
	int lastAuthResultCode;
	bool specialAuthRemote=false;
	bool remoteAuthUpdating=false;
	HANDLE remoteAuthUpdate;
	queue<char*> remoteCommandList;

	int lastAuthResultCodeL;
	HANDLE remoteAuthLogin;
	bool remoteAuthLoggingIn=false;
	queue <byte>notLoggedInRemote;

	struct sha1hash
	{
		unsigned char val[20];

		sha1hash()
		{
			memset(val, '\0', 20);
		}

		sha1hash(const sha1hash& oth)
		{
			memmove(val, oth.val, 20);
		}

		sha1hash(const unsigned char * str)
		{
			memset(val, '\0', 20);
			if (str)
			{
				strncpy((char*)val, (char*)str, 20);
			}
		}

		// required for 'map', 'set', etc
		bool operator<(const sha1hash& oth) const
		{
			return strncmp((char*)val , (char*)oth.val, 20) < 0;
		}
	};



	struct account
	{
		char login[50];
		unsigned char phash[20];
		bool isActive;
		bool isAdmin;
		char unused[8];
	};

	typedef map<sha1hash, account> authMap;
	authMap authData;


	string urlencode(const string &c);
	string char2hex( char dec );

	string urlencode(const string &c)
	{
	    
		string escaped="";
		int max = c.length();
		for(int i=0; i<max; i++)
		{
			if ( (48 <= c[i] && c[i] <= 57) ||//0-9
				 (65 <= c[i] && c[i] <= 90) ||//abc...xyz
				 (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
				 (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'')
			)
			{
				escaped.append( &c[i], 1);
			}
			else
			{
				escaped.append("%");
				escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
			}
		}
		return escaped;
	}

	string char2hex( char dec )
	{
		char dig1 = (dec&0xF0)>>4;
		char dig2 = (dec&0x0F);
		if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
		if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
		if ( 0<= dig2 && dig2<= 9) dig2+=48;
		if (10<= dig2 && dig2<=15) dig2+=97-10;

		string r;
		r.append( &dig1, 1);
		r.append( &dig2, 1);
		return r;
	}
	

	unsigned __stdcall authRemoteCommand(void* command)
	{
		string urlEncode((char*)command);
		string urlEncoded=urlencode(urlEncode);
		char* resultCommand=new char[strlen(urlEncoded.c_str())+strlen(remoteCommand)+1];
		strcpy(resultCommand, remoteCommand);
		strcat(resultCommand, urlEncoded.c_str());
		const char *Src=0,*Host=0,*Port=0,*Href=0,*P;
		Src=(char*)resultCommand;
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
			return 0;
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
		//char httpGetResultData[0x400];
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
				/*P=strstr(Buf,"\r\n\r\n");
				if (P!=NULL)
					strcpy(httpGetResultData, P+4);*/
			}
			else
			{
				R=WSAGetLastError();
				//httpGetResult=NULL;
			}
		}
		closesocket(sock);
		//strcpy(httpGetSrc, Src);
		lastAuthResultCode=resultCode;
		return 0;
	}

	unsigned __stdcall authRemoteLogin(void* command)
	{
		string urlEncode((char*)command);
		string urlEncoded=urlencode(urlEncode);
		char* resultCommand=new char[strlen(urlEncoded.c_str())+strlen(remoteCommand)+1];
		strcpy(resultCommand, remoteCommand);
		strcat(resultCommand, urlEncoded.c_str());
		const char *Src=0,*Host=0,*Port=0,*Href=0,*P;
		Src=(char*)resultCommand;
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
			Buf[P-Port]=NULL;
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
			return 0;
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
		//char httpGetResultData[0x400];
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
				/*P=strstr(Buf,"\r\n\r\n");
				if (P!=NULL)
					strcpy(httpGetResultData, P+4);*/
			}
			else
			{
				R=WSAGetLastError();
				//httpGetResult=NULL;
			}
		}
		closesocket(sock);
		//strcpy(httpGetSrc, Src);
		lastAuthResultCodeL=resultCode;
		return 0;
	}

	unsigned __stdcall saveAuthData(void* Data)
	{
		ofstream file("authData.bin", ios::out | ios::trunc | ios::binary);
		for (authMap::const_iterator it = authData.begin(); it != authData.end(); ++it)
		{
			char rawData[100];
			strncpy(rawData, (char*)it->first.val, 20);
			memcpy(&rawData[20], &it->second, 80);
			char zero=0;
			memcpy(&rawData[92], &zero, 8);
			file.write(rawData, 100);
		}
		file.close();
		return true;
	}

	bool authRegister(char* login, char* pass)
	{
		CSHA1 sha;
		sha.Update((const unsigned char*)login, strlen(login));
		sha.Final();
		unsigned char lhash[20];
		sha.GetHash(lhash);
		sha.Reset();
		sha.Update((const unsigned char*)pass, strlen(pass));
		sha.Final();
		unsigned char phash[20];
		sha.GetHash(phash);
		sha.Reset();
		account* newAcc = new account;
		memset(newAcc->login, '\0', 50);
		strncpy(newAcc->login, login, ((strlen(login)>50)?50:strlen(login)));
		strncpy((char*)newAcc->phash, (char*)phash, 20);
		newAcc->isActive=true;
		newAcc->isAdmin=false;
		memset(newAcc->unused, '\0', 8);
		pair<sha1hash, account> newPair = pair<sha1hash, account>(sha1hash(lhash), *newAcc);
		if(authData.insert(newPair).second)
		{
			updateAuthDB.push(true);
			return true;
		}
		return false;
	}

	bool authLogin(char* login, char* pass)
	{
		CSHA1 sha;
		sha.Update((const unsigned char*)login, strlen(login));
		sha.Final();
		unsigned char lhash[20];
		sha.GetHash(lhash);
		sha.Reset();
		if(authData.find(sha1hash(lhash))!=authData.end() && strncmp(authData[sha1hash(lhash)].login, login, 50)==0 && authData[sha1hash(lhash)].isActive==true)
		{
			sha.Update((const unsigned char*)pass, strlen(pass));
			sha.Final();
			unsigned char phash[20];
			sha.GetHash(phash);
			sha.Reset();
			if(strncmp((char*)authData[sha1hash(lhash)].phash, (char*)phash, 20)==0)
				return true;
		}
		return false;
	}

	bool authLock(char* login)
	{
		CSHA1 sha;
		sha.Update((const unsigned char*)login, strlen(login));
		sha.Final();
		unsigned char lhash[20];
		sha.GetHash(lhash);
		sha.Reset();
		if(authData.find(sha1hash(lhash))!=authData.end() && strncmp(authData[sha1hash(lhash)].login, login, 50)==0)
		{
			if(authData[sha1hash(lhash)].isActive==true)
				authData[sha1hash(lhash)].isActive=false;
			else
				authData[sha1hash(lhash)].isActive=true;
			updateAuthDB.push(true);
			return true;
		}
		return false;
	}

	bool authDelete(char* login)
	{
		CSHA1 sha;
		sha.Update((const unsigned char*)login, strlen(login));
		sha.Final();
		unsigned char lhash[20];
		sha.GetHash(lhash);
		sha.Reset();
		if(authData.find(sha1hash(lhash))!=authData.end() && strncmp(authData[sha1hash(lhash)].login, login, 50)==0)
		{
			authMap::iterator it = authData.find(sha1hash(lhash));
			account* acc=&it->second;
			//sha1hash* lhash=&it->first;
			authData.erase(it);
			
			//delete acc;
			//delete lhash;
			updateAuthDB.push(true);
			return true;
		}
		return false;
	}

	bool authChangePass(char* login, char* pass)
	{
		CSHA1 sha;
		sha.Update((const unsigned char*)login, strlen(login));
		sha.Final();
		unsigned char lhash[20];
		sha.GetHash(lhash);
		sha.Reset();
		if(authData.find(sha1hash(lhash))!=authData.end() && strncmp(authData[sha1hash(lhash)].login, login, 50)==0)
		{
			sha.Update((const unsigned char*)pass, strlen(pass));
			sha.Final();
			sha.GetHash(authData[sha1hash(lhash)].phash);
			sha.Reset();
			updateAuthDB.push(true);
			return true;
		}
		return false;
	}

	void authentificate()
	{
		/*int lastAuthResultCodeL;
		HANDLE remoteAuthLogin;
		bool remoteAuthLoggingIn=false;
		queue <byte>notLoggedInRemote;*/
		if(WAIT_OBJECT_0==WaitForSingleObject(remoteAuthLogin, 0) && remoteAuthLoggingIn==true && specialAuthRemote==true)
		{
			byte playerIdx = notLoggedInRemote.front();
			notLoggedIn.erase(playerIdx);
			notLoggedInRemote.pop();
			if(lastAuthResultCodeL==200)
				authorisedState[playerIdx]++;
			else
			{
				authorisedState[playerIdx]-=2;
				delete [] authorisedLogins[playerIdx];
				authorisedLogins[playerIdx]="";
			}
			authSendWelcomeMsg[playerIdx]=-1;
			remoteAuthLoggingIn=false;
		}
		if(notLoggedInRemote.empty()==false && remoteAuthLoggingIn==false && specialAuthRemote==true)
		{
			byte playerIdx = notLoggedInRemote.front();
			char* command=new char[11+6+5+2+2+strlen(authorisedLogins[playerIdx])+strlen(notLoggedIn[playerIdx])+4+1];
			strcpy(command, "authLogin({login=\"");
			strcat(command, authorisedLogins[playerIdx]);
			strcat(command, "\",pass=\"");
			strcat(command, notLoggedIn[playerIdx]);
			strcat(command, "\"})");
			remoteCommandList.push(command);
			remoteAuthLogin = (HANDLE)_beginthreadex(NULL, 0, &authRemoteLogin, (void*)command, 0, NULL);
			remoteAuthLoggingIn=true;
		}
		if(notLoggedIn.empty()!=true && specialAuthRemote==false)
		{
			for (map<byte, char*>::const_iterator it = notLoggedIn.begin(); it != notLoggedIn.end(); ++it)
			{
				if(authLogin(authorisedLogins[it->first], it->second))
					authorisedState[it->first]++;
				else
				{
					authorisedState[it->first]-=2;
					delete [] authorisedLogins[it->first];
					authorisedLogins[it->first]="";
				}
				authSendWelcomeMsg[it->first]=-1;
				delete [] it->second;
			}
			notLoggedIn.clear();
		}
	}

	int authRegisterL(lua_State *L)
	{
		if (lua_type(L,1)==LUA_TTABLE)
		{
			char* login;
			char* pass;
			lua_getfield(L,1,"login");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				const char* loginc=lua_tostring(L,-1);
				login=new char[strlen(loginc)+1];
				strcpy(login,loginc);
			}
			else
			{
				lua_pushstring(L,"wrong args!");
				lua_error_(L);
			}
			lua_getfield(L,1,"pass");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				const char* passc=lua_tostring(L,-1);
				pass=new char[strlen(passc)+1];
				strcpy(pass,passc);
			}
			else
			{
				lua_pushstring(L,"wrong args!");
				lua_error_(L);
			}
			if(specialAuthRemote)
			{
				char *command=new char[14+6+5+2+2+strlen(login)+strlen(pass)+4+1];
				strcpy(command, "authRegister({login=\"");
				strcat(command, login);
				strcat(command, "\",pass=\"");
				strcat(command, pass);
				strcat(command, "\"})");
				remoteCommandList.push(command);
			}
			else if(!authRegister(login, pass))
			{
				lua_pushstring(L,"couldn't register");
				lua_error_(L);
			}
			delete [] login;
			delete [] pass;
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushstring(L, "ok");
		return 1;
	}

	int authChangePassL(lua_State *L)
	{
		if (lua_type(L,1)==LUA_TTABLE)
		{
			char* login;
			char* pass;
			lua_getfield(L,1,"login");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				const char* loginc=lua_tostring(L,-1);
				login=new char[strlen(loginc)+1];
				strcpy(login,loginc);
			}
			else
			{
				lua_pushstring(L,"wrong args!");
				lua_error_(L);
			}
			lua_getfield(L,1,"pass");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				const char* passc=lua_tostring(L,-1);
				pass=new char[strlen(passc)+1];
				strcpy(pass,passc);
			}
			else
			{
				lua_pushstring(L,"wrong args!");
				lua_error_(L);
			}
			if(specialAuthRemote)
			{
				char *command=new char[16+6+5+2+2+strlen(login)+strlen(pass)+4+1];
				strcpy(command, "authChangePass({login=\"");
				strcat(command, login);
				strcat(command, "\",pass=\"");
				strcat(command, pass);
				strcat(command, "\"})");
				remoteCommandList.push(command);
			}
			else if(!authChangePass(login, pass))
			{
				lua_pushstring(L,"couldn't change password");
				lua_error_(L);
			}
			delete [] login;
			delete [] pass;
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushstring(L, "ok");
		return 1;
	}

	int authLoginL(lua_State *L)
	{
		if (lua_type(L,1)==LUA_TTABLE)
		{
			char* login;
			char* pass;
			lua_getfield(L,1,"login");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				const char* loginc=lua_tostring(L,-1);
				login=new char[strlen(loginc)+1];
				strcpy(login,loginc);
			}
			else
			{
				lua_pushstring(L,"wrong args!");
				lua_error_(L);
			}
			lua_getfield(L,1,"pass");
			if (lua_type(L,-1)!=LUA_TNIL)
			{
				const char* passc=lua_tostring(L,-1);
				pass=new char[strlen(passc)+1];
				strcpy(pass,passc);
			}
			else
			{
				lua_pushstring(L,"wrong args!");
				lua_error_(L);
			}
			if(specialAuthRemote)
			{
				char *command=new char[11+6+5+2+2+strlen(login)+strlen(pass)+4+1];
				strcpy(command, "authLogin({login=\"");
				strcat(command, login);
				strcat(command, "\",pass=\"");
				strcat(command, pass);
				strcat(command, "\"})");
				remoteCommandList.push(command);
			}
			else if(!authLogin(login, pass))
			{
				lua_pushstring(L,"couldn't login");
				lua_error_(L);
			}
			delete [] login;
			delete [] pass;
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		lua_pushstring(L, "ok");
		return 1;
	}

	int authLockL(lua_State *L)
	{
		if (lua_type(L,1)==LUA_TSTRING)
		{
			char* login;
			const char* loginc=lua_tostring(L, 1);
			login=new char[strlen(loginc)+1];
			strcpy(login, loginc);
			if(specialAuthRemote)
			{
				char *command=new char[10+strlen(login)+2+1];
				strcpy(command, "authLock(\"");
				strcat(command, login);
				strcat(command, "\")");
				remoteCommandList.push(command);
			}
			else
				authLock(login);
			delete [] login;
		}
		else
		{
			lua_pushstring(L,"wrong args");
			lua_error_(L);
		}
		lua_pushstring(L, "ok");
		return 1;
	}

	int authDeleteL(lua_State *L)
	{
		if (lua_type(L,1)==LUA_TSTRING)
		{
			char* login;
			const char* loginc=lua_tostring(L, 1);
			login=new char[strlen(loginc)+1];
			strcpy(login, loginc);
			if(specialAuthRemote)
			{
				char *command=new char[12+strlen(login)+2+1];
				strcpy(command, "authDelete(\"");
				strcat(command, login);
				strcat(command, "\")");
				remoteCommandList.push(command);
			}
			else
				authDelete(login);
			delete [] login;
		}
		else
		{
			lua_pushstring(L,"wrong args");
			lua_error_(L);
		}
		lua_pushstring(L, "ok");
		return 1;
	}

	int authToggleL(lua_State *L)
	{
		if(specialAuthorisation)
		{
			specialAuthorisation=false;
			specialAuthRemote=false;
			remoteCommand=NULL;
		}
		else
			specialAuthorisation=true;
		if (lua_type(L,1)==LUA_TTABLE && specialAuthorisation==true)
		{
			lua_getfield(L, 1, "host");
			if(lua_type(L, -1)!=LUA_TSTRING)
			{
				lua_pushstring(L,"wrong args");
				lua_error_(L);
			}
			const char* hostc=lua_tostring(L,-1);
			char* host=new char[strlen(hostc)+1];
			strcpy(host,hostc);
			lua_getfield(L, 1, "port");
			if(lua_type(L, -1)!=LUA_TSTRING && lua_type(L, -1)!=LUA_TNUMBER)
			{
				lua_pushstring(L,"wrong args");
				lua_error_(L);
			}
			const char* portc=lua_tostring(L,-1);
			char* port=new char[strlen(portc)+1];
			strcpy(port,portc);
			lua_getfield(L, 1, "pass");
			bool passProvided=false;
			char* pass;
			if(lua_type(L, -1)==LUA_TSTRING)
			{
				passProvided=true;
				const char* passc=lua_tostring(L,-1);
				pass=new char[strlen(passc)+1];
				strcpy(pass,passc);
			}
			char* commandString=new char[7+strlen(host)+1+strlen(port)+2+(passProvided?(2+strlen(pass)+1):(0))+2+1];
			strcpy(commandString, "http://");
			strcat(commandString, host);
			strcat(commandString, ":");
			strcat(commandString, port);
			strcat(commandString, "/?");
			if(passProvided)
			{
				strcat(commandString, "p=");
				strcat(commandString, pass);
				strcat(commandString, "&");
			}
			strcat(commandString, "r=");
			delete [] host;
			delete [] port;
			if(passProvided)
				delete [] pass;
			remoteCommand=commandString;
			specialAuthRemote=true;
		}
		return 1;
	}

	int playerGetByLogin(lua_State *L)
	{
		if(lua_type(L, -1)==LUA_TSTRING)
		{
			const char *S=lua_tostring(L,-1);
			bool found=false;
			for(void *Pl=playerFirstUnit(); Pl!=0; Pl=playerNextUnit(Pl))
			{
				void **PP=(void **)(((char*)Pl)+0x2EC);
				PP=(void**)(((char*)*PP)+0x114);
				byte *P=(byte*)(*PP);
				
				byte playerIdx = *((byte*)(P+0x810));
				if(strncmp(authorisedLogins[playerIdx],S, 50)==0)
				{
					found=true;
					lua_pushlightuserdata(L, Pl);
					break;
				}
			}
			if(found==false)
			{
				lua_pushstring(L,"not found!");
				lua_error_(L);
			}
		}
		else
		{
			lua_pushstring(L,"wrong args!");
			lua_error_(L);
		}
		return 1;
	}
}

void authInit(lua_State *L)
{
	registerserver("authRegister",&authRegisterL);
	registerserver("authLock",&authLockL);
	registerserver("authLogin",&authLoginL);
	registerserver("authChangePass",&authChangePassL);
	registerserver("authToggle",&authToggleL);
	registerserver("authDelete",&authDeleteL);
	registerserver("playerGetByLogin",&playerGetByLogin);
}
bool initAuthData()
{
	ifstream file("authData.bin", ios::in | ios::binary);
	if(file.is_open())
	{
		file.seekg(0, ios::end);
		size_t filesize=file.tellg();
		for(size_t i=0; (i+100)<=filesize; i+=100)
		{
			file.seekg(i, ios::beg);
			unsigned char *loginhash = new unsigned char[20];
			file.read((char*)loginhash, 20);
			file.seekg(i+20, ios::beg);
			char logindata[80];
			file.read(logindata, 80);
			account *authEntry = new account;
			memcpy(authEntry, logindata, 80);
			authData.insert(pair<sha1hash, account>(sha1hash(loginhash), *authEntry));
		}
		file.close();
		return true;
	}
	file.close();
	return false;
}

void updateAuthDBProcess()
{
	if(WAIT_OBJECT_0==WaitForSingleObject(authUpdate, 0) && authUpdating==true)
	{
		updateAuthDB.pop();
		authUpdating=false;
	}
	if(updateAuthDB.empty()==false && authUpdating==false)
	{
		authUpdate = (HANDLE)_beginthreadex(NULL, 0, &saveAuthData, NULL, 0, NULL);
		authUpdating=true;
	}
	authentificate();
	if(specialAuthRemote)
	{
		if(WAIT_OBJECT_0==WaitForSingleObject(remoteAuthUpdate, 0) && remoteAuthUpdating==true)
		{
			char buf[200];
			strcpy(buf, remoteCommandList.front());
			strcat(buf, " executed, state: ");
			if(lastAuthResultCode==200)
				strcat(buf, "success!");
			else
				strcat(buf, "error!");
			conPrintI(buf);
			delete [] remoteCommandList.front();
			remoteCommandList.pop();
			remoteAuthUpdating=false;
		}
		if(remoteCommandList.empty()==false && remoteAuthUpdating==false)
		{
			remoteAuthUpdate = (HANDLE)_beginthreadex(NULL, 0, &authRemoteCommand, (void*)remoteCommandList.front(), 0, NULL);
			remoteAuthUpdating=true;
		}
	}
	return;
}

void authCheckDelayed(byte playerIdx, char* pass)
{
	/*if(specialAuthRemote)
	{
		char command=new char[11+6+5+2+2+strlen(authorisedLogins[playerIdx])+strlen(pass)+4+1];
		strcpy(command, 'authLogin({login="');
		strcat(command, login);
		strcat(command, '", pass="');
		strcat(command, '"})');
		remoteCommandList.push(command);
	}*/

	
	
	notLoggedIn.insert(pair<byte, char*>(playerIdx, pass));
	if(specialAuthRemote)
		notLoggedInRemote.push(playerIdx);
}