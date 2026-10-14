// build/gamespytest.sh: a GameSpy.dll's exports exercised offline (CD keys, the
// server's properties, the browser's requests), printing every result.
#include <windows.h>
#include <stdio.h>
#define GAMESPY_EXPORTS   // declare dllexport: we call through GetProcAddress anyway
#include "IGameSpy.h"

typedef bool (*IsCDKeyValidFn)(const char*, int);
typedef IGameSpyServer* (*CreateServerFn)(IGameSpyServer::StartupInfo const&);
typedef void (*DeleteServerFn)(IGameSpyServer*);
typedef IGameSpyBrowser* (*CreateBrowserFn)(IGameSpyBrowser::StartupInfo const&);
typedef void (*DeleteBrowserFn)(IGameSpyBrowser*);

int main(int argc, char** argv)
{
	HMODULE h = LoadLibraryA(argv[1]);
	if(!h) { printf("load failed %lu\n", GetLastError()); return 1; }
	IsCDKeyValidFn pValid = (IsCDKeyValidFn)GetProcAddress(h, "IsCDKeyValid");
	const char* keys[] = { "", "ABCD-EFGH-IJKL-MNOP-2345", "BABA-BABA-BABA-BABA-2222", "DUC2-FYR3-PAS4-TEM5-6789",
		"bab2bab2bab2bab22222", "ZYZ9-ZYZ9-ZYZ9-ZYZ9-9999", "CEG3 JUL4 NAP5 RYS6 7892" };
	for(int t = 1; t <= 4; t++)
		for(int i = 0; i < sizeof(keys)/sizeof(keys[0]); i++)
			printf("IsCDKeyValid(%s,%d)=%d\n", keys[i], t, pValid(keys[i], t));
	// random keys: count the valid ones and print a few
	int n = 0; char k[32];
	srand(1234);
	for(int r = 0; r < 40000; r++) {
		const char* C = "BCDFGJLMNPRSTWXZ"; const char* V = "AEUY";
		char* p = k;
		for(int g = 0; g < 4; g++) { *p++ = C[rand()%16]; *p++ = V[rand()%4]; *p++ = C[rand()%16]; *p++ = '2' + rand()%8; *p++ = '-'; }
		for(int d = 0; d < 4; d++) *p++ = '2' + rand()%8;
		*p = 0;
		if(pValid(k, 1)) { n++; if(n < 4) printf("valid: %s\n", k); }
	}
	printf("valid count %d\n", n);

	CreateServerFn pCreate = (CreateServerFn)GetProcAddress(h, "?Create@IGameSpyServer@@SAPAV1@ABUStartupInfo@1@@Z");
	DeleteServerFn pDelete = (DeleteServerFn)GetProcAddress(h, "?Delete@IGameSpyServer@@SAXPAV1@@Z");
	IGameSpyServer::StartupInfo si;
	si.m_nGameType = 1; si.m_sVersion = "1.1"; si.m_nPort = 27888;
	IGameSpyServer* s = pCreate(si);
	printf("server %p status %d\n", s, s->GetPublishStatus());
	printf("reg %d %d %d\n", s->RegisterKey("hostname"), s->RegisterKey("score_"), s->RegisterKey("mykey"));
	printf("set %d\n", s->SetServerProperty("hostname", "Test Server"));
	char buf[64];
	printf("get %d '%s'\n", s->GetServerProperty("HOSTNAME", buf, sizeof(buf)), buf);
	printf("get %d '%s'\n", s->GetServerProperty("gamever", buf, sizeof(buf)), buf);
	unsigned int len = 64;
	printf("chal %d len %u\n", s->GetCDKeyChallenge(buf, &len), len);
	printf("chalresp %d\n", s->GetChallengeResponse(3));
	s->SetNumPlayers(5);
	printf("get %d '%s'\n", s->GetServerProperty("numplayers", buf, sizeof(buf)), buf);
	unsigned char ip[4] = {127,0,0,1};
	printf("net %d %d\n", s->HandleNetMessage((const unsigned char*)"hello", 5, ip, 1000), s->HandleNetMessage((const unsigned char*)"\xfd\xfc\x1e\x66\x6a\xb2", 6, ip, 1000));
	pDelete(s);

	CreateBrowserFn pCreateB = (CreateBrowserFn)GetProcAddress(h, "?Create@IGameSpyBrowser@@SAPAV1@ABUStartupInfo@1@@Z");
	DeleteBrowserFn pDeleteB = (DeleteBrowserFn)GetProcAddress(h, "?Delete@IGameSpyBrowser@@SAXPAV1@@Z");
	IGameSpyBrowser::StartupInfo bi; bi.m_nGameType = 1;
	IGameSpyBrowser* b = pCreateB(bi);
	printf("browser %p status %d\n", b, b->GetBrowserStatus());
	printf("regb %d %d\n", b->RegisterKey("hostname"), b->AddSummaryKey("hostname"));
	unsigned int rl = 0;
	printf("resp %d len %u\n", b->GetCDKeyChallengeResponse("ABCD", "efgh", NULL, &rl), rl);
	printf("halt %d status %d\n", b->HaltRequest(), b->GetBrowserStatus());
	pDeleteB(b);
	printf("done\n");
	return 0;
}
