#include "nninternal.h"
#include "../darray.h"
#include <stddef.h>
#include <stdio.h>

unsigned char NNMagicData[] = {NN_MAGIC_0, NN_MAGIC_1, NN_MAGIC_2, NN_MAGIC_3, NN_MAGIC_4, NN_MAGIC_5};
struct _NATNegotiator
{
	SOCKET negotiateSock;
	SOCKET gameSock;
	int cookie;
	int clientindex;
	NegotiateState state;
	int initAckRecv[3];
	int retryCount;
	int maxRetryCount;
	unsigned long retryTime;
	unsigned int guessedIP;
	unsigned short guessedPort;
	unsigned char gotRemoteData;
	unsigned char sendGotRemoteData;
	NegotiateProgressFunc progressCallback;
	NegotiateCompletedFunc completedCallback;
	void *userdata;	
};

typedef struct _NATNegotiator *NATNegotiator;

DArray negotiateList = NULL;

char *Matchup1Hostname;
char *Matchup2Hostname;

unsigned int matchup1ip = 0;
unsigned int matchup2ip = 0;



static NATNegotiator FindNegotiatorForCookie(int cookie)
{
	int i;
	if (negotiateList == NULL)
		return NULL;
	for (i = 0 ; i < ArrayLength(negotiateList) ; i++)
	{
		//we go backwards in case we need to remove one..
		NATNegotiator neg = (NATNegotiator)ArrayNth(negotiateList, i);
		if (neg->cookie == cookie)
			return neg;
	}
	return NULL;
}

static NATNegotiator AddNegotiator()
{
	
	struct _NATNegotiator _neg;
	

	memset(&_neg, 0, sizeof(_neg));

	if (negotiateList == NULL)
		negotiateList = ArrayNew(sizeof(_neg), 4, NULL);

	ArrayAppend(negotiateList, &_neg);

	return (NATNegotiator)ArrayNth(negotiateList, ArrayLength(negotiateList) - 1);
}

static void RemoveNegotiator(NATNegotiator neg)
{
	int i;
	for (i = 0 ; i < ArrayLength(negotiateList) ; i++)
	{
		//we go backwards in case we need to remove one..
		if (neg == (NATNegotiator)ArrayNth(negotiateList, i))
		{
			ArrayRemoveAt(negotiateList, i);
			return;

		}
	}
}

void NNFreeNegotiateList()
{
	if (negotiateList != NULL)
	{
		ArrayFree(negotiateList);
		negotiateList = NULL;
	}
}

static int CheckMagic(char *data)
{
	return (memcmp(data, NNMagicData, NATNEG_MAGIC_LEN) == 0);
}

static void SendPacket(SOCKET sock, unsigned int toaddr, unsigned short toport, void *data, int len)
{
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(toport);
	saddr.sin_addr.s_addr = toaddr;
	sendto(sock, (char *)data, len, 0, (struct sockaddr *)&saddr, sizeof(saddr));
}

static unsigned int GetLocalIP()
{
	int num_local_ips;
	struct hostent *phost;
	struct in_addr *addr;
	unsigned int localip = 0;
	phost = getlocalhost();
	if (phost == NULL)
		return 0;
	for (num_local_ips = 0 ; ; num_local_ips++)
	{
		if (phost->h_addr_list[num_local_ips] == 0)
			break;
		addr = (struct in_addr *)phost->h_addr_list[num_local_ips];
		if (addr->s_addr == htonl(0x7F000001))
			continue;
		localip = addr->s_addr;

		if(IsPrivateIP(addr))
			return localip;
	}
	return localip; //else a specific private address wasn't found - return what we've got
}

static unsigned short GetLocalPort(SOCKET sock)
{
	int ret;
	struct sockaddr_in saddr;
	int saddrlen = sizeof(saddr);

	ret = getsockname(sock,(struct sockaddr *)&saddr, &saddrlen);

	if (gsiSocketIsError(ret))
		return 0;
	return saddr.sin_port;
}

