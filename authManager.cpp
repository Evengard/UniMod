#include "stdafx.h"
#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <iterator>
#include <process.h>
#include "Libs/csha1/SHA1.h"

using namespace std;

extern byte authorisedState[0x20];
extern char* authorisedLogins[0x20];

namespace
{
	queue<bool> updateAuthDB;
	map<byte, char*> notLoggedIn;
	bool authUpdating=false;
	HANDLE authUpdate;

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

	void authentificate()
	{
		if(notLoggedIn.empty()!=true)
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
			if(!authRegister(login, pass))
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
		return 0;
	}

	int authLockL(lua_State *L)
	{
		if (lua_type(L,1)==LUA_TSTRING)
		{
			char* login;
			const char* loginc=lua_tostring(L, 1);
			login=new char[strlen(loginc)+1];
			strcpy(login, loginc);
			authLock(login);
			delete [] login;
		}
		else
		{
			lua_pushstring(L,"couldn't register");
			lua_error_(L);
		}
		return 0;
	}
}

bool initAuthData()
{
	registerserver("authRegister",&authRegisterL);
	registerserver("authLock",&authLockL);


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
	return;
}

void authCheckDelayed(byte playerIdx, char* pass)
{
	notLoggedIn.insert(pair<byte, char*>(playerIdx, pass));
}