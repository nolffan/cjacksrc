// build/gamespytest.sh: a server published from one GameSpy.dll found on the LAN
// by a browser from another (a copy, so both load), then its details asked for.
#include <windows.h>
#include <stdio.h>
#define GAMESPY_EXPORTS
#include "IGameSpy.h"

typedef IGameSpyServer* (*CreateServerFn)(IGameSpyServer::StartupInfo const&);
typedef void (*DeleteServerFn)(IGameSpyServer*);
typedef IGameSpyBrowser* (*CreateBrowserFn)(IGameSpyBrowser::StartupInfo const&);
typedef void (*DeleteBrowserFn)(IGameSpyBrowser*);

static int g_nFound = 0;
static void ServerInfo(IGameSpyBrowser& b, IGameSpyBrowser::HGAMESERVER h, void* p)
{
	char name[64] = "", ver[64] = "", np[16] = "";
	b.GetServerProperty(h, "hostname", name, sizeof(name));
	b.GetServerProperty(h, "gamever", ver, sizeof(ver));
	b.GetServerProperty(h, "numplayers", np, sizeof(np));
	printf("found: '%s' ver '%s' players '%s' (%d)\n", name, ver, np, b.GetNumPlayers(h));
	g_nFound++;
}

int main(int argc, char** argv)
{
	HMODULE hs = LoadLibraryA(argv[1]);
	char path[MAX_PATH]; strcpy(path, argv[2]);
	// a second copy so both can be loaded at once
	CopyFileA(argv[2], "gsb.dll", FALSE);
	HMODULE hb = LoadLibraryA("gsb.dll");
	if(!hs || !hb) { printf("load failed\n"); return 1; }
	CreateServerFn pCreate = (CreateServerFn)GetProcAddress(hs, "?Create@IGameSpyServer@@SAPAV1@ABUStartupInfo@1@@Z");
	DeleteServerFn pDelete = (DeleteServerFn)GetProcAddress(hs, "?Delete@IGameSpyServer@@SAXPAV1@@Z");
	CreateBrowserFn pCreateB = (CreateBrowserFn)GetProcAddress(hb, "?Create@IGameSpyBrowser@@SAPAV1@ABUStartupInfo@1@@Z");
	DeleteBrowserFn pDeleteB = (DeleteBrowserFn)GetProcAddress(hb, "?Delete@IGameSpyBrowser@@SAXPAV1@@Z");

	IGameSpyServer::StartupInfo si;
	si.m_nGameType = 1; si.m_sVersion = "1.1"; si.m_nPort = 27888; si.m_bPublic = false; si.m_sIP = "";
	IGameSpyServer* s = pCreate(si);
	s->RegisterKey("hostname");
	s->SetServerProperty("hostname", "LAN Test");
	s->SetNumPlayers(3);
	printf("publish %d status %d\n", s->Publish(), s->GetPublishStatus());

	IGameSpyBrowser::StartupInfo bi; bi.m_nGameType = 1;
	IGameSpyBrowser* b = pCreateB(bi);
	b->RegisterKey("hostname");
	printf("request %d\n", b->RequestServerList(false, 27888, NULL, ServerInfo, NULL));
	for(int i = 0; i < 300 && b->GetBrowserStatus() == IGameSpyBrowser::eBrowserStatus_Processing; i++) Sleep(10);
	Sleep(500);
	printf("browser status %d found %d\n", b->GetBrowserStatus(), g_nFound);
	g_nFound = 0;
	printf("details %d\n", b->RequestServerDetails("127.0.0.1", 27889, true, ServerInfo, NULL));
	for(int j = 0; j < 100 && !g_nFound; j++) { Sleep(10); }
	printf("details status %d found %d\n", b->GetBrowserStatus(), g_nFound);
	pDeleteB(b);
	printf("unpublish %d\n", s->Unpublish());
	pDelete(s);
	return 0;
}
