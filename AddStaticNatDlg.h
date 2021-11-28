#pragma once
#include "afxcmn.h"
#include "resource.h"

class AddStaticNatDlg : public CDialog
{
	DECLARE_DYNAMIC(AddStaticNatDlg)

public:
	AddStaticNatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AddStaticNatDlg();

	// Dialog Data
	enum { IDD = IDD_ADDSTATICNAT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CComboBox modeCombo;
	CIPAddressCtrl globalIpAddressBox;
	CIPAddressCtrl localIpAddressBox;
	CButton portForwardingCheckbox;
	CComboBox protocolCombo;
	CEdit globalPortBox;
	CSpinButtonCtrl globalPortSpin;
	CEdit localPortBox;
	CSpinButtonCtrl localPortSpin;
public:
	afx_msg void OnPortForwardCheckClicked();
	afx_msg void OnSetBtnClicked();
};
