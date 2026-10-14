/*
GameSpy GHTTP SDK 
Dan "Mr. Pants" Schoenblum
dan@gamespy.com

Copyright 1999-2007 GameSpy Industries, Inc

devsupport@gamespy.com
*/

#include "ghttpCommon.h"

// Disable compiler warnings for issues that are unavoidable.
/////////////////////////////////////////////////////////////
#if defined(_MSC_VER) // DevStudio
// Level4, "conditional expression is constant". 
// Occurs with use of the MS provided macro FD_SET
#pragma warning ( disable: 4127 )
#endif // _MSC_VER

#ifdef WIN32
// A lock.
//////////
typedef void * GLock;

// The lock used by ghttp.
//////////////////////////
static GLock ghiGlobalLock;
#endif

// Proxy server.
////////////////
char * ghiProxyAddress;
unsigned short ghiProxyPort;

// Throttle settings.
/////////////////////
int ghiThrottleBufferSize = 125;
gsi_time ghiThrottleTimeDelay = 250;

// Number of connections
/////////////////////
extern int ghiNumConnections;


#ifdef WIN32
// Creates a lock.
//////////////////
static GLock GNewLock(void)
{
	CRITICAL_SECTION * criticalSection;

	criticalSection = (CRITICAL_SECTION *)gsimalloc(sizeof(CRITICAL_SECTION));
	if(!criticalSection)
		return NULL;

	InitializeCriticalSection(criticalSection);

	return (GLock)criticalSection;
}

// Frees a lock.
////////////////
static void GFreeLock(GLock lock)
{
	CRITICAL_SECTION * criticalSection = (CRITICAL_SECTION *)lock;

	if(!lock)
		return;

	DeleteCriticalSection(criticalSection);

	gsifree(criticalSection);
}

// Locks a lock.
////////////////
static void GLockLock(GLock lock)
{
	CRITICAL_SECTION * criticalSection = (CRITICAL_SECTION *)lock;

	if(!lock)
		return;

	EnterCriticalSection(criticalSection);
}

// Unlocks a lock.
//////////////////
static void GUnlockLock(GLock lock)
{
	CRITICAL_SECTION * criticalSection = (CRITICAL_SECTION *)lock;

	if(!lock)
		return;

	LeaveCriticalSection(criticalSection);
}
#endif

// Creates the ghttp lock.
//////////////////////////
void ghiCreateLock(void)
{
#ifdef WIN32
	// We shouldn't already have a lock.
	////////////////////////////////////
	assert(!ghiGlobalLock);

	// Create the lock.
	///////////////////
	ghiGlobalLock = GNewLock();
#endif
}

// Frees the ghttp lock.
////////////////////////
void ghiFreeLock(void)
{
#ifdef WIN32
	if(!ghiGlobalLock)
		return;

	GFreeLock(ghiGlobalLock);
	ghiGlobalLock = NULL;
#endif
}

// Locks the ghttp lock.
////////////////////////
void ghiLock
(
	void
)
{
#ifdef WIN32
	if(!ghiGlobalLock)
		return;

	GLockLock(ghiGlobalLock);
#endif
}

// Unlocks the ghttp lock.
//////////////////////////
void ghiUnlock
(
	void
)
{
#ifdef WIN32
	if(!ghiGlobalLock)
		return;

	GUnlockLock(ghiGlobalLock);
#endif
}

// Logs traffic.
////////////////
#ifdef HTTP_LOG
void ghiLogToFile(const char * buffer, int len, const char* fileName)
{
#ifdef _NITRO
	int i;

	if(!buffer || !len)
		return;

	for(i = 0 ; i < len ; i++)
		OS_PutChar(buffer[i]);
#else
	FILE * file;

	if(!buffer || !len)
		return;

	file = fopen(fileName, "ab");
	if(file)
	{
		fwrite(buffer, 1, len, file);
		fclose(file);
	}
#endif
}
#endif

