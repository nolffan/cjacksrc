/*
GameSpy GHTTP SDK 
Dan "Mr. Pants" Schoenblum
dan@gamespy.com

Copyright 1999-2007 GameSpy Industries, Inc

devsupport@gamespy.com
*/

#include "ghttpProcess.h"
#include "ghttpCallbacks.h"
#include "ghttpPost.h"
#include "ghttpMain.h"
#include "ghttpCommon.h"

// Parse the URL into:
//   server address (and IP)
//   server port
//   request path.
/////////////////////////////
static GHTTPBool ghiParseURL
(
	GHIConnection * connection
)
{
	char * URL;
	int nIndex;
	char tempChar;
	char * str;

	assert(connection);
	if(!connection)
		return GHTTPFalse;

	// 2002.Apr.18.JED - Make sure we have an URL
	/////////////////////////////////////////////
	assert(connection->URL);
	if(!connection->URL)
		return GHTTPFalse;

	URL = connection->URL;

	// Check for "http://".
	//////////////////////
	if(strncmp(URL, "http://", 7) == 0)
	{
		connection->protocol = GHIHttp;
		URL += 7;
	}
	else if (strncmp(URL, "https://", 8) == 0)
	{
		connection->protocol = GHIHttps;
		URL += 8;
	}
	else
	{
		return GHTTPFalse;
	}

	// Read the address.
	////////////////////
	nIndex = (int)strcspn(URL, ":/");
	tempChar = URL[nIndex];
	URL[nIndex] = '\0';
	connection->serverAddress = goastrdup(URL);
	if(!connection->serverAddress)
		return GHTTPFalse;
	URL[nIndex] = tempChar;
	URL += nIndex;

	// Read the port.
	/////////////////
	if(*URL == ':')
	{
		URL++;
		connection->serverPort = (unsigned short)atoi(URL);
		if(!connection->serverPort)
			return GHTTPFalse;
		do
		{
			URL++;
		}while(*URL && (*URL != '/'));
	}
	else
	{
		if (connection->protocol == GHIHttps)
			connection->serverPort = GHI_DEFAULT_SECURE_PORT;
		else
			connection->serverPort = GHI_DEFAULT_PORT;
	}

	// Read the path.
	/////////////////
	if(!*URL)
		URL = "/";
	connection->requestPath = goastrdup(URL);
	while((str = strchr(connection->requestPath, ' ')) != NULL)
		*str = '+';
	if(!connection->requestPath)
		return GHTTPFalse;

	return GHTTPTrue;
}

/****************
** SOCKET INIT **
****************/
void ghiDoSocketInit
(
	GHIConnection * connection
)
{
	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_Comment, "Socket Initialization\n");

	// Progress.
	////////////
	ghiCallProgressCallback(connection, NULL, 0);

	// Init sockets.
	////////////////
	SocketStartUp();

	// Parse the URL.
	/////////////////
	if(!ghiParseURL(connection))
	{
		connection->completed = GHTTPTrue;
		connection->result = GHTTPParseURLFailed;
		return;
	}

	// Progress.
	////////////
	connection->state = GHTTPHostLookup;
	ghiCallProgressCallback(connection, NULL, 0);
}

