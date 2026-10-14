// ----------------------------------------------------------------------- //
//
// MODULE  : Ping.cpp
//
// PURPOSE : An ICMP echo ("ping") of a host, for the server browser.
//
// cjacksrc: reconstructed from the retail GameSpy.dll, 0x10006070..
// 0x1000638d (its assert names the file:
// C:\proj\to2\source\Game\Libs\GameSpy\Ping.cpp, line 225). It is
// Microsoft's raw-socket ping sample, cut down to one echo.
//
// ----------------------------------------------------------------------- //

#include <winsock.h>
#include <assert.h>
#include "Ping.h"

#define ICMP_ECHO		8
#define ICMP_ECHOREPLY	0

#define ICMP_MIN		8	// minimum 8 byte icmp packet (just header)

// The IP header
struct IpHeader
{
	unsigned int	h_len:4;		// length of the header
	unsigned int	version:4;		// Version of IP
	unsigned char	tos;			// Type of service
	unsigned short	total_len;		// total length of the packet
	unsigned short	ident;			// unique identifier
	unsigned short	frag_and_flags;	// flags
	unsigned char	ttl;
	unsigned char	proto;			// protocol (TCP, UDP etc)
	unsigned short	checksum;		// IP checksum
	unsigned int	sourceIP;
	unsigned int	destIP;
};

// ICMP header
struct IcmpHeader
{
	BYTE	i_type;
	BYTE	i_code;		// type sub code
	USHORT	i_cksum;
	USHORT	i_id;
	USHORT	i_seq;
	// This is not the std header, but we reserve space for time
	ULONG	timestamp;
};

#define DEF_PACKET_SIZE		32
#define MAX_PACKET			1024

#define PING_TIMEOUT		1000
#define PING_ERROR			0xffff

// RETAIL: 0x10006070
unsigned short DecodeResponse( char* buf, int bytes, sockaddr_in* from )
{
	IpHeader* iphdr = ( IpHeader* )buf;
	unsigned short iphdrlen = iphdr->h_len * 4; // number of 32-bit words *4 = bytes

	if( bytes < iphdrlen + ICMP_MIN )
	{
		// RETAIL: the assert is on line 225 of Monolith's Ping.cpp.
#line 225
		assert( false );
	}

	IcmpHeader* icmphdr = ( IcmpHeader* )( buf + iphdrlen );

	if( icmphdr->i_type != ICMP_ECHOREPLY )
		return PING_ERROR;

	if( icmphdr->i_id != ( USHORT )GetCurrentProcessId( ))
		return PING_ERROR;

	DWORD dwPing = GetTickCount( ) - icmphdr->timestamp;
	if( dwPing >= PING_ERROR )
		dwPing = PING_ERROR;

	return ( unsigned short )dwPing;
}

// RETAIL: 0x100060e0
USHORT Checksum( USHORT* buffer, int size )
{
	unsigned long cksum = 0;

	while( size > 1 )
	{
		cksum += *buffer++;
		size -= sizeof( USHORT );
	}

	if( size )
	{
		cksum += *( UCHAR* )buffer;
	}

	cksum = ( cksum >> 16 ) + ( cksum & 0xffff );
	cksum += ( cksum >> 16 );
	return ( USHORT )( ~cksum );
}

// RETAIL: 0x10006140
void FillICMPData( char* icmp_data, int datasize )
{
	IcmpHeader* icmp_hdr = ( IcmpHeader* )icmp_data;

	icmp_hdr->i_type = ICMP_ECHO;
	icmp_hdr->i_code = 0;
	icmp_hdr->i_id = ( USHORT )GetCurrentProcessId( );
	icmp_hdr->i_cksum = 0;
	icmp_hdr->i_seq = 0;

	char* datapart = icmp_data + sizeof( IcmpHeader );

	// Place some junk in the buffer.
	memset( datapart, 'E', datasize - sizeof( IcmpHeader ));
}

// RETAIL: 0x10006190
unsigned short Ping( char const* pszAddress )
{
	WSADATA wsaData;
	sockaddr_in dest, from;
	int fromlen = sizeof( from );
	int timeout = PING_TIMEOUT;
	char* icmp_data = NULL;
	char* recvbuf = NULL;
	unsigned short nPing = PING_ERROR;
	unsigned short seq_no = 0;

	if( WSAStartup( MAKEWORD( 2, 1 ), &wsaData ) != 0 )
		return PING_ERROR;

	SOCKET sockRaw = socket( AF_INET, SOCK_RAW, IPPROTO_ICMP );
	if( sockRaw != INVALID_SOCKET )
	{
		if( setsockopt( sockRaw, SOL_SOCKET, SO_RCVTIMEO, ( char* )&timeout, sizeof( timeout )) != SOCKET_ERROR )
		{
			timeout = PING_TIMEOUT;
			if( setsockopt( sockRaw, SOL_SOCKET, SO_SNDTIMEO, ( char* )&timeout, sizeof( timeout )) != SOCKET_ERROR )
			{
				memset( &dest, 0, sizeof( dest ));

				unsigned int addr = 0;
				hostent* hp = gethostbyname( pszAddress );
				if( !hp )
				{
					addr = inet_addr( pszAddress );
				}

				bool bResolved = ( hp || addr != INADDR_NONE );
				if( bResolved )
				{
					if( hp != NULL )
						memcpy( &( dest.sin_addr ), hp->h_addr, hp->h_length );
					else
						dest.sin_addr.s_addr = addr;
					if( hp )
						dest.sin_family = hp->h_addrtype;
					else
						dest.sin_family = AF_INET;
				}

				if( bResolved )
				{
					inet_ntoa( dest.sin_addr );

					icmp_data = ( char* )HeapAlloc( GetProcessHeap( ), HEAP_ZERO_MEMORY, MAX_PACKET );
					recvbuf = ( char* )HeapAlloc( GetProcessHeap( ), HEAP_ZERO_MEMORY, MAX_PACKET );
					if( icmp_data )
					{
						memset( icmp_data, 0, MAX_PACKET );
						int datasize = DEF_PACKET_SIZE + sizeof( IcmpHeader );
						FillICMPData( icmp_data, datasize );

						(( IcmpHeader* )icmp_data )->i_cksum = 0;
						(( IcmpHeader* )icmp_data )->timestamp = GetTickCount( );
						(( IcmpHeader* )icmp_data )->i_seq = seq_no++;
						(( IcmpHeader* )icmp_data )->i_cksum = Checksum(( USHORT* )icmp_data, datasize );

						if( sendto( sockRaw, icmp_data, datasize, 0, ( sockaddr* )&dest, sizeof( dest )) != SOCKET_ERROR )
						{
							int bread = recvfrom( sockRaw, recvbuf, MAX_PACKET, 0, ( sockaddr* )&from, &fromlen );
							if( bread != SOCKET_ERROR )
							{
								nPing = DecodeResponse( recvbuf, bread, &from );
							}
						}
					}
				}
			}
		}

		closesocket( sockRaw );
		if( icmp_data )
			HeapFree( GetProcessHeap( ), 0, icmp_data );
		if( recvbuf )
			HeapFree( GetProcessHeap( ), 0, recvbuf );
	}

	WSACleanup( );
	return nPing;
}
