/******
nonport.h
GameSpy Common Code

Copyright 1999-2003 GameSpy Industries, Inc

******/
/*
 * cjacksrc: the GameSpy SDK's platform layer as it was in 2003 (common code
 * 1.10.3x, July-October 2003), when it was still this one header and
 * nonport.c, Win32 only. Written for this project from the 2007 snapshot's
 * common/ (gsPlatform.h, gsPlatformSocket.h, gsPlatformUtil.h, gsMemory.h,
 * gsAssert.h, gsDebug.h, gsStringUtil.h), cut down to what the modules
 * GameSpy.dll links use. The 2007 modules' later additions (debug output,
 * the availability check, UNICODE wrappers) compile away here.
 */

#ifndef _NONPORT_H_
#define _NONPORT_H_

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// Types

typedef char              gsi_i8;
typedef unsigned char     gsi_u8;
typedef short             gsi_i16;
typedef unsigned short    gsi_u16;
typedef int               gsi_i32;
typedef unsigned int      gsi_u32;
typedef unsigned int      gsi_time;
typedef __int64           gsi_i64;
typedef unsigned __int64  gsi_u64;
typedef gsi_i32           goa_int32;
typedef gsi_u32           goa_uint32;

typedef int               gsi_bool;
#define gsi_false         ((gsi_bool)0)
#define gsi_true          ((gsi_bool)1)
#define gsi_is_false(x)   ((x) == gsi_false)
#define gsi_is_true(x)    ((x) != gsi_false)

#define GSI_MIN_I32       INT_MIN
#define GSI_MAX_I32       INT_MAX
#define GSI_MAX_U32       UINT_MAX
#define GSI_MAX_INTEGRAL_BITS 64

// no UNICODE interface in this build: gsi_char is char
#define gsi_char          char
#define _tcscpy           strcpy
#define _tcsncpy          strncpy
#define _tcscat           strcat
#define _tcslen           strlen
#define _tcschr           strchr
#define _tcscmp           strcmp
#define _tfopen           fopen
#define _tsnprintf        _snprintf
#define _T(a)             a

#define snprintf          _snprintf
#define strcasecmp        _stricmp
#define strncasecmp       _strnicmp

#define GS_STATIC_CALLBACK __cdecl
#define GSI_LITTLE_ENDIAN

#define GSI_UNUSED(x)     x

#ifndef GSI_DOMAIN_NAME
	#define GSI_DOMAIN_NAME "gamespy.com"
#endif

// ---------------------------------------------------------------------------
// Memory (the 2003 SDK had only these defines)

#define gsimalloc         malloc
#define gsifree           free
#define gsirealloc        realloc
#define gsimemalign(a,s)  malloc(s)

// ---------------------------------------------------------------------------
// Asserts and debug output. The retail image's asserts are live: the SDK's
// asserts were plain assert() then.

#define GS_ASSERT(x)      {assert(x);}
#define GS_FAIL()         {assert(0);}

// the 2007 modules' debug output: compiled away (VC 7.1 has no variadic
// macros, so a call becomes a discarded comma expression)
typedef enum
{
	GSIDebugCat_App, GSIDebugCat_GP, GSIDebugCat_Peer, GSIDebugCat_QR2,
	GSIDebugCat_SB, GSIDebugCat_Voice, GSIDebugCat_AD, GSIDebugCat_NatNeg,
	GSIDebugCat_HTTP, GSIDebugCat_CDKey, GSIDebugCat_Common
} GSIDebugCategory;
typedef enum
{
	GSIDebugType_Network, GSIDebugType_File, GSIDebugType_Memory,
	GSIDebugType_State, GSIDebugType_Misc
} GSIDebugType;
typedef enum
{
	GSIDebugLevel_None = 0, GSIDebugLevel_HotError = 1, GSIDebugLevel_WarmError = 2,
	GSIDebugLevel_Warning = 4, GSIDebugLevel_Notice = 8, GSIDebugLevel_Comment = 16,
	GSIDebugLevel_RawDump = 32, GSIDebugLevel_StackTrace = 64, GSIDebugLevel_Debug = 127
} GSIDebugLevel;
#define gsDebugFormat     1 ? (void)0 : (void)
#define gsDebugBinary     1 ? (void)0 : (void)
#define gsDebugTTyFormat  1 ? (void)0 : (void)

// ---------------------------------------------------------------------------
// Sockets

#define gsiSocketIsError(theReturnValue)     ((theReturnValue) == -1)
#define gsiSocketIsNotError(theReturnValue)  ((theReturnValue) != -1)

#define GOAGetLastError(s) WSAGetLastError()

#ifndef INADDR_NONE
	#define INADDR_NONE 0xffffffff
#endif

int SetSockBlocking(SOCKET sock, int isblocking);
int DisableNagle(SOCKET sock);
int SetReceiveBufferSize(SOCKET sock, int size);
int SetSendBufferSize(SOCKET sock, int size);
int GetReceiveBufferSize(SOCKET sock);
int GetSendBufferSize(SOCKET sock);
int CanReceiveOnSocket(SOCKET sock);
int CanSendOnSocket(SOCKET sock);
HOSTENT * getlocalhost(void);
int IsPrivateIP(IN_ADDR * addr);

void SocketStartUp(void);
void SocketShutDown(void);

// ---------------------------------------------------------------------------
// Utilities

gsi_time current_time(void);
void msleep(gsi_time msec);
char * goastrdup(const char *src);
const char * GOAGetUniqueID(void);

// Synchronous name resolution only (the 2007 ghttp resolves on a thread
// unless GSI_NO_THREADS: 2003's never did)
#define GSI_NO_THREADS

// ---------------------------------------------------------------------------
// The 2007 modules' availability check (gsAvailable, November 2003, after
// Contract J.A.C.K.): every service is taken as available

typedef enum
{
	GSIACWaiting,
	GSIACAvailable,
	GSIACUnavailable,
	GSIACTemporarilyUnavailable
} GSIACResult;


#define GSI_MIN(a,b)              (((a) < (b)?(a):(b)))
#define GSI_MAX(a,b)              (((a) > (b)?(a):(b)))

#ifdef __cplusplus
}
#endif

#endif
