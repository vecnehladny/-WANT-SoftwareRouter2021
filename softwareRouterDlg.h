
// softwareRouterDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxbutton.h"
#include "Interface.h"
#include "ProtocolStorage.h"
#include "RoutingTable.h"

#define WM_SETIP_MESSAGE WM_APP+100


// CsoftwareRouterDlg dialog
class CsoftwareRouterDlg : public CDialogEx
{
// Construction
public:
	CsoftwareRouterDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SOFTWAREROUTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CStatic int1DeviceNameText;	
	CStatic int1MacAddressText;
	CStatic int1IpAddressText;
	CMFCButton int1SetIpButton;
	CMFCButton int1EnableButton;

	CStatic int2IpAddressText;
	CStatic int2MacAddressText;
	CStatic int2DeviceNameText;
	CMFCButton int2SetIpButton;
	CMFCButton int2EnableButton;

	void initInterfacesInfos();

public:
	void setIpAddr(Interface* i, ipAddressStructure newIpAddressStruct);
	void enableInterface(Interface* i, CMFCButton* enableButton);
	void disableInterface(Interface* i, CMFCButton* disableButton);
	static UINT editIpAddrThread(void* pParam);

public:
	afx_msg void onInt1SetIpButtonClicked();
	afx_msg void onInt2SetIpButtonClicked();
	afx_msg void onInt1EnableButtonClicked();
	afx_msg void onInt2EnableButtonClicked();

protected:
	afx_msg LRESULT onSetIpMessage(WPARAM wParam, LPARAM lParam);
};