static void SendInitPackets(NATNegotiator neg)
{
	char buffer[INITPACKET_SIZE];

	NatNegPacket * p = (NatNegPacket *)buffer;
	unsigned int localip;
	unsigned short localport;

	memcpy(p->magic, NNMagicData, NATNEG_MAGIC_LEN);
	p->version = NN_PROTVER;
	p->packettype = NN_INIT;
	p->cookie = (int)htonl((unsigned int)neg->cookie);
	p->Packet.Init.clientindex = (unsigned char)neg->clientindex;
	p->Packet.Init.usegameport = (unsigned char)((neg->gameSock == INVALID_SOCKET) ? 0 : 1);
	localip = ntohl(GetLocalIP());
	//ip
	buffer[INITPACKET_ADDRESS_OFFSET] = (char)((localip >> 24) & 0xFF);
	buffer[INITPACKET_ADDRESS_OFFSET+1] = (char)((localip >> 16) & 0xFF);
	buffer[INITPACKET_ADDRESS_OFFSET+2] = (char)((localip >> 8) & 0xFF);
	buffer[INITPACKET_ADDRESS_OFFSET+3] = (char)(localip & 0xFF);
	//port (this may not be determined until the first packet goes out)
	buffer[INITPACKET_ADDRESS_OFFSET+4] = 0;
	buffer[INITPACKET_ADDRESS_OFFSET+5] = 0;
	if (p->Packet.Init.usegameport && !neg->initAckRecv[NN_PT_GP])
	{
		p->Packet.Init.porttype = NN_PT_GP;

		gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
			"Sending INIT (GP) to %s:%d...\n", inet_ntoa(*(struct in_addr *)&matchup1ip), MATCHUP_PORT1);

		SendPacket(neg->gameSock, matchup1ip, MATCHUP_PORT1, p, INITPACKET_SIZE);		
	}

	if (!neg->initAckRecv[NN_PT_NN1])
	{
		p->Packet.Init.porttype = NN_PT_NN1;
		gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
			"Sending INIT (NN1) to %s:%d...\n", inet_ntoa(*(struct in_addr *)&matchup1ip), MATCHUP_PORT1);

		SendPacket(neg->negotiateSock, matchup1ip, MATCHUP_PORT1, p, INITPACKET_SIZE);
	}	

	//this should be determined now...
	localport = ntohs(GetLocalPort((p->Packet.Init.usegameport) ?  neg->gameSock : neg->negotiateSock));
	buffer[INITPACKET_ADDRESS_OFFSET+4] = (char)((localport >> 8) & 0xFF);
	buffer[INITPACKET_ADDRESS_OFFSET+5] = (char)(localport & 0xFF);

	if (!neg->initAckRecv[NN_PT_NN2])
	{
		p->Packet.Init.porttype = NN_PT_NN2;
		gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
			"Sending INIT (NN2) to %s:%d...\n", inet_ntoa(*(struct in_addr *)&matchup2ip), MATCHUP_PORT2);

		SendPacket(neg->negotiateSock, matchup2ip, MATCHUP_PORT2, p, INITPACKET_SIZE);
	}

	neg->retryTime = current_time() + INIT_RETRY_TIME;
	neg->maxRetryCount = INIT_RETRY_COUNT;
}

static void SendPingPacket(NATNegotiator neg)
{
	NatNegPacket p;

	memcpy(p.magic, NNMagicData, NATNEG_MAGIC_LEN);
	p.version = NN_PROTVER;
	p.packettype = NN_CONNECT_PING;
	p.cookie = (int)htonl((unsigned int)neg->cookie);
	p.Packet.Connect.remoteIP = neg->guessedIP;
	p.Packet.Connect.remotePort = htons(neg->guessedPort);
	p.Packet.Connect.gotyourdata = neg->gotRemoteData;
	p.Packet.Connect.finished = (unsigned char)((neg->state == ns_connectping) ? 0 : 1);

//////////////
// playing with a way to re-sync with the NAT's port mappings in the case the guess is off:
//if(neg->retryCount >= 3 && neg->retryCount % 3 == 0) neg->guessedPort++;
//////////////

	gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
		"Sending PING to %s:%d (got remote data: %d)\n", inet_ntoa(*(struct in_addr *)&neg->guessedIP), neg->guessedPort,   neg->gotRemoteData);
	SendPacket((neg->gameSock != INVALID_SOCKET) ? neg->gameSock : neg->negotiateSock, neg->guessedIP, neg->guessedPort,  &p, CONNECTPACKET_SIZE);
	neg->retryTime = current_time() + PING_RETRY_TIME;
	neg->maxRetryCount = PING_RETRY_COUNT;
	if(neg->gotRemoteData)
		neg->sendGotRemoteData = 1;
}