/****************
** HOST LOOKUP **
****************/
void ghiDoHostLookup
(
	GHIConnection * connection
)
{
	HOSTENT * host = NULL;
	const char * server = NULL;

	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_Comment, "Host Lookup\n");


	// Check for using a proxy.
	///////////////////////////
	if(ghiProxyAddress)
		server = ghiProxyAddress;
	else
		server = connection->serverAddress;

	// Try resolving the address as an IP a.b.c.d number.
	/////////////////////////////////////////////////////
	connection->serverIP = inet_addr(server);
	if(connection->serverIP == INADDR_NONE)
	{
		// Try resolving with DNS.
		//////////////////////////
		host = gethostbyname(server);

		if(host == NULL)
		{
			gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_HotError, 
				"Host Lookup failed\n");
			connection->completed = GHTTPTrue;
			connection->result = GHTTPHostLookupFailed;
			return;
		}

		// Get the IP.
		//////////////
		connection->serverIP = *(unsigned int *)host->h_addr_list[0];
	}

	// Progress.
	////////////
	connection->state = GHTTPConnecting;
	ghiCallProgressCallback(connection, NULL, 0);
}
/***************
** CONNECTING **
***************/
void ghiDoConnecting
(
	GHIConnection * connection
)
{
	int rcode;
	SOCKADDR_IN address;
	int writeFlag;
	int exceptFlag;

	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_Comment, "Connecting\n");

	// If we don't have a socket yet, set it up.
	////////////////////////////////////////////
	if(connection->socket == INVALID_SOCKET)
	{
		// Create the socket.
		/////////////////////
		connection->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(connection->socket == INVALID_SOCKET)
		{
			connection->completed = GHTTPTrue;
			connection->result = GHTTPSocketFailed;
			connection->socketError = GOAGetLastError(connection->socket);
			return;
		}

		// Set the socket to non-blocking.
		//////////////////////////////////
		if(!SetSockBlocking(connection->socket, 0))
		{
			connection->completed = GHTTPTrue;
			connection->result = GHTTPSocketFailed;
			connection->socketError = GOAGetLastError(connection->socket);
			return;
		}

		// If throttling, use a small receive buffer.
		/////////////////////////////////////////////
		if(connection->throttle)
			SetReceiveBufferSize(connection->socket, ghiThrottleBufferSize);

		// Setup the server address.
		////////////////////////////
		memset(&address, 0, sizeof(SOCKADDR_IN));
		address.sin_family = AF_INET;
		if(ghiProxyAddress)
			address.sin_port = htons(ghiProxyPort);
		else
			address.sin_port = htons(connection->serverPort);
		address.sin_addr.s_addr = connection->serverIP;

		// Start the connect.
		/////////////////////
		rcode = connect(connection->socket, (SOCKADDR *)&address, sizeof(address));
		if(gsiSocketIsError(rcode))
		{
			int socketError = GOAGetLastError(connection->socket);
			if((socketError != WSAEWOULDBLOCK) && (socketError != WSAEINPROGRESS) && (socketError != WSAETIMEDOUT))
			{
				connection->completed = GHTTPTrue;
				connection->result = GHTTPConnectFailed;
				connection->socketError = socketError;
				return;
			}
		}
	}

	// Check if the connect has completed.
	//////////////////////////////////////
	rcode = ghiSocketSelect(connection->socket, NULL, &writeFlag, &exceptFlag);
	if((gsiSocketIsError(rcode)) || ((rcode == 1) && exceptFlag))
	{
		connection->completed = GHTTPTrue;
		connection->result = GHTTPConnectFailed;
		if(gsiSocketIsError(rcode))
			connection->socketError = GOAGetLastError(connection->socket);
		else
			connection->socketError = 0;
		return;
	}

	// Check if we're connected.
	////////////////////////////
	if((rcode == 1) && writeFlag)
	{
		// Progress.
		////////////
		connection->state = GHTTPSendingRequest;
		ghiCallProgressCallback(connection, NULL, 0);
	}
} 

