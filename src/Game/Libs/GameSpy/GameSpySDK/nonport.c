/******
nonport.c
GameSpy Common Code

Copyright 1999-2003 GameSpy Industries, Inc

******/
/*
 * cjacksrc: the SDK's platform functions as they were in 2003, Win32 only,
 * in the order the retail GameSpy.dll has them (0x1000dfe0..0x1000e5b5).
 * Written for this project from the 2007 snapshot's common/gsPlatformUtil.c
 * and gsPlatformSocket.c, which kept these functions nearly unchanged.
 */

#include "nonport.h"

#pragma comment(lib, "wsock32.lib")

#define REGKEY "Software\\GameSpy\\GameSpy 3D\\Registration"

gsi_time current_time()  //returns current time in msec
{
	return (GetTickCount());
}

void msleep(gsi_time msec)
{
	Sleep(msec);
}

void SocketStartUp()
{
	WSADATA data;
	WSAStartup(MAKEWORD(1,1), &data);
}

void SocketShutDown()
{
	WSACleanup();
}

char * goastrdup(const char *src)
{
	char *res;
	if(src == NULL)      //PANTS|02.11.00|check for NULL before strlen
		return NULL;
	res = (char *)gsimalloc(strlen(src) + 1);
	if(res != NULL)      //PANTS|02.02.00|check for NULL before strcpy
		strcpy(res, src);
	return res;
}

int SetSockBlocking(SOCKET sock, int isblocking)
{
	int rcode;
	unsigned long argp;

	if(isblocking)
		argp = 0;
	else
		argp = 1;
	rcode = ioctlsocket(sock, FIONBIO, &argp);

	if(rcode == 0)
		return 1;
	return 0;
}

int DisableNagle(SOCKET sock)
{
	int rcode;
	int noDelay = 1;

	rcode = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&noDelay, sizeof(int));

	return gsiSocketIsNotError(rcode);
}

int SetReceiveBufferSize(SOCKET sock, int size)
{
	int rcode;
	rcode = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(int));
	return gsiSocketIsNotError(rcode);
}

int SetSendBufferSize(SOCKET sock, int size)
{
	int rcode;
	rcode = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(int));
	return gsiSocketIsNotError(rcode);
}

int GetReceiveBufferSize(SOCKET sock)
{
	int rcode;
	int size;
	int len;

	len = sizeof(size);

	rcode = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&size, &len);

	if(gsiSocketIsError(rcode))
		return rcode;

	return size;
}

int GetSendBufferSize(SOCKET sock)
{
	int rcode;
	int size;
	int len;

	len = sizeof(size);

	rcode = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&size, &len);

	if(gsiSocketIsError(rcode))
		return rcode;

	return size;
}

int CanReceiveOnSocket(SOCKET sock)
{
	fd_set fd;
	struct timeval timeout;
	int rcode;

	FD_ZERO(&fd);
	FD_SET(sock, &fd);

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	rcode = select(FD_SETSIZE, &fd, NULL, NULL, &timeout);
	if(gsiSocketIsError(rcode) || (rcode == 0))
		return 0;

	return 1;
}

int CanSendOnSocket(SOCKET sock)
{
	fd_set fd;
	struct timeval timeout;
	int rcode;

	FD_ZERO(&fd);
	FD_SET(sock, &fd);

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	rcode = select(FD_SETSIZE, NULL, &fd, NULL, &timeout);
	if(gsiSocketIsError(rcode) || (rcode == 0))
		return 0;

	return 1;
}

HOSTENT * getlocalhost(void)
{
	char hostname[256] = "";

	// get the local host's name
	gethostname(hostname, sizeof(hostname));

	// return the host for that name
	return gethostbyname(hostname);
}

int IsPrivateIP(IN_ADDR * addr)
{
	int b1;
	int b2;
	unsigned int ip;

	// get the first 2 bytes
	ip = ntohl(addr->s_addr);
	b1 = (int)(ip >> 24);
	b2 = (int)((ip >> 16) & 0xFF);

	// 10.X.X.X
	if(b1 == 10)
		return 1;

	// 172.16-31.X.X
	if((b1 == 172) && ((b2 >= 16) && (b2 <= 31)))
		return 1;

	// 192.168.X.X
	if((b1 == 192) && (b2 == 168))
		return 1;

	return 0;
}

/* Unique ID: a random key, kept in the registry */

static long randSeed = 1;

static long nextlongrand(long seed)
{
	unsigned long lo, hi;

	lo = 16807 * (long)(seed & 0xFFFF);
	hi = 16807 * (long)((unsigned long)seed >> 16);
	lo += (hi & 0x7FFF) << 16;
	if (lo > 2147483647)
	{
		lo &= 2147483647;
		++lo;
	}
	lo += hi >> 15;
	if (lo > 2147483647)
	{
		lo &= 2147483647;
		++lo;
	}
	return (long)lo;
}

static long longrand(void)
{
	randSeed = nextlongrand(randSeed);
	return randSeed;
}

static void Util_RandSeed(unsigned long seed)
{
	randSeed = (seed == 0) ? 1 : (long)(seed & 2147483647);
}

static int Util_RandInt(int low, int high)
{
	int range = high-low;
	int num;

	num = longrand() % range;

	return(num + low);
}

static void GenerateID(char *keyval)
{
	int i;
	const char crypttab[63] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	LARGE_INTEGER l1;
	UINT seed;

	if (QueryPerformanceCounter(&l1))
		seed = (l1.LowPart ^ l1.HighPart);
	else
		seed = 0;

	Util_RandSeed(time(NULL) ^ clock() ^ GetTickCount() ^ seed);
	for (i = 0; i < 19; i++)
		if (i == 4 || i == 9 || i == 14)
			keyval[i] = '-';
		else
			keyval[i] = crypttab[Util_RandInt(0, 62)];
	keyval[19] = 0;
}

const char * GOAGetUniqueID(void)
{
	static char keyval[MAX_PATH] = "";
	unsigned int ret;
	int docreate;
	HKEY thekey;
	DWORD thetype = REG_SZ;
	DWORD len = MAX_PATH;
	DWORD disp;

	if (RegOpenKeyExA(HKEY_CURRENT_USER, REGKEY, 0, KEY_ALL_ACCESS, &thekey) != ERROR_SUCCESS)
		docreate = 1;
	else
		docreate = 0;
	ret = RegQueryValueExA(thekey, "Crypt", 0, &thetype, (LPBYTE)keyval, &len);

	if (ret != ERROR_SUCCESS || strlen(keyval) != 19) //need to generate a new key
	{
		GenerateID(keyval);
		if (docreate)
		{
			ret = RegCreateKeyExA(HKEY_CURRENT_USER, REGKEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &thekey, &disp);
		}
		RegSetValueExA(thekey, "Crypt", 0, REG_SZ, (const LPBYTE)keyval, strlen(keyval)+1);
	}
	RegCloseKey(thekey);

	// Strip out the -'s.
	memmove(keyval + 4, keyval + 5, 4);
	memmove(keyval + 8, keyval + 10, 4);
	memmove(keyval + 12, keyval + 15, 4);
	keyval[16] = '\0';

	return keyval;
}
