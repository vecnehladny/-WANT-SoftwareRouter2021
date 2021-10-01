#include "pch.h"
#include "SetIntDlg.h"
#include "afxdialogex.h"
#include "softwareRouter.h"
#include<iostream>

using namespace std;

PIP_ADAPTER_ADDRESSES CSetIntDlg::adaptersInfos = NULL;

IMPLEMENT_DYNAMIC(CSetIntDlg, CDialog)

CSetIntDlg::CSetIntDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetIntDlg::IDD, pParent)
{

}

CSetIntDlg::~CSetIntDlg() 
{

}

void CSetIntDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INTERFACE1COMBO, interface1Box);
	DDX_Control(pDX, IDC_INTERFACE2COMBO, interface2Box);
}


BEGIN_MESSAGE_MAP(CSetIntDlg, CDialog)
ON_BN_CLICKED(IDOK, &CSetIntDlg::selectInterfaces)
END_MESSAGE_MAP()

BOOL CSetIntDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	PIP_ADAPTER_ADDRESSES currentAdapter;
	CString validationResult = ProtocolStorage::getInstance()->validateFiles();

	if (validationResult.IsEmpty() == FALSE) {
		AfxMessageBox(validationResult, MB_ICONERROR);
		OnCancel();
	}

	if (getAdaptersList() != NO_ERROR) {
		MessageBox(
			CString("Searching for interfaces ends with and error"),
			_TEXT("Error"),
			MB_ICONERROR);
		OnCancel();
	}

	currentAdapter = adaptersInfos;

	while (currentAdapter) {
		if (currentAdapter->PhysicalAddressLength == 6) {
			interfaceNames.Add(currentAdapter->AdapterName);
			if (currentAdapter->AdapterName) {
				interfaceDescriptions.Add(currentAdapter->Description);
				addStringsToBothInterfaceBoxes(CString(currentAdapter->Description));
			}
			else {
				interfaceDescriptions.Add(currentAdapter->Description);
				addStringsToBothInterfaceBoxes(CString(currentAdapter->Description));
			}
			interfaceMacAddresses.Add(currentAdapter->PhysicalAddress);
		}
		currentAdapter = currentAdapter->Next;
	}

	if (interfaceNames.GetCount() > 2) {
		interface1Box.SetCurSel(0);
		interface2Box.SetCurSel(1);
		return TRUE;
	}
	else {
		MessageBox(
			_TEXT("You need at least 2 functional interfaces"),
			_TEXT("Error"),
			MB_ICONERROR);
		OnCancel();
	}

	
	return FALSE;
}

void CSetIntDlg::addStringsToBothInterfaceBoxes(CString stringToAdd) {
	interface1Box.AddString(stringToAdd);
	interface2Box.AddString(stringToAdd);
}

DWORD CSetIntDlg::getAdaptersList(void) {
	ULONG maxLength = 10000;
	DWORD toReturn = NO_ERROR;

	ULONG flags = GAA_FLAG_SKIP_UNICAST 
		| GAA_FLAG_SKIP_ANYCAST 
		| GAA_FLAG_SKIP_MULTICAST 
		| GAA_FLAG_SKIP_DNS_SERVER;

	if (!adaptersInfos) {
		adaptersInfos = (IP_ADAPTER_ADDRESSES*)malloc(maxLength);

		toReturn = GetAdaptersAddresses(AF_INET, flags, NULL, adaptersInfos, &maxLength);

		if (toReturn == ERROR_BUFFER_OVERFLOW) {
			free(adaptersInfos);
			adaptersInfos = NULL;
		}
	}

	return toReturn;
}

void CSetIntDlg::selectInterfaces() {
	CStringA adapterRpcapAddress;

	if (interface1Box.GetCurSel() == interface2Box.GetCurSel())
	{
		MessageBox(
			_TEXT("You need to select different interfaces"),
			_TEXT("Warning"), 
			MB_ICONWARNING);
		return;
	}

	theApp.getInterface(1)->setName(getAdapterRpcapAddressFromInterface(1));
	theApp.getInterface(1)->setDescription(interfaceDescriptions[interface1Box.GetCurSel()]);
	theApp.getInterface(1)->setMacAddress(interfaceMacAddresses[interface1Box.GetCurSel()]);

	theApp.getInterface(2)->setName(getAdapterRpcapAddressFromInterface(2));
	theApp.getInterface(2)->setDescription(interfaceDescriptions[interface2Box.GetCurSel()]);
	theApp.getInterface(2)->setMacAddress(interfaceMacAddresses[interface2Box.GetCurSel()]);

	clearAllAdapterInfos();
	
	CDialog::OnOK();
}

void CSetIntDlg::clearAllAdapterInfos(void) {
	interfaceNames.RemoveAll();
	interfaceDescriptions.RemoveAll();
	interfaceMacAddresses.RemoveAll();
	free(adaptersInfos);
	adaptersInfos = NULL;
}

CStringA CSetIntDlg::getAdapterRpcapAddressFromInterface(int interfaceNumber) {
	CStringA rpcapAddress;

	if (interfaceNumber == 1) {
		rpcapAddress.Format(
			"rpcap://\\Device\\NPF_%s",
			interfaceNames[interface1Box.GetCurSel()]);
		return rpcapAddress;
	}

	if (interfaceNumber == 2) {
		rpcapAddress.Format(
			"rpcap://\\Device\\NPF_%s",
			interfaceNames[interface1Box.GetCurSel()]);
		return rpcapAddress;
	}

	return NULL;
}