NegotiateError NNBeginNegotiation(int cookie, int clientindex, NegotiateProgressFunc progresscallback, NegotiateCompletedFunc completedcallback, void *userdata)
{
	return NNBeginNegotiationWithSocket(INVALID_SOCKET, cookie, clientindex, progresscallback, completedcallback, userdata);
}

static unsigned int NameToIp(const char *name)
{
	unsigned int ret;
	struct hostent *hent;

	ret = inet_addr(name);
	
	if (ret == INADDR_NONE)
	{
		hent = gethostbyname(name);
		if (!hent)
			return 0;
		ret = *(unsigned int *)hent->h_addr_list[0];
	}
	return ret;
}

NegotiateError NNBeginNegotiationWithSocket(SOCKET gamesocket, int cookie, int clientindex, NegotiateProgressFunc progresscallback, NegotiateCompletedFunc completedcallback, void *userdata)
{
	NATNegotiator neg;

	if (matchup1ip == 0)
	{
		if (Matchup1Hostname == NULL)
			Matchup1Hostname = MATCHUP1_HOSTNAME;
		matchup1ip = NameToIp(Matchup1Hostname);
	}
	if (matchup2ip == 0)
	{
		if (Matchup2Hostname == NULL)
			Matchup2Hostname = MATCHUP2_HOSTNAME;
		matchup2ip = NameToIp(Matchup2Hostname);
	}
	if (matchup1ip == 0 || matchup2ip == 0)
		return ne_dnserror;
	
	neg = AddNegotiator();
	if (neg == NULL)
		return ne_allocerror;
	neg->gameSock = gamesocket;
	neg->clientindex = clientindex;
	neg->cookie = cookie;
	neg->progressCallback = progresscallback;
	neg->completedCallback = completedcallback;
	neg->userdata = userdata;
	neg->negotiateSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	neg->retryCount = 0;
	neg->gotRemoteData = 0;
	neg->sendGotRemoteData = 0;
	neg->guessedIP = 0;
	neg->guessedPort = 0;
	neg->maxRetryCount = 0;
	if (neg->negotiateSock == INVALID_SOCKET)
	{
		RemoveNegotiator(neg);
		return ne_socketerror;
	}
	SendInitPackets(neg);

	return ne_noerror;
}

void NNCancel(int cookie)
{
	NATNegotiator neg = FindNegotiatorForCookie(cookie);
	if (neg == NULL)
		return;
	if (neg->negotiateSock != INVALID_SOCKET)
		closesocket(neg->negotiateSock);
	neg->negotiateSock = INVALID_SOCKET;
	neg->state = ns_canceled;
}

