
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
	RoutingTable* routingTab = theApp.getRoutingTable();
	ArpTable* arpTab = theApp.getArpTable();
	int retval = 0;
	macAddressStructure sourceMac, destinationMac, localMac = inInterface->getMacAddressStruct();
	ipAddressStructure destinationIp, nextHop, * nextHopPointer;

	while (TRUE)
	{
		
		buffer->getFrame();
		sourceMac = buffer->getSourceMacAddress();
		destinationMac = buffer->getDestinationMacAddress();

		if (!theApp.ignoreFrame(buffer, localMac))
		{
			//ARP
			if (buffer->getLayer3Type() == 0x0806)
			{
				if (buffer->isReplyArp()) arpTab->proccessArpReply(buffer, inInterface);
				if (buffer->isRequestArp()) arpTab->replyToArpRequest(buffer, inInterface);
				continue;
			}
			// decrease the TTL value of packet by 1
			buffer->decreaseTTL();
			if (buffer->getTTL() == 0)
			{
				// traceroute
				TTLex.generateTtlExceeded(buffer, inInterface->getIpAddressStruct(), inInterface->generateIpHeaderId());
				inInterface->sendFrame(&TTLex, NULL, FALSE);
				TTLex.clear();
				continue;
			}

			destinationIp = buffer->getDestinationIpAddress();
			if ((theApp.getInterface(1)->isIpLocal(destinationIp)) || (theApp.getInterface(2)->isIpLocal(destinationIp)))
			{
				// ECHO reply
				if ((buffer->isIcmpEchoRequest()) && (buffer->isIcmpChecksumValid()))
				{
					buffer->generateIcmpEchoReply(destinationIp);
					inInterface->sendFrame(buffer, NULL, FALSE);
				}
				continue;
			}

			// NAT
			routingTab->criticalSectionTable.Lock();
			nextHopPointer = NULL;
			outInterface = routingTab->doLookup(destinationIp, &nextHopPointer);
			if (nextHopPointer) {
				nextHop = *nextHopPointer;
				nextHopPointer = &nextHop;
			}
			routingTab->criticalSectionTable.Unlock();
			if (!outInterface) continue;
			buffer->fillIpChecksum();
			retval = outInterface->sendFrame(buffer, nextHopPointer);
			if (retval != 0) break;
		}
		else 
		{
			continue;
		}
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

BOOL CsoftwareRouterApp::ignoreFrame(Frame* buffer, macAddressStructure localMac)
{
	// if source MAC address is the local MAC address, the frame is ignored
	if (theApp.compareMac(buffer->getSourceMacAddress(), localMac) == 0) return TRUE;
	// if source and destination MAC addresses are the same, the frame is ignored
	if (theApp.compareMac(buffer->getSourceMacAddress(), buffer->getDestinationMacAddress()) == 0) return TRUE;
	// if source MAC address is broadcast address, the frame is ignored
	if (theApp.isBroadcast(buffer->getSourceMacAddress())) return TRUE;
	// if destination MAC address is not the local or broadcast MAC address
	if ((theApp.compareMac(buffer->getDestinationMacAddress(), localMac) == 1) && (theApp.isBroadcast(buffer->getDestinationMacAddress()) == 0)) return TRUE;
	// if not include IP packet, the frame is ignored
	if (buffer->getLayer3Type() != 0x0800) return TRUE;
	// if the IP header checksum is not valid, the packet is ignored
	if (!buffer->validateChecksum()) return TRUE;
}
