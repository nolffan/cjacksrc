 /*
GameSpy GHTTP SDK 
Dan "Mr. Pants" Schoenblum
dan@gamespy.com

Copyright 1999-2007 GameSpy Industries, Inc

devsupport@gamespy.com
*/

#ifndef _GHTTPCOMMON_H_
#define _GHTTPCOMMON_H_

#include "ghttp.h"
#include "ghttpConnection.h"

#ifdef __cplusplus
extern "C" {
#endif

// HTTP Line-terminator.
////////////////////////
#define CRLF    "\xD\xA"

// HTTP URL Encoding 
////////////////////////
#define GHI_LEGAL_URLENCODED_CHARS      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_@-.*"
#define GHI_DIGITS                      "0123456789ABCDEF"

// Default HTTP port.
/////////////////////
#define GHI_DEFAULT_PORT                      80
#define GHI_DEFAULT_SECURE_PORT               443
#define GHI_DEFAULT_THROTTLE_BUFFER_SIZE      125
#define GHI_DEFAULT_THROTTLE_TIME_DELAY       250

// Proxy server.
////////////////
extern char * ghiProxyAddress;
extern unsigned short ghiProxyPort;

// Throttle settings.
/////////////////////
extern int ghiThrottleBufferSize;
extern gsi_time ghiThrottleTimeDelay;

// Our thread lock.
///////////////////
void ghiCreateLock(void);
void ghiFreeLock(void);
void ghiLock(void);
void ghiUnlock(void);

// Do logging.
//////////////
#ifdef HTTP_LOG
void ghiLogToFile
(
	const char * buffer,
	int len,
	const char * fileName
);
#define ghiLogRequest(b,c)  ghiLogToFile(b,c,"request.log");
#define ghiLogResponse(b,c)	ghiLogToFile(b,c,"response.log");
#define ghiLogPost(b,c)	    ghiLogToFile(b,c,"post.log");
#else
#define ghiLogRequest(b,c)
#define ghiLogResponse(b,c)
#define ghiLogPost(b,c)
#endif


// Check a socket's state without blocking.
///////////////////////////////////////////
int ghiSocketSelect
(
	SOCKET socket,
	int * readFlag,
	int * writeFlag,
	int * exceptFlag
);

// Possible results from ghiDoReceive.
//////////////////////////////////////
typedef enum
{
	GHIRecvData,         // Data was received.
	GHINoData,           // No data was available.
	GHIConnClosed,       // The connection was closed.
	GHIError             // There was a socket error.
} GHIRecvResult;

// Receive some data.
/////////////////////
GHIRecvResult ghiDoReceive
(
	GHIConnection * connection,
	char buffer[],
	int * bufferLen
);

// Do a send on the connection's socket.
// Returns number of bytes sent (0 or more).
// If error, returns  (-1).
////////////////////////////////////////////
int ghiDoSend
(
	GHIConnection * connection,
	const char * buffer,
	int len
);

// Results for ghtTrySendThenBuffer.
////////////////////////////////////
typedef enum
{
	GHITrySendError,     // There was an error sending.
	GHITrySendSent,      // Everything was sent.
	GHITrySendBuffered   // Some or all of the data was buffered.
} GHITrySendResult;

// Sends whatever it can on the socket.
// Buffers whatever can't be sent in the sendBuffer.
////////////////////////////////////////////////////
GHITrySendResult ghiTrySendThenBuffer
(
	GHIConnection * connection,
	const char * buffer,
	int len
);

// Set the proxy server
////////////////////////
GHTTPBool ghiSetProxy
(
	const char * server
);

// Set the proxy server for a specific request
////////////////////////

// Set the throttle settings.
/////////////////////////////
void ghiThrottleSettings
(
	int bufferSize,
	gsi_time timeDelay
);



#ifdef __cplusplus
}
#endif

#endif