static void NegotiateThink(NATNegotiator neg)
{
	//check for any incoming data
	static char indata[NNINBUF_LEN]; //256 byte input buffer
	struct sockaddr_in saddr;
	int saddrlen = sizeof(struct sockaddr_in);
	int error;

	if (neg->state == ns_canceled) //we need to remove it
	{
		gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Memory, GSIDebugLevel_Notice,
			"Removing canceled negotiator\n");
		RemoveNegotiator(neg);
		return;
	}

	if (neg->negotiateSock != INVALID_SOCKET)
	{
		//first, socket processing
		while (CanReceiveOnSocket(neg->negotiateSock))
		{
			error = recvfrom(neg->negotiateSock, indata, NNINBUF_LEN, 0, (struct sockaddr *)&saddr, &saddrlen);

			if (gsiSocketIsError(error))
			{
				gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
					"RECV SOCKET ERROR: %d\n", GOAGetLastError(neg->negotiateSock));
				break;
			}

			NNProcessData(indata, error, &saddr);
			if (neg->state == ns_canceled)
				break;

			if (neg->negotiateSock == INVALID_SOCKET)
				break;
		}
	}

	if (neg->state == ns_initsent || neg->state == ns_connectping) //see if we need to resend init packets
	{
		if (current_time() > neg->retryTime)
		{
			if (neg->retryCount > neg->maxRetryCount)
			{
				gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
					"RETRY FAILED...\n");
				neg->completedCallback(nr_inittimeout, INVALID_SOCKET, NULL, neg->userdata);
				NNCancel(neg->cookie); //mark to-be-canceled
			} else
			{
				
				neg->retryCount++;
				if (neg->state == ns_initsent) //resend init packets
					SendInitPackets(neg);
				else
					SendPingPacket(neg); //resend ping packet
				gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
					"[retry]\n");
			}

		}
	}

	if (neg->state == ns_finished && current_time() > neg->retryTime) //check if it is ready to be removed
	{
		struct sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(neg->guessedPort);
		saddr.sin_addr.s_addr = neg->guessedIP;
		if (neg->gameSock == INVALID_SOCKET)
		{
			neg->completedCallback(nr_success, neg->negotiateSock, &saddr, neg->userdata);
			neg->negotiateSock = INVALID_SOCKET; //no gameSock, so don't let NNCancel close this socket
		}
		else
			neg->completedCallback(nr_success, neg->gameSock, &saddr, neg->userdata);
		NNCancel(neg->cookie); //mark to-be-canceled
	}

	if (neg->state == ns_initack && current_time() > neg->retryTime) //see if the partner has timed out
	{
		neg->completedCallback(nr_deadbeatpartner, INVALID_SOCKET, NULL, neg->userdata);
		NNCancel(neg->cookie); //mark to-be-canceled
	}

}

void NNThink()
{
	int i;

	if (negotiateList == NULL)
		return;

	for(i = ArrayLength(negotiateList) - 1 ; i >= 0 ; i--)
	{
		//we go backwards in case we need to remove one..
		NegotiateThink((NATNegotiator)ArrayNth(negotiateList, i));
	}
}

static void SendConnectAck(NATNegotiator neg, struct sockaddr_in *toaddr)
{
	NatNegPacket p;

	memcpy(p.magic, NNMagicData, NATNEG_MAGIC_LEN);
	p.version = NN_PROTVER;
	p.packettype = NN_CONNECT_ACK;
	p.cookie = (int)htonl((unsigned int)neg->cookie);
	p.Packet.Init.clientindex = (unsigned char)neg->clientindex;

	gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
		"Sending connect ack...\n");
	SendPacket(neg->negotiateSock, toaddr->sin_addr.s_addr, ntohs(toaddr->sin_port), &p, INITPACKET_SIZE);
}

static void ProcessConnectPacket(NATNegotiator neg, NatNegPacket *p, struct sockaddr_in *fromaddr)
{
	gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
		"Got connect packet (finish code: %d), guess: %s:%d\n", p->Packet.Connect.finished, inet_ntoa(*(struct in_addr *)&p->Packet.Connect.remoteIP), ntohs(p->Packet.Connect.remotePort));
	//send an ack..
	if (p->Packet.Connect.finished == FINISHED_NOERROR) //don't need to ack any errors
		SendConnectAck(neg, fromaddr);

	
	if (neg->state >= ns_connectping)
		return; //don't process it any further

	if (p->Packet.Connect.finished != FINISHED_NOERROR) //call the completed callback with the error code
	{
		NegotiateResult errcode;
		errcode = nr_unknownerror; //default if unknown
		if (p->Packet.Connect.finished == FINISHED_ERROR_DEADBEAT_PARTNER)
			errcode = nr_deadbeatpartner;
		else if (p->Packet.Connect.finished == FINISHED_ERROR_INIT_PACKETS_TIMEDOUT)
			errcode = nr_inittimeout;
		neg->completedCallback(errcode, INVALID_SOCKET, NULL, neg->userdata);
		NNCancel(neg->cookie); //mark to-be-canceled
		return;
	}

	neg->guessedIP = p->Packet.Connect.remoteIP;
	neg->guessedPort = ntohs(p->Packet.Connect.remotePort);
	neg->retryCount = 0;

	neg->state = ns_connectping;
	neg->progressCallback(neg->state, neg->userdata);

	SendPingPacket(neg);	
}