// Check a socket's state without blocking (2003: ghttp's own; the 2007 SDK
// made it the common code's GSISocketSelect).
//////////////////////////////////////////////////////////////////////////
int ghiSocketSelect
(
	SOCKET socket,
	int * readFlag,
	int * writeFlag,
	int * exceptFlag
)
{
	fd_set readSet;
	fd_set writeSet;
	fd_set exceptSet;
	fd_set * readFds = NULL;
	fd_set * writeFds = NULL;
	fd_set * exceptFds = NULL;
	struct timeval timeout = { 0, 0 };
	int rcode;

	assert(socket != INVALID_SOCKET);

	// Setup the parameters.
	////////////////////////
	if(readFlag != NULL)
	{
		FD_ZERO(&readSet);
		FD_SET(socket, &readSet);
		readFds = &readSet;
	}
	if(writeFlag != NULL)
	{
		FD_ZERO(&writeSet);
		FD_SET(socket, &writeSet);
		writeFds = &writeSet;
	}
	if(exceptFlag != NULL)
	{
		FD_ZERO(&exceptSet);
		FD_SET(socket, &exceptSet);
		exceptFds = &exceptSet;
	}

	// Do the select.
	/////////////////
	rcode = select(FD_SETSIZE, readFds, writeFds, exceptFds, &timeout);
	if(gsiSocketIsError(rcode))
		return -1;

	// Check results.
	/////////////////
	if(readFlag != NULL)
		*readFlag = ((rcode > 0) && FD_ISSET(socket, readFds));
	if(writeFlag != NULL)
		*writeFlag = ((rcode > 0) && FD_ISSET(socket, writeFds));
	if(exceptFlag != NULL)
		*exceptFlag = ((rcode > 0) && FD_ISSET(socket, exceptFds));

	return rcode;
}

// Receive some data.
/////////////////////
GHIRecvResult ghiDoReceive
(
	GHIConnection * connection,
	char buffer[],
	int * bufferLen
)
{
	int rcode;
	int socketError;
	int len;

	// How much to try and receive.
	///////////////////////////////
	len = (*bufferLen - 1);

	// Are we throttled?
	////////////////////
	if(connection->throttle)
	{
		unsigned long now;

		// Don't receive too often.
		///////////////////////////
		now = current_time();
		if(now < (connection->lastThrottleRecv + ghiThrottleTimeDelay))
			return GHINoData;

		// Update the receive time.
		///////////////////////////
		connection->lastThrottleRecv = (unsigned int)now;

		// Don't receive too much.
		//////////////////////////
		len = min(len, ghiThrottleBufferSize);
	}

	// Receive some data.
	/////////////////////
	rcode = recv(connection->socket, buffer, len, 0);	

	// There was an error.
	//////////////////////
	if(gsiSocketIsError(rcode))
	{
		// Get the error code.
		//////////////////////
		socketError = GOAGetLastError(connection->socket);

		// Check for a closed connection.
		/////////////////////////////////
		if(socketError == WSAENOTCONN)
		{
			connection->connectionClosed = GHTTPTrue;
			return GHIConnClosed;
		}

		// Check for nothing waiting.
		/////////////////////////////
		if((socketError == WSAEWOULDBLOCK) || (socketError == WSAEINPROGRESS) || (socketError == WSAETIMEDOUT))
			return GHINoData;

		// There was a real error.
		//////////////////////////
		connection->completed = GHTTPTrue;
		connection->result = GHTTPSocketFailed;
		connection->socketError = socketError;
		connection->connectionClosed = GHTTPTrue;

		return GHIError;
	}

	// The connection was closed.
	/////////////////////////////
	if(rcode == 0)
	{
		connection->connectionClosed = GHTTPTrue;
		return GHIConnClosed;
	}

	// Cap the buffer.
	//////////////////
	buffer[rcode] = '\0';
	*bufferLen = rcode;

	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_Network, GSIDebugLevel_RawDump, "Received %d bytes\n", rcode);

	// Notify app.
	//////////////
	return GHIRecvData;
}

