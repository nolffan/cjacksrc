// ----------------------------------------------------------------------- //
//
// MODULE  : Ping.h
//
// PURPOSE : An ICMP echo ("ping") of a host, for the server browser.
//
// cjacksrc: reconstructed from the retail GameSpy.dll (Ping.cpp).
//
// ----------------------------------------------------------------------- //

#ifndef __PING_H__
#define __PING_H__

// The round trip to pszAddress (a name or a dotted address) in
// milliseconds, or 0xffff if it could not be measured (raw sockets need
// administrator rights on Windows 2000 and later).
unsigned short Ping( char const* pszAddress );

#endif // __PING_H__
