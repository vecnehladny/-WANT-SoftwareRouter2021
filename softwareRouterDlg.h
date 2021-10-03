
// softwareRouterDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxbutton.h"
#include "Interface.h"
#include "RoutingTableListCtrl.h"
#include "ProtocolStorage.h"
#include "RoutingTable.h"
#include "AddStaticRouteDlg.h"

#define WM_SETIP_MESSAGE WM_APP+100
#define WM_ADDROUTE_MESSAGE WM_APP+101
#define WM_REMOVEROUTE_MESSAGE WM_APP+102


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

	CRoutingTableListCtrl routingTableBox;

	void autoResizeCols(CListCtrl* control);
	void initInterfacesInfos();
	void initRoutingTable();

public:
	void setIpAddr(Interface* i, ipAddressStructure newIpAddressStruct);
	void enableInterface(Interface* i, CMFCButton* enableButton);
	void disableInterface(Interface* i, CMFCButton* disableButton);
	static UINT editIpAddrThread(void* pParam);
	static UINT editRouteThread(void* pParam);
	void addRoute(int index, routeStructure& r);
	void removeRoute(int index);

public:
	afx_msg void onInt1SetIpButtonClicked();
	afx_msg void onInt2SetIpButtonClicked();
	afx_msg void onInt1EnableButtonClicked();
	afx_msg void onInt2EnableButtonClicked();
	afx_msg void onAddStaticRouteButtonClicked();
	afx_msg void onRemoveStaticRouteButtonClicked();

protected:
	afx_msg LRESULT onSetIpMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onAddRouteMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onRemoveRouteMessage(WPARAM wParam, LPARAM lParam);
};
