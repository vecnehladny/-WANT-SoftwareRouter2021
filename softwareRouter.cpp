
// softwareRouter.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "softwareRouter.h"
#include "softwareRouterDlg.h"
#include "SetIntDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CsoftwareRouterApp

BEGIN_MESSAGE_MAP(CsoftwareRouterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CsoftwareRouterApp construction

CsoftwareRouterApp::CsoftwareRouterApp() :
	routingTable(NULL)
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CsoftwareRouterApp object

CsoftwareRouterApp theApp;


// CsoftwareRouterApp initialization

BOOL CsoftwareRouterApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	interface1 = new Interface(1);
	interface2 = new Interface(2);
	routingTable = new RoutingTable();
	arpTable = new ArpTable();
	natTable = new NatTable();

	CSetIntDlg setIntDlg;
	INT_PTR nResponse = setIntDlg.DoModal();
	if (nResponse == IDOK)
	{
		CsoftwareRouterDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

Interface* CsoftwareRouterApp::getInterface(int id){
	if (id == 1) {
		return this->interface1;
	}
	if (id == 2) {
		return this->interface2;
	}
	return NULL;
}


CsoftwareRouterDlg* CsoftwareRouterApp::getSoftwareRouterDialog()
{
	return (CsoftwareRouterDlg*)m_pMainWnd;
}

RoutingTable* CsoftwareRouterApp::getRoutingTable() 
{
	return routingTable;
}


ArpTable* CsoftwareRouterApp::getArpTable(void)
{
	return arpTable;
}


UINT CsoftwareRouterApp::routingProcess(void* pParam)
{
	Interface* inInterface = (Interface*)pParam;
	Interface* outInterface;
	Frame* buffer = inInterface->getFrames();
	Frame TTLex;
	RoutingTable* routingTable = theApp.getRoutingTable();
	ArpTable* arpTable = theApp.getArpTable();
	NatTable* natTable = theApp.getNatTable();
	int retval;
	macAddressStructure sourceMac, destinationMac, localMac = inInterface->getMacAddressStruct();
	ipAddressStructure destinationIp, nextHop, * nextHopPointer;

	while (TRUE)
	{
		buffer->getFrame();

		sourceMac = buffer->getSourceMacAddress();
		destinationMac = buffer->getDestinationMacAddress();

		if (theApp.compareMac(sourceMac, localMac) == 0) continue;
		if (theApp.compareMac(sourceMac, destinationMac) == 0) continue;
		if (theApp.isBroadcast(sourceMac)) continue;
		if ((theApp.compareMac(destinationMac, localMac) == 1)
			&& (theApp.isBroadcast(destinationMac) == 0)
			&& (!buffer->isMulticast())) continue;

		// ARP
		if (buffer->getLayer3Type() == 0x0806)
		{
			if (buffer->isReplyArp()) arpTable->proccessArpReply(buffer, inInterface);
			if (buffer->isRequestArp()) arpTable->replyToArpRequest(buffer, inInterface);
			continue;
		}

		if (buffer->getLayer3Type() != 0x0800) continue;
		if (!buffer->validateChecksum()) continue;

		//RIPv2
		retval = buffer->isRipMessage();
		//RIPv2 REQ
		if (retval == 1)
		{
			routingTable->processRipRequest(buffer, inInterface);
			continue;
		}

		//RIPv2 RESP
		if (retval == 2)
		{
			routingTable->processRipResponse(buffer, inInterface);
			continue;
		}

		buffer->decreaseTTL();
		if (buffer->getTTL() == 0)
		{
			//TRACERT
			TTLex.generateTtlExceeded(buffer, inInterface->getIpAddressStruct(), inInterface->generateIpHeaderId());
			inInterface->sendFrame(&TTLex, NULL, FALSE);
			TTLex.clear();
			continue;
		}

		//NAT
		if (natTable->translate(buffer, inInterface) != 0) continue;

		destinationIp = buffer->getDestinationIpAddress();
		if ((theApp.getInterface(1)->isIpLocal(destinationIp)) || (theApp.getInterface(2)->isIpLocal(destinationIp)))
		{
			//ICMP
			if ((buffer->isIcmpEchoRequest()) && (buffer->isIcmpChecksumValid()))
			{
				buffer->generateIcmpEchoReply(destinationIp);
				inInterface->sendFrame(buffer, NULL, FALSE);
			}
			continue;
		}

		destinationIp = buffer->getDestinationIpAddress();
		routingTable->criticalSectionTable.Lock();
		nextHopPointer = NULL;
		outInterface = routingTable->doLookup(destinationIp, &nextHopPointer);
		if (nextHopPointer) {
			nextHop = *nextHopPointer;
			nextHopPointer = &nextHop;
		}
		routingTable->criticalSectionTable.Unlock();
		if (!outInterface) continue;

		buffer->fillIpChecksum();

		retval = outInterface->sendFrame(buffer, nextHopPointer);
		if (retval != 0) break;
	}

	return 0;
}


void CsoftwareRouterApp::startThreads(void)
{
	AfxBeginThread(CsoftwareRouterApp::routingProcess, interface1);
	AfxBeginThread(CsoftwareRouterApp::routingProcess, interface2);
}


int CsoftwareRouterApp::compareMac(macAddressStructure& mac1, macAddressStructure& mac2)
{
	for (int i = 0; i < 6; i++) {
		if (mac1.section[i] != mac2.section[i]) {
			return 1;
		}
	}

	return 0;
}


int CsoftwareRouterApp::isBroadcast(macAddressStructure& address)
{
	for (int i = 0; i < 6; i++) {
		if (address.section[i] != 0xFF) {
			return 0;
		}
	}

	return 1;
}


NatTable* CsoftwareRouterApp::getNatTable()
{
	return natTable;
}