/********************
** SENDING REQUEST **
********************/
void ghiDoSendingRequest
(
	GHIConnection * connection
)
{
	char * requestType;
	int oldPos;

	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_Comment, "Sending Request\n");
	
	// If we haven't filled the send buffer yet, do that first.
	///////////////////////////////////////////////////////////
	if(!connection->sendBuffer.len)
	{
		GHIBuffer* writeBuffer = &connection->sendBuffer;
		// Fill in the request line.
		////////////////////////////
		if(connection->post)
			requestType = "POST ";
		else if(connection->type == GHIHEAD)
			requestType = "HEAD ";
		else
			requestType = "GET ";
		ghiAppendDataToBuffer(writeBuffer, requestType, 0);
		if(ghiProxyAddress)
			ghiAppendDataToBuffer(writeBuffer, connection->URL, 0);
		else
			ghiAppendDataToBuffer(writeBuffer, connection->requestPath, 0);
		ghiAppendDataToBuffer(writeBuffer, " HTTP/1.1" CRLF, 0);

		// Add the host header.
 		///////////////////////
		if(connection->serverPort == GHI_DEFAULT_PORT)
		{
			ghiAppendHeaderToBuffer(writeBuffer, "Host", connection->serverAddress);
		}
		else
		{
			ghiAppendDataToBuffer(writeBuffer, "Host: ", 0);
			ghiAppendDataToBuffer(writeBuffer, connection->serverAddress, 0);
			ghiAppendCharToBuffer(writeBuffer, ':');
			ghiAppendIntToBuffer(writeBuffer, connection->serverPort);
			ghiAppendDataToBuffer(writeBuffer, CRLF, 2);
		}

		// Add the user-agent header.
		/////////////////////////////
		ghiAppendHeaderToBuffer(writeBuffer, "User-Agent", "GameSpyHTTP/1.0");
		
		// Add the connection header.
		/////////////////////////////
		ghiAppendHeaderToBuffer(writeBuffer, "Connection", "close");

		// Post needs extra headers.
		////////////////////////////
		if(connection->post)
		{
			char buf[16];

			// Add the content-length header.
			/////////////////////////////////
			sprintf(buf, "%d", connection->postingState.totalBytes);
			ghiAppendHeaderToBuffer(writeBuffer, "Content-Length", buf);

			// Add the content-type header.
			///////////////////////////////
			ghiAppendHeaderToBuffer(writeBuffer, "Content-Type", ghiPostGetContentType(connection));
		}

		// Not supported by all servers
		//ghiAppendHeaderToBuffer(writeBuffer, "Expect", "100-continue");

		// Add user-headers.
		////////////////////
		if(connection->sendHeaders)
			ghiAppendDataToBuffer(writeBuffer, connection->sendHeaders, 0);

		// Add the blank line to finish it off.
		///////////////////////////////////////
		ghiAppendDataToBuffer(writeBuffer, CRLF, 2);
	}

	// Store the old position.
	//////////////////////////
	oldPos = connection->sendBuffer.pos;

	// Send what we can.
	////////////////////
	if(!ghiSendBufferedData(connection))
		return;
	
	// Log anything we sent.
	////////////////////////
	#ifdef HTTP_LOG
	if(connection->sendBuffer.pos != oldPos)
		ghiLogRequest(connection->sendBuffer.data + oldPos, connection->sendBuffer.pos - oldPos);
	#endif

	// Check for data still buffered.
	/////////////////////////////////
	if(connection->sendBuffer.pos < connection->sendBuffer.len)
		return;

	// Clear the send buffer.
	/////////////////////////
	ghiResetBuffer(&connection->sendBuffer);

	// Finished sending.
	////////////////////
	if(connection->post)
		connection->state = GHTTPPosting;
	else
		connection->state = GHTTPWaiting;
	ghiCallProgressCallback(connection, NULL, 0);

	GSI_UNUSED(oldPos);
}

/************
** POSTING **
************/
void ghiDoPosting
(
	GHIConnection * connection
)
{
	GHIPostingResult result;
	int oldBytesPosted;

	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_Comment, "Posting\n");

	// Store the old bytes posted.
	//////////////////////////////
	oldBytesPosted = connection->postingState.bytesPosted;

	// Do some posting.
	///////////////////
	result = ghiPostDoPosting(connection);

	// Check for an error.
	//////////////////////
	if(result == GHIPostingError)
	{
		int rcode = 0;
		int readFlag = 0;

		// Make sure we already set the error stuff.
		////////////////////////////////////////////
		assert(connection->completed && connection->result);

		// Cleanup the posting state.
		/////////////////////////////
		ghiPostCleanupState(connection);

		// Is there a server response?
		rcode = ghiSocketSelect(connection->socket, &readFlag, NULL, NULL);
		if((rcode == 1) && readFlag)
		{
			// Ready to receive.
			////////////////////
			connection->state = GHTTPReceivingStatus;
			ghiCallProgressCallback(connection, NULL, 0);
		}
		return;
	}

	// Call the callback if we sent anything.
	/////////////////////////////////////////
	if(oldBytesPosted != connection->postingState.bytesPosted)
		ghiCallPostCallback(connection);

	// Check for done.
	//////////////////
	if(result == GHIPostingDone)
	{
		// Cleanup the posting state.
		/////////////////////////////
		ghiPostCleanupState(connection);
		// Set the new connection state.
		////////////////////////////////
		connection->state = GHTTPWaiting;
		ghiCallProgressCallback(connection, NULL, 0);

		return;
	}
}

