
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
#include "ArpTable.h"
#include "NatTable.h"
#include "afxcmn.h"

#define WM_SETIP_MESSAGE WM_APP+100
#define WM_ADDROUTE_MESSAGE WM_APP+101
#define WM_REMOVEROUTE_MESSAGE WM_APP+102
#define WM_ADDARP_MESSAGE WM_APP+103
#define WM_REMOVEARP_MESSAGE WM_APP+104
#define WM_RIPUPDATE_MESSAGE WM_APP+105
#define WM_ADDNAT_MESSAGE WM_APP+106
#define WM_REMOVENAT_MESSAGE WM_APP+107
#define WM_UPDATENAT_MESSAGE WM_APP+108
#define WM_EDITPOOL_MESSAGE WM_APP+109



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
	CComboBox int1NatmodeCombo;

	CStatic int2IpAddressText;
	CStatic int2MacAddressText;
	CStatic int2DeviceNameText;
	CMFCButton int2SetIpButton;
	CMFCButton int2EnableButton;
	CComboBox int2NatmodeCombo;

	CRoutingTableListCtrl routingTableBox;
	CRoutingTableListCtrl arpTableBox;

	CMFCButton addRouteButton;
	CMFCButton removeRouteButton;
	CMFCButton clearArpTableButton;

	CMFCButton ripEnableButton;
	CMFCButton ripSetTimersButton;
	CStatic ripNextUpdateText;

	CStatic ripUpdateInText;

	CRoutingTableListCtrl natTableBox;
	CStatic currPool;
	CMFCButton enableNatButton;
	CMFCButton enablePatButton;
	CMFCButton setAddressPoolButton;
	CMFCButton removeReservationsButton;
	CMFCButton addStaticNatButton;
	CMFCButton removeStaticNatButton;

	void autoResizeCols(CListCtrl* control);
	void initInterfacesInfos();
	void initRoutingTable();
	void initArpTable();
	void initNatTable();

public:
	void setIpAddr(Interface* i, ipAddressStructure newIpAddressStruct);
	void enableInterface(Interface* i, CMFCButton* enableButton);
	void disableInterface(Interface* i, CMFCButton* disableButton);
	static UINT editIpAddrThread(void* pParam);
	static UINT editRouteThread(void* pParam);
	void addRoute(int index, routeStructure& r);
	void removeRoute(int index);
	void addArp(int index, arpStructure& entry);
	void removeArp(int index);
	static UINT setRipTimersThread(void* pParam);
	void setRipUpdateStatus(int sec);
	void addTranslation(int index, translationStructure& nat);
	void removeTranslation(int index);
	static UINT editStaticNatThread(void* pParam);
	void editPool(ipAddressStructure start, ipAddressStructure end);
	static UINT editPoolThread(void* pParam);
	void updateTranslation(int index, UINT timeout);

public:
	afx_msg void onInt1SetIpButtonClicked();
	afx_msg void onInt2SetIpButtonClicked();
	afx_msg void onInt1EnableButtonClicked();
	afx_msg void onInt2EnableButtonClicked();
	afx_msg void onAddStaticRouteButtonClicked();
	afx_msg void onRemoveStaticRouteButtonClicked();
	afx_msg void onClearArpTableButtonClicked();
	afx_msg void onRipEnableButtonClicked();
	afx_msg void onRipSetTimersButtonClicked();
	afx_msg void onAddNatButtonClicked();
	afx_msg void onRemoveNatButtonClicked();
	afx_msg void onSetPoolButtonClicked();
	afx_msg void onResetNatButtonClicked();
	afx_msg void onEnableNatButtonClicked();
	afx_msg void onEnablePatButtonClicked();
	afx_msg void onInt1NatModeChange();
	afx_msg void onInt2NatModeChange();

protected:
	afx_msg LRESULT onSetIpMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onAddRouteMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onRemoveRouteMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onAddArp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onRemoveArp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onRipUpdateMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onAddNatMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onRemoveNatMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onEditPoolMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onUpdateNatMessage(WPARAM wParam, LPARAM lParam);
};
