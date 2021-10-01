#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Interface.h"
#include "resource.h"

#define INVALID_CIDR 33

// IPAddrDlg dialog

class SetIpDlg : public CDialog
{
	DECLARE_DYNAMIC(SetIpDlg)

public:
	SetIpDlg(Interface* i, CWnd* pParent = NULL);   // standard constructor
	virtual ~SetIpDlg();

	// Dialog Data
	enum { IDD = IDD_SET_IP_ADDR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	Interface* setIpInterface;
	CIPAddressCtrl setIpAddrBox;
	CIPAddressCtrl setIpMaskBox;
	CEdit setIpMaskCidrBox;
	CSpinButtonCtrl setIpMaskCidrSpin;
	BYTE getCidr(void);
	void setMask(BYTE cidr);
public:
	afx_msg void onSetBtnClicked();
	afx_msg void onMaskCidrChange();
	afx_msg void onMaskChange(NMHDR* pNMHDR, LRESULT* pResult);
};