/************
** WAITING **
************/
void ghiDoWaiting
(
	GHIConnection * connection
)
{
	int readFlag;
	int exceptFlag;
	int rcode;

	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_Comment, "Waiting\n");

	// We're waiting to receive something.
	//////////////////////////////////////
	rcode = ghiSocketSelect(connection->socket, &readFlag, NULL, &exceptFlag);
	if((gsiSocketIsError(rcode)) || ((rcode == 1) && exceptFlag))
	{
		connection->completed = GHTTPTrue;
		connection->result = GHTTPSocketFailed;
		if(gsiSocketIsError(rcode))
			connection->socketError = GOAGetLastError(connection->socket);
		else
			connection->socketError = 0;
		return;
	}

	// Check for waiting data.
	//////////////////////////
	if((rcode == 1) && readFlag)
	{
		// Ready to receive.
		////////////////////
		connection->state = GHTTPReceivingStatus;
		ghiCallProgressCallback(connection, NULL, 0);
	}
}

// Parse the status line.
/////////////////////////
static GHTTPBool ghiParseStatus
(
	GHIConnection * connection
)
{
	int majorVersion;
	int minorVersion;
	int statusCode;
	int statusStringIndex;
	int rcode;
	char c;

	GS_ASSERT(connection);
	GS_ASSERT(connection->recvBuffer.len > 0);

#if defined(_X360)
	//Xbox 360 needs "%n" to be manually enabled
	{
	int oldPrintCountValue = _set_printf_count_output(1);
#endif

	// Parse the string.
	////////////////////
	rcode = sscanf(connection->recvBuffer.data, "HTTP/%d.%d %d%n",
		&majorVersion,
		&minorVersion,
		&statusCode,
		&statusStringIndex);

#if defined(_X360)
	_set_printf_count_output(oldPrintCountValue);
	}
#endif

	// Check what we got.
	/////////////////////
	if((rcode != 3) ||     // Not all fields read.
		//!*statusString ||  // No status string.  PANTS|9.16.02 - apparently some servers don't return a status string
		(majorVersion < 1) ||  // Major version is less than 1.
		(statusCode < 100) ||        // 1xx is lowest status code.
		(statusCode >= 600))         // 5xx is highest status code.
	{
		connection->completed = GHTTPTrue;
		connection->result = GHTTPBadResponse;
		return GHTTPFalse;
	}

	// Figure out where the status string starts.
	/////////////////////////////////////////////
	while((c = connection->recvBuffer.data[statusStringIndex]) != '\0' && isspace(c))
		statusStringIndex++;

	// Set connection members.
	//////////////////////////
	connection->statusMajorVersion = majorVersion;
	connection->statusMinorVersion = minorVersion;
	connection->statusCode = statusCode;
	connection->statusStringIndex = statusStringIndex;

	return GHTTPTrue;
}

/*********************
** RECEIVING STATUS **
*********************/
void ghiDoReceivingStatus
(
	GHIConnection * connection
)
{
	char buffer[1024];
	int bufferLen;
	GHIRecvResult result;
	char * endOfStatus;

	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_Comment, "Receiving Status\n");

	// Get data.
	////////////
	bufferLen = sizeof(buffer);
	result = ghiDoReceive(connection, buffer, &bufferLen);

	// Handle error or no data.
	///////////////////////////
	if(result == GHIError)
		return;
	if(result == GHINoData)
		return;

	// Only append data if we got data.
	///////////////////////////////////
	if(result == GHIRecvData)
	{
		// Add the data to the buffer.
		//////////////////////////////
		if(!ghiAppendDataToBuffer(&connection->recvBuffer, buffer, bufferLen))
			return;
	}

	// Check if the status is finished.
	/////////////////////////////////////
	endOfStatus = strstr(connection->recvBuffer.data, CRLF);
	if(endOfStatus)
	{
		int statusLength;

		// Cap the status.
		//////////////////
		*endOfStatus = '\0';

		// Get the status length.
		/////////////////////////
		statusLength = (endOfStatus - connection->recvBuffer.data);

		// Log it.
		//////////
		ghiLogResponse(connection->recvBuffer.data, statusLength);
		ghiLogResponse("\n", 1);

		// Parse the status line.
		/////////////////////////
		if(!ghiParseStatus(connection))
			return;

		// Store the position of the start of the headers.
		//////////////////////////////////////////////////
		connection->headerStringIndex = (statusLength + 2);


		// We're receiving headers now.
		///////////////////////////////
		connection->state = GHTTPReceivingHeaders;
		ghiCallProgressCallback(connection, NULL, 0);
	}
	else if(result == GHIConnClosed)
	{
		// Connection closed.
		/////////////////////
		connection->completed = GHTTPTrue;
		connection->result = GHTTPBadResponse;
		connection->socketError = GOAGetLastError(connection->socket);
		return;
	}
}

