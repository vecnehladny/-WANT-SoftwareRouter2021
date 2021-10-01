#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"

#define INVALID_CIDR 33

// StaticRouteDlg dialog

class AddStaticRouteDlg : public CDialog
{
	DECLARE_DYNAMIC(AddStaticRouteDlg)

public:
	AddStaticRouteDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AddStaticRouteDlg();

	// Dialog Data
	enum { IDD = IDD_ADDSTATICROUTE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CIPAddressCtrl prefixBox;
	CIPAddressCtrl maskBox;
	CEdit cidrBox;
	CSpinButtonCtrl cidrSpin;
	CIPAddressCtrl nextHopBox;
	CComboBox interfaceComboBox;
	CEdit administrativeDistanceBox;
	CSpinButtonCtrl administrativeDistanceSpin;
	BYTE getCidr(void);
	void setMask(BYTE cidr);
public:
	afx_msg void onMaskChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onCidrChange();
	afx_msg void onInterfaceSelect();
	afx_msg void onAddButtonClicked();
};