int ghiDoSend
(
	struct GHIConnection * connection,
	const char * buffer,
	int len
)
{
	int rcode;

	if (buffer == NULL || len == 0)
		return 0;
	
	// Do the send.
	///////////////
	rcode = send(connection->socket, buffer, len, 0);
	// Check for an error.
	//////////////////////
	if(gsiSocketIsError(rcode))
	{
		int error;

		// Would block just means 0 bytes sent.
		///////////////////////////////////////
		error = GOAGetLastError(connection->socket);
		if((error == WSAEWOULDBLOCK) || (error == WSAEINPROGRESS) || (error == WSAETIMEDOUT))
			return 0;

		connection->completed = GHTTPTrue;
		connection->result = GHTTPSocketFailed;
		connection->socketError = error;

		return -1;
	}

	if(connection->state == GHTTPPosting)
	{
		connection->postingState.bytesPosted += rcode;
		ghiLogRequest(buffer, rcode);
	}

	return rcode;
}

GHITrySendResult ghiTrySendThenBuffer
(
	GHIConnection * connection,
	const char * buffer,
	int len
)
{
	int rcode = 0;

	// If we already have something buffered, don't send.
	/////////////////////////////////////////////////////
	if(connection->sendBuffer.pos >= connection->sendBuffer.len)
	{
		// Try and send.
		////////////////
		rcode = ghiDoSend(connection, buffer, len);
		if(gsiSocketIsError(rcode))
			return GHITrySendError;

		// Was it all sent?
		///////////////////
		if(rcode == len)
			return GHITrySendSent;
	}
	
	// Buffer whatever wasn't sent.
	///////////////////////////////
	if(!ghiAppendDataToBuffer(&connection->sendBuffer, buffer + rcode, len - rcode))
		return GHITrySendError;
	return GHITrySendBuffered;
}

static GHTTPBool ghiParseProxyServer
(
	const char * server,
	char ** proxyAddress,       // [out] the proxy address
	unsigned short * proxyPort  // [out] the proxy port
)
{
	char * strPort;

	// Make sure each pointer is valid as well as what it points to
	assert(server && *server);
	assert(proxyAddress && !*proxyAddress);
	assert(proxyPort);

	// Copy off the server address.
	///////////////////////////////
	*proxyAddress = goastrdup(server);
	if(!*proxyAddress)
		return GHTTPFalse;

	// Check for a port.
	////////////////////
	if((strPort = strchr(*proxyAddress, ':')) != NULL)
	{
		*strPort++ = '\0';

		// Try getting the port.
		////////////////////////
		*proxyPort = (unsigned short)atoi(strPort);
		if(!*proxyPort)
		{
			gsifree(*proxyAddress);
			*proxyAddress = NULL;
			return GHTTPFalse;
		}
	}
	else
	{
		*proxyPort = GHI_DEFAULT_PORT;
	}
	return GHTTPTrue;
}

GHTTPBool ghiSetProxy
(
	const char * server
)
{
	// Free any existing proxy address.
	///////////////////////////////////
	if(ghiProxyAddress)
	{
		gsifree(ghiProxyAddress);
		ghiProxyAddress = NULL;
	}
	ghiProxyPort = 0;

	// If a server was supplied, try to parse it
	if(server && *server)
		return ghiParseProxyServer(server, &ghiProxyAddress, &ghiProxyPort);

	// No server supplied results in proxy being cleared
	return GHTTPTrue;
}

void ghiThrottleSettings
(
	int bufferSize,
	gsi_time timeDelay
)
{
	ghiThrottleBufferSize = bufferSize;
	ghiThrottleTimeDelay = timeDelay;
}

// Re-enable previously disabled compiler warnings
///////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning ( default: 4127 )
#endif // _MSC_VER

