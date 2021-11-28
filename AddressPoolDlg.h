#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#define INVALID_CIDR 33

// AddressPoolDlg dialog

class AddressPoolDlg : public CDialog
{
	DECLARE_DYNAMIC(AddressPoolDlg)

public:
	AddressPoolDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AddressPoolDlg();

	// Dialog Data
	enum { IDD = IDD_SET_NAT_ADDRESSPOOL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CIPAddressCtrl startIpBox;
	CIPAddressCtrl lastIpBox;
	CIPAddressCtrl maskBox;
	CEdit cidrBox;
	CSpinButtonCtrl cidrSpin;
	BYTE getCIDR(void);
	void setMask(BYTE cidr);
public:
	afx_msg void onMaskChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onCidrChange();
	afx_msg void onSetButtonClicked();
};
