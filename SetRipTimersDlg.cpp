#include "pch.h"
#include "SoftwareRouter.h"
#include "SetRipTimersDlg.h"
#include "afxdialogex.h"


// RipTimersDlg dialog

IMPLEMENT_DYNAMIC(SetRipTimersDlg, CDialog)

SetRipTimersDlg::SetRipTimersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetRipTimersDlg::IDD, pParent)
{

}

SetRipTimersDlg::~SetRipTimersDlg()
{
}

void SetRipTimersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SETRIPUPDATESPIN, updateIntervalSpin);
	DDX_Control(pDX, IDC_SETRIPINVALIDSPIN, invalidIntervalSpin);
	DDX_Control(pDX, IDC_SETRIPFLUSHSPIN, flushIntervalSpin);
	DDX_Control(pDX, IDC_SETRIPHOLDDOWNSPIN, holddownIntervalSpin);
}


BEGIN_MESSAGE_MAP(SetRipTimersDlg, CDialog)
	ON_BN_CLICKED(IDOK, &SetRipTimersDlg::onSetTimersButtonClicked)
END_MESSAGE_MAP()


// RipTimersDlg message handlers

BOOL SetRipTimersDlg::OnInitDialog()
{
	UINT update, invalid, flush, holddown;

	CDialog::OnInitDialog();

	theApp.getRoutingTable()->getRipTimeIntervals(update, invalid, flush, holddown);

	updateIntervalSpin.SetBuddy(GetDlgItem(IDC_SETRIPUPDATE));
	updateIntervalSpin.SetRange32(5, 1000);
	updateIntervalSpin.SetPos(update);

	invalidIntervalSpin.SetBuddy(GetDlgItem(IDC_SETRIPINVALID));
	invalidIntervalSpin.SetRange32(5, 1000);
	invalidIntervalSpin.SetPos(invalid);

	flushIntervalSpin.SetBuddy(GetDlgItem(IDC_SETRIPFLUSH));
	flushIntervalSpin.SetRange32(5, 1000);
	flushIntervalSpin.SetPos(flush);

	holddownIntervalSpin.SetBuddy(GetDlgItem(IDC_SETRIPHOLDDOWN));
	holddownIntervalSpin.SetRange32(0, 1000);
	holddownIntervalSpin.SetPos(holddown);

	return TRUE;
}


void SetRipTimersDlg::onSetTimersButtonClicked()
{
	theApp.getRoutingTable()->setRipTimeIntervals(updateIntervalSpin.GetPos(), invalidIntervalSpin.GetPos(), flushIntervalSpin.GetPos(), holddownIntervalSpin.GetPos());

	CDialog::OnOK();
}