// Delivers incoming file data to the appropriate place,
// then calls the progress callback.
// For GetFile, adds to buffer.
// For SaveFile, writes to disk.
// For StreamFile, does nothing.
// Returns false on error.
////////////////////////////////////////////////////////
static GHTTPBool ghiDeliverIncomingFileData
(
	GHIConnection * connection,
	char * data,
	int len
)
{
	char * buffer = NULL;
	int bufferLen = 0;

	// Add this to the total.
	/////////////////////////
	connection->fileBytesReceived += len;

	// Do we have the whole thing?
	//////////////////////////////
	if(connection->fileBytesReceived == connection->totalSize || connection->connectionClosed)
		connection->completed = GHTTPTrue;

	// Handle based on type.
	////////////////////////
	if(connection->type == GHIGET)
	{
		// Put this in the buffer.
		//////////////////////////
		if(!ghiAppendDataToBuffer(&connection->getFileBuffer, data, len))
			return GHTTPFalse;

		// Set the callback parameters.
		///////////////////////////////
		buffer = connection->getFileBuffer.data;
		bufferLen = connection->getFileBuffer.len;
	}
	else if(connection->type == GHISAVE)
	{
		int bytesWritten = 0;
#ifndef NOFILE
		bytesWritten = fwrite(data, 1, len, connection->saveFile);
#endif
		if(bytesWritten != len)
		{
			connection->completed = GHTTPTrue;
			connection->result = GHTTPFileWriteFailed;
			return GHTTPFalse;
		}

		// Set the callback parameters.
		///////////////////////////////
		buffer = data;
		bufferLen = len;
	}
	else if(connection->type == GHISTREAM)
	{
		// Set the callback parameters.
		///////////////////////////////
		buffer = data;
		bufferLen = len;
	}

	// Call the callback.
	/////////////////////
	ghiCallProgressCallback(connection, buffer, bufferLen);

	return GHTTPTrue;
}

// Gets the size of a chunk from a chunk header.
// Returns -1 on error.
////////////////////////////////////////////////
static int ghiParseChunkSize
(
	GHIConnection * connection
)
{
	char * header;
	int len;
	int num;
	int rcode;

	header = connection->chunkHeader;
	len = connection->chunkHeaderLen;

	assert(len);
	GSI_UNUSED(len);

	rcode = sscanf(header, "%x", &num);
	if(rcode != 1)
		return -1;

	return num;
}

// Appends the data to the chunk header buffer.
///////////////////////////////////////////////
static void ghiAppendToChunkHeaderBuffer
(
	GHIConnection * connection,
	char * data,
	int len
)
{
	assert(connection);
	assert(data);
	assert(len >= 0);

	// This can happen at the end of a header.
	//////////////////////////////////////////
	if(len == 0)
		return;

	// Is there room in the buffer?  If not, just
	// skip, we most likely already have the chunk size.
	////////////////////////////////////////////////////
	if(connection->chunkHeaderLen < CHUNK_HEADER_SIZE)
	{
		int numBytes;

		// How many bytes are we copying?
		/////////////////////////////////
		numBytes = min(CHUNK_HEADER_SIZE - connection->chunkHeaderLen, len);
		
		// Move the (possibly partial) header into the buffer.
		//////////////////////////////////////////////////////
		memcpy(connection->chunkHeader + connection->chunkHeaderLen, data, (unsigned int)numBytes);

		// Cap off the buffer.
		//////////////////////
		connection->chunkHeaderLen += numBytes;
		connection->chunkHeader[connection->chunkHeaderLen] = '\0';
	}
}

