#pragma once
#pragma comment(lib, "iphlpapi.lib")

#include <IPHlpApi.h>
#include <afxwin.h>
#include "resource.h"
#include <IPHlpApi.h>

class CSetIntDlg :
    public CDialog
{
    DECLARE_DYNAMIC(CSetIntDlg)

public:
    CSetIntDlg(CWnd* pParent = NULL);
    virtual ~CSetIntDlg();

    enum { IDD = IDD_SET_INT_DIALOG };

    afx_msg void selectInterfaces();

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

private :
    static PIP_ADAPTER_ADDRESSES adaptersInfos;
    CComboBox interface1Box;
    CComboBox interface2Box;
    CArray<PCHAR, PCHAR> interfaceNames;
    CArray<PWCHAR, PWCHAR> interfaceDescriptions;
    CArray<PBYTE, PBYTE> interfaceMacAddresses;
    void addStringsToBothInterfaceBoxes(CString);
    DWORD getAdaptersList(void);
    void clearAllAdapterInfos(void);
    CStringA getAdapterRpcapAddressFromInterface(int);
};

