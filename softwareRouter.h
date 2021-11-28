
// softwareRouter.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "Resource.h"		// main symbols
#include "softwareRouterDlg.h"
#include "ArpTable.h"


// CsoftwareRouterApp:
// See softwareRouter.cpp for the implementation of this class
//

class CsoftwareRouterApp : public CWinApp
{
public:
	CsoftwareRouterApp();

// Overrides
public:
	virtual BOOL InitInstance();
	Interface* getInterface(int);
	CsoftwareRouterDlg* getSoftwareRouterDialog();
	RoutingTable* getRoutingTable();
	ArpTable* getArpTable();
	static UINT routingProcess(void* pParam);
	void startThreads(void);
	int compareMac(macAddressStructure& mac1, macAddressStructure& mac2);
	int isBroadcast(macAddressStructure& address);
	NatTable* getNatTable(void);

// Implementation

	DECLARE_MESSAGE_MAP()

private:
	Interface* interface1;
	Interface* interface2;
	RoutingTable* routingTable;
	ArpTable* arpTable;
	NatTable* natTable;
};

extern CsoftwareRouterApp theApp;
