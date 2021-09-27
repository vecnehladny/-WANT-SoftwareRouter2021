
// softwareRouterDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxbutton.h"
#include "Interface.h"


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
	void enableInterface(Interface* i, CMFCButton* enableButton);
	void disableInterface(Interface* i, CMFCButton* disableButton);

public:
	afx_msg void onInt1EnableButtonClicked();
	afx_msg void onInt2EnableButtonClicked();
};