static void ProcessPingPacket(NATNegotiator neg, NatNegPacket *p, struct sockaddr_in *fromaddr)
{
	if (neg->state < ns_connectping)
		return;

	//update our guessed ip and port
	gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
		"Got ping from: %s:%d (gotmydata: %d, finished: %d)\n", inet_ntoa(fromaddr->sin_addr), ntohs(fromaddr->sin_port), p->Packet.Connect.gotyourdata, p->Packet.Connect.finished);

	neg->guessedIP = fromaddr->sin_addr.s_addr;
	neg->guessedPort = ntohs(fromaddr->sin_port);
	neg->gotRemoteData = 1;

	if (!p->Packet.Connect.gotyourdata) //send another packet until they have our data
		SendPingPacket(neg);
	else //they have our data, and we have their data - it's a connection!
	{
		if (neg->state == ns_connectping) //advance it
		{
			gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
				"CONNECT FINISHED\n");

			if(!neg->sendGotRemoteData)
				SendPingPacket(neg);

			//we need to leave it around for a while to process any incoming data.
			neg->state = ns_finished;
			neg->retryTime = current_time() + FINISHED_IDLE_TIME;


		} else if (!p->Packet.Connect.finished)
			SendPingPacket(neg);
	}
}

static void ProcessInitPacket(NATNegotiator neg, NatNegPacket *p, struct sockaddr_in *fromaddr)
{
	switch (p->packettype)
	{
	case NN_INITACK:
		//mark our init as ack'd
		if (p->Packet.Init.porttype > NN_PT_NN2)
			return; //invalid port
		gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
			"Got init ack for port %d\n", p->Packet.Init.porttype);
		neg->initAckRecv[p->Packet.Init.porttype] = 1;
		if (neg->state == ns_initsent) //see if we can advance to negack
		{
			if (neg->initAckRecv[NN_PT_NN1] != 0 && neg->initAckRecv[NN_PT_NN2] != 0 &&
				(neg->gameSock == INVALID_SOCKET ||  neg->initAckRecv[NN_PT_GP] != 0))
			{
				neg->state = ns_initack;
				neg->retryTime = current_time() + PARTNER_WAIT_TIME;
				neg->progressCallback(neg->state, neg->userdata);
			}
		}
		break;

	case NN_ERTTEST:
		//we just send the packet back where it came from..
		gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
			"Got ERT\n");
		p->packettype = NN_ERTACK;
		SendPacket(neg->negotiateSock, fromaddr->sin_addr.s_addr, ntohs(fromaddr->sin_port), p, INITPACKET_SIZE);
		break;

	}
}

void NNProcessData(char *data, int len, struct sockaddr_in *fromaddr)
{
	NatNegPacket p;
	NATNegotiator neg;
	unsigned char ptype;

	if (!CheckMagic(data))
		return; //invalid packet

	ptype = *(unsigned char *)(data + offsetof(NatNegPacket, packettype));

	gsDebugFormat(GSIDebugCat_NatNeg, GSIDebugType_Network, GSIDebugLevel_Notice,
		"Process data, packet type: %d, %d bytes (%s:%d)\n", ptype, len, inet_ntoa(fromaddr->sin_addr), ntohs(fromaddr->sin_port));
	
	if (ptype == NN_CONNECT || ptype == NN_CONNECT_PING)
	{ //it's a connect packet
		if (len < CONNECTPACKET_SIZE)
			return;
		memcpy(&p, data, CONNECTPACKET_SIZE);		
		neg = FindNegotiatorForCookie((int)ntohl((unsigned int)p.cookie));
		if (neg)
		{
			if (ptype == NN_CONNECT)
				ProcessConnectPacket(neg, &p, fromaddr);
			else
				ProcessPingPacket(neg, &p, fromaddr);
		}
			

	} else //it's an init packet
	{
		if (len < INITPACKET_SIZE)
			return;
		memcpy(&p, data, INITPACKET_SIZE);		
		neg = FindNegotiatorForCookie((int)ntohl((unsigned int)p.cookie));
		if (neg)
			ProcessInitPacket(neg, &p, fromaddr);
	}
}
