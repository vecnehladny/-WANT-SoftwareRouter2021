#pragma once
#include "afxcmn.h"


// RipTimersDlg dialog

class SetRipTimersDlg : public CDialog
{
	DECLARE_DYNAMIC(SetRipTimersDlg)

public:
	SetRipTimersDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SetRipTimersDlg();

	// Dialog Data
	enum { IDD = IDD_SET_RIP_TIMER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CSpinButtonCtrl updateIntervalSpin;
	CSpinButtonCtrl invalidIntervalSpin;
	CSpinButtonCtrl flushIntervalSpin;
	CSpinButtonCtrl holddownIntervalSpin;
public:
	afx_msg void onSetTimersButtonClicked();
};