// Does any neccessary processing to incoming file data
// before it gets delivered.  This includes un-chunking.
// Returns false on error.
////////////////////////////////////////////////////////
static GHTTPBool ghiProcessIncomingFileData
(
	GHIConnection * connection,
	char * data,
	int len
)
{
	assert(connection);
	assert(data);
	assert(len > 0);

	// Is this a chunked transfer?
	//////////////////////////////
	if(connection->chunkedTransfer)
	{
		// Loop while there's stuff to process.
		///////////////////////////////////////
		while(len > 0)
		{
			// Reading a header?
			////////////////////
			if(connection->chunkReadingState == CRHeader)
			{
				char * endOfHeader;

				// Have we hit the LF (as in the CRLF ending the header)?
				/////////////////////////////////////////////////////////
				endOfHeader = strchr(data, 0xA);
				if(endOfHeader)
				{
					// Append what we have to the buffer.
					/////////////////////////////////////
					ghiAppendToChunkHeaderBuffer(connection, data, endOfHeader - data);

					// Adjust data and len.
					///////////////////////
					endOfHeader++;
					len -= (endOfHeader - data);
					data = endOfHeader;

					// Read the chunk size.
					///////////////////////
					connection->chunkBytesLeft = ghiParseChunkSize(connection);
					if(connection->chunkBytesLeft == -1)
					{
						// There was an error reading the chunk size.
						/////////////////////////////////////////////
						connection->completed = GHTTPTrue;
						connection->result = GHTTPBadResponse;
						return GHTTPFalse;
					}

					// Set the chunk reading state.
					///////////////////////////////
					if(connection->chunkBytesLeft == 0)
					{
						connection->chunkReadingState = CRFooter;
						gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_Network, GSIDebugLevel_RawDump,
							"Reading footer\n");
					}
					else
					{
						connection->chunkReadingState = CRChunk;
						gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_Network, GSIDebugLevel_RawDump,
							"Reading %d byte chunk\n", connection->chunkBytesLeft);
					}
				}
				else
				{
					// Move it all into the buffer.
					///////////////////////////////
					ghiAppendToChunkHeaderBuffer(connection, data, len);

					// Nothing else we can do now.
					//////////////////////////////
					return GHTTPTrue;
				}
			}
			// Reading a chunk?
			///////////////////
			else if(connection->chunkReadingState == CRChunk)
			{
				int numBytes;

				// How many bytes of data are we dealing with?
				//////////////////////////////////////////////
				numBytes = min(connection->chunkBytesLeft, len);

				gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_Network, GSIDebugLevel_RawDump,
					"Read %d bytes of chunk\n", numBytes);

				// Deliver the bytes.
				/////////////////////
				if(!ghiDeliverIncomingFileData(connection, data, numBytes))
					return GHTTPFalse;

				// Adjust data and len.
				///////////////////////
				data += numBytes;
				len -= numBytes;

				// Figure out how many bytes left in chunk.
				///////////////////////////////////////////
				connection->chunkBytesLeft -= numBytes;

				// Did we finish the chunk?
				///////////////////////////
				if(connection->chunkBytesLeft == 0)
					connection->chunkReadingState = CRCRLF;
			}
			// Reading a chunk footer (CRLF)?
			/////////////////////////////////
			else if(connection->chunkReadingState == CRCRLF)
			{
				char * endOfFooter;

				// Did we get an LF?
				////////////////////
				endOfFooter = strchr(data, 0xA);

				// The footer hasn't ended yet.
				///////////////////////////////
				if(!endOfFooter)
					return GHTTPTrue;

				// Adjust data and len.
				///////////////////////
				endOfFooter++;
				len -= (endOfFooter - data);
				data = endOfFooter;

				// Set up for reading the next header.
				//////////////////////////////////////
				connection->chunkHeader[0] = '\0';
				connection->chunkHeaderLen = 0;
				connection->chunkBytesLeft = 0;
				connection->chunkReadingState = CRHeader;

				gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_Network, GSIDebugLevel_RawDump,
					"Read chunk footer\n");
			}
			// Reading the footer?
			//////////////////////
			else if(connection->chunkReadingState == CRFooter)
			{
				// We're done.
				//////////////
				connection->completed = GHTTPTrue;

				gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_Network, GSIDebugLevel_RawDump,
					"Finished reading chunks\n");

				return GHTTPTrue;
			}
			// Bad state!
			/////////////
			else
			{
				assert(0);
				return GHTTPFalse;
			}
		}

		return GHTTPTrue;
	}

	// Regular transfer, just deliver it.
	/////////////////////////////////////
	return ghiDeliverIncomingFileData(connection, data, len);
}

