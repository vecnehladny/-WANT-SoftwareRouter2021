#include "pch.h"
#include "softwareRouter.h"
#include "AddStaticNatDlg.h"
#include "afxdialogex.h"

#define TCP_ID 6
#define UDP_ID 17


IMPLEMENT_DYNAMIC(AddStaticNatDlg, CDialog)

AddStaticNatDlg::AddStaticNatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AddStaticNatDlg::IDD, pParent)
{

}

AddStaticNatDlg::~AddStaticNatDlg()
{
}

void AddStaticNatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADDSTATICNATMODECOMBO, modeCombo);
	DDX_Control(pDX, IDC_ADDSTATICNATGIP, globalIpAddressBox);
	DDX_Control(pDX, IDC_ADDSTATICNATLIP, localIpAddressBox);
	DDX_Control(pDX, IDC_ADDSTATICNATPFCHECKBOX, portForwardingCheckbox);
	DDX_Control(pDX, IDC_ADDSTATICNATGP, globalPortBox);
	DDX_Control(pDX, IDC_ADDSTATICNATGPSPIN, globalPortSpin);
	DDX_Control(pDX, IDC_ADDSTATICNATLP, localPortBox);
	DDX_Control(pDX, IDC_ADDSTATICNATLPSPIN, localPortSpin);
	DDX_Control(pDX, IDC_ADDSTATICNATPROTOCOLCOMBO, protocolCombo);
}


BEGIN_MESSAGE_MAP(AddStaticNatDlg, CDialog)
	ON_BN_CLICKED(IDC_ADDSTATICNATPFCHECKBOX, &AddStaticNatDlg::OnPortForwardCheckClicked)
	ON_BN_CLICKED(IDOK, &AddStaticNatDlg::OnSetBtnClicked)
END_MESSAGE_MAP()


// StaticNatDlg message handlers

BOOL AddStaticNatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	modeCombo.SetCurSel(0);

	portForwardingCheckbox.SetCheck(BST_UNCHECKED);
	protocolCombo.SetCurSel(0);
	protocolCombo.EnableWindow(FALSE);
	globalPortSpin.SetBuddy(GetDlgItem(IDC_ADDSTATICNATGP));
	globalPortSpin.SetRange32(1, 65535);
	globalPortSpin.EnableWindow(FALSE);
	globalPortBox.EnableWindow(FALSE);
	localPortSpin.SetBuddy(GetDlgItem(IDC_ADDSTATICNATLP));
	localPortSpin.SetRange32(1, 65535);
	localPortSpin.EnableWindow(FALSE);
	localPortBox.EnableWindow(FALSE);

	return TRUE;
}

void AddStaticNatDlg::OnPortForwardCheckClicked()
{
	if (portForwardingCheckbox.GetCheck())
	{
		protocolCombo.EnableWindow(TRUE);
		globalPortSpin.EnableWindow(TRUE);
		globalPortBox.EnableWindow(TRUE);
		localPortSpin.EnableWindow(TRUE);
		localPortBox.EnableWindow(TRUE);
	}
	else
	{
		protocolCombo.EnableWindow(FALSE);
		globalPortSpin.EnableWindow(FALSE);
		globalPortBox.EnableWindow(FALSE);
		localPortSpin.EnableWindow(FALSE);
		localPortBox.EnableWindow(FALSE);
	}
}


void AddStaticNatDlg::OnSetBtnClicked()
{
	translationStructure newTrans;
	WORD globalPort = (WORD)GetDlgItemInt(IDC_ADDSTATICNATGP, NULL, 0);
	WORD localPort = (WORD)GetDlgItemInt(IDC_ADDSTATICNATLP, NULL, 0);

	newTrans.type = STATICNAT;

	if (modeCombo.GetCurSel() == 0) newTrans.mode = INSIDE;
	else newTrans.mode = OUTSIDE;

	globalIpAddressBox.GetAddress(newTrans.global.octets[3], newTrans.global.octets[2], newTrans.global.octets[1], newTrans.global.octets[0]);
	localIpAddressBox.GetAddress(newTrans.local.octets[3], newTrans.local.octets[2], newTrans.local.octets[1], newTrans.local.octets[0]);

	if (portForwardingCheckbox.GetCheck())
	{
		if ((globalPort < 1) || (globalPort > 65535))
		{
			AfxMessageBox(_T("The global port must be between 1 and 65535!"));
			return;
		}
		if ((localPort < 1) || (localPort > 65535))
		{
			AfxMessageBox(_T("The local port must be between 1 and 65535!"));
			return;
		}
		newTrans.isPortForward = TRUE;
		if (protocolCombo.GetCurSel() == 0) newTrans.layer4Protocol = TCP_ID;
		else newTrans.layer4Protocol = UDP_ID;
		newTrans.globalPort = globalPort;
		newTrans.localPort = localPort;
	}
	else newTrans.isPortForward = FALSE;

	theApp.getNatTable()->addTranslation(newTrans);

	CDialog::OnOK();
}