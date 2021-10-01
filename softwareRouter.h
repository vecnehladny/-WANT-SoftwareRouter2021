
// softwareRouter.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "softwareRouterDlg.h"


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
	BOOL statsEnabled;
	Interface* getInterface(int);
	CsoftwareRouterDlg* getSoftwareRouterDialog();
	RoutingTable* getRoutingTable();

// Implementation

	DECLARE_MESSAGE_MAP()

private:
	Interface* interface1;
	Interface* interface2;
	RoutingTable* routingTable;
};

extern CsoftwareRouterApp theApp;