/**********************
** RECEIVING HEADERS **
**********************/
void ghiDoReceivingHeaders
(
	GHIConnection * connection
)
{
	char buffer[4096];
	int bufferLen;
	GHIRecvResult result;
	GHTTPBool hasHeaders = GHTTPTrue;
	char * headers;
	char * endOfHeaders = NULL;

	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_Comment, "Receiving Headers\n");

	// Get data.
	////////////
	bufferLen = sizeof(buffer);
	result = ghiDoReceive(connection, buffer, &bufferLen);

	// Handle error, no data, conn closed.
	//////////////////////////////////////
	if(result == GHIError)
		return;
	if(result == GHINoData)
		return;

	// Only append data if we got data.
	///////////////////////////////////
	if(result == GHIRecvData)
	{
		// Add the data to the buffer.
		//////////////////////////////
		if(!ghiAppendDataToBuffer(&connection->recvBuffer, buffer, bufferLen))
			return;
	}

	// Cache a pointer to the front of the headers.
	///////////////////////////////////////////////
	headers = (connection->recvBuffer.data + connection->headerStringIndex);

	// Check if the headers are finished.
	/////////////////////////////////////
	if( ((connection->statusCode / 100) == 1) &&
		 (strncmp(headers, "\r\n", 2) == 0 || strncmp(headers, "\xA\xA", 2) == 0)
		 )
	{
		// If a continue doesn't have a header (immediate CRLF) move on to next status
		endOfHeaders = headers;
		hasHeaders = GHTTPFalse;
	}
	else
	{
		endOfHeaders = strstr(headers, CRLF CRLF);
	}
	if(!endOfHeaders)
	{
		endOfHeaders = strstr(headers, "\xA\xA"); // some servers seem to use LFs only?! Seen in 302 redirect. (28may01/bgw)
	}
	if(endOfHeaders)
	{
		char * fileStart;
		int fileLength;
		//int headersLength;
		char * contentLength;

#ifdef HTTP_LOG
		int headersLength;
#endif

		// Clear off the empty line.
		////////////////////////////
		if (GHTTPTrue == hasHeaders)
			endOfHeaders += 2;
		*endOfHeaders = '\0';

		// Figure out where the file starts, and how many bytes.
		////////////////////////////////////////////////////////
#ifdef HTTP_LOG
		headersLength = (endOfHeaders - headers);
#endif

		fileStart = (endOfHeaders + 2);
		fileLength = (connection->recvBuffer.len - (fileStart - connection->recvBuffer.data));

		// Set the headers buffer's new length.
		///////////////////////////////////////
		connection->recvBuffer.len = (endOfHeaders - connection->recvBuffer.data + 1);
		connection->recvBuffer.pos = connection->recvBuffer.len;

		// Log it.
		//////////
#ifdef HTTP_LOG
		ghiLogResponse(headers, headersLength);
		ghiLogResponse("\n", 1);
#endif

		// Check for continue.
		//////////////////////
		if((connection->statusCode / 100) == 1)
		{
			if(fileLength)
			{
				// Move any data to the front of the buffer.
				////////////////////////////////////////////
				memmove(connection->recvBuffer.data, fileStart, (unsigned int)fileLength + 1);
				connection->recvBuffer.len = fileLength;
			}
			else
			{
				// Reset the buffer.
				/////////////////////////
				ghiResetBuffer(&connection->recvBuffer);
			}

			// We're back to receiving status.
			//////////////////////////////////
			connection->state = GHTTPReceivingStatus;
			ghiCallProgressCallback(connection, NULL, 0);

			return;
		}

		// Check for redirection.
		/////////////////////////
		if((connection->statusCode / 100) == 3)
		{
			char * location;

			// Are we over our redirection count?
			/////////////////////////////////////
			if(connection->redirectCount > 10)
			{
				connection->completed = GHTTPTrue;
				connection->result = GHTTPFileNotFound;
				return;
			}

			// Find the new location.
			/////////////////////////
			location = strstr(headers, "Location:");
			if(location)
			{
				char * end;

				// Find the start of the URL.
				/////////////////////////////
				location += 9;
				while(isspace(*location))
					location++;

				// Find the end.
				////////////////
				for(end = location; *end && !isspace(*end) ; end++)  { };
				*end = '\0';

				// Check if this is not a full URL.
				///////////////////////////////////
				if(*location == '/')
				{
					int len;

					// Recompose the URL ourselves.
					///////////////////////////////
					len = (int)(strlen(connection->serverAddress) + 13 + strlen(location) + 1);
					connection->redirectURL = (char *)gsimalloc((unsigned int)len);
					if(!connection->redirectURL)
					{
						connection->completed = GHTTPTrue;
						connection->result = GHTTPOutOfMemory;
					}
					sprintf(connection->redirectURL, "http://%s:%d%s", connection->serverAddress, connection->serverPort, location);
				}
				else
				{
					// Set the redirect URL.
					////////////////////////
					connection->redirectURL = goastrdup(location);
					if(!connection->redirectURL)
					{
						connection->completed = GHTTPTrue;
						connection->result = GHTTPOutOfMemory;
					}
				}

				return;
			}
		}

		// If we know the file-length, set it.
		//////////////////////////////////////
		contentLength = strstr(headers, "Content-Length:");
		if(contentLength)
		{
			// Verify that the download size is something we can handle
			///////////////////////////////////////////////////////////
#if (GSI_MAX_INTEGRAL_BITS >= 64)
			char  szMaxSize[] = "9223372036854775807"; // == GSI_MAX_I64       
#else
			char  szMaxSize[] = "2147483647";          // == GSI_MAX_I32
#endif
			char* pStart  = contentLength+16;
			char* pEnd    = pStart;
			int   nMaxLen = (int)strlen(szMaxSize);

			// Skip to the end of the line
			while( pEnd && *pEnd != '\0' && *pEnd != '\n' && *pEnd != '\r' && *pEnd != ' ' )
				pEnd++;

 			if( pEnd-pStart > nMaxLen )
			{
				// Wow, that IS a big number
				connection->completed = GHTTPTrue;
				connection->result = GHTTPFileToBig;
				return;
			}
			else
			if( pEnd-pStart == nMaxLen )
			{
				// Same length, maybe a bigger number
				if( strncmp(pStart,szMaxSize,(unsigned int)(pEnd-pStart)) >= 0 )
				{
					connection->completed = GHTTPTrue;
					connection->result = GHTTPFileToBig;
					return;
				}
			}

			// Record the full size of the expected download
			////////////////////////////////////////////////
#if (GSI_MAX_INTEGRAL_BITS >= 64)
			connection->totalSize = _atoi64(pStart);
#else
			connection->totalSize = atoi(pStart);
#endif
		}

		// Check the chunky.
		////////////////////
		connection->chunkedTransfer = (strstr(headers, "Transfer-Encoding: chunked") != NULL)?GHTTPTrue:GHTTPFalse;
		if(connection->chunkedTransfer)
		{
			connection->chunkHeader[0] = '\0';
			connection->chunkHeaderLen = 0;
			connection->chunkBytesLeft = 0;
			connection->chunkReadingState = CRHeader;
		}

		// If we're just getting headers, or only posting data, we're done.
		///////////////////////////////////////////////////////////////////
		if((connection->type == GHIHEAD) || (connection->type == GHIPOST))
		{
			connection->completed = GHTTPTrue;
			return;
		}

		// We're receiving file data now.
		/////////////////////////////////
		connection->state = GHTTPReceivingFile;

		// Is this an empty file?
		/////////////////////////
		if(contentLength && !connection->totalSize)
		{
			connection->completed = GHTTPTrue;
			return;
		}

		// If any of the body has arrived, handle it.
		/////////////////////////////////////////////
		if(fileLength > 0)
			ghiProcessIncomingFileData(connection, fileStart, fileLength);

		// Don't reset the buffer -- we store status and header info
		//ghiResetBuffer(&connection->recvBuffer);
	}
	else if(result == GHIConnClosed)
	{
		// The conn was closed, and we didn't finish the headers - bad.
		///////////////////////////////////////////////////////////////
		connection->completed = GHTTPTrue;
		connection->result = GHTTPBadResponse;
		connection->socketError = GOAGetLastError(connection->socket);
	}
}

/*******************
** RECEIVING FILE **
*******************/
void ghiDoReceivingFile
(
	GHIConnection * connection
)
{
	char buffer[8192];
	int bufferLen;
	GHIRecvResult result;

	gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State, GSIDebugLevel_Comment, "Receiving File\n");

	// Get data.
	////////////
	bufferLen = sizeof(buffer);
	result = ghiDoReceive(connection, buffer, &bufferLen);

	// Handle error, no data, conn closed.
	//////////////////////////////////////
	if(result == GHIError)
		return;
	if(result == GHINoData)
		return;
	if(result == GHIConnClosed)
	{
		// The file is done (hopefully).
		////////////////////////////////
		connection->completed = GHTTPTrue;

		if (connection->totalSize > 0 && connection->fileBytesReceived < connection->totalSize)
			connection->result = GHTTPFileIncomplete;
		return;
	}

	// Process the data.
	////////////////////
	if(!ghiProcessIncomingFileData(connection, buffer, bufferLen))
		return;
}
