// AddressPoolDlg.cpp : implementation file
//

#include "pch.h"
#include "softwareRouter.h"
#include "AddressPoolDlg.h"
#include "afxdialogex.h"


// AddressPoolDlg dialog

IMPLEMENT_DYNAMIC(AddressPoolDlg, CDialog)

AddressPoolDlg::AddressPoolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AddressPoolDlg::IDD, pParent)
{

}

AddressPoolDlg::~AddressPoolDlg()
{
}

void AddressPoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SETNATAPSTARTIP, startIpBox);
	DDX_Control(pDX, IDC_SETNATAPENDIP, lastIpBox);
	DDX_Control(pDX, IDC_SETNATAPMASK, maskBox);
	DDX_Control(pDX, IDC_SETNATAPMASKCIDR, cidrBox);
	DDX_Control(pDX, IDC_SETNATAPMASKCIDRSPIN, cidrSpin);
}


BEGIN_MESSAGE_MAP(AddressPoolDlg, CDialog)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_SETNATAPMASK, &AddressPoolDlg::onMaskChange)
	ON_EN_CHANGE(IDC_SETNATAPMASKCIDR, &AddressPoolDlg::onCidrChange)
	ON_BN_CLICKED(IDOK, &AddressPoolDlg::onSetButtonClicked)
END_MESSAGE_MAP()


// AddressPoolDlg message handlers


BOOL AddressPoolDlg::OnInitDialog()
{
	ipAddressStructure start, end;
	CDialog::OnInitDialog();

	cidrSpin.SetBuddy(GetDlgItem(IDC_SETNATAPMASKCIDR));
	cidrSpin.SetRange32(0, 32);

	if (theApp.getNatTable()->isPoolSet())
	{
		start = theApp.getNatTable()->getFirstIpAddressInPool();
		end = theApp.getNatTable()->getLastIpAddressInPool();
		startIpBox.SetAddress(start.dw);
		lastIpBox.SetAddress(end.dw);
		setMask(start.mask);
		cidrSpin.SetPos(start.mask);
	}

	return TRUE;
}


BYTE AddressPoolDlg::getCIDR(void)
{
	DWORD dwMask;
	BYTE cidr = 0;

	maskBox.GetAddress(dwMask);
	while ((dwMask >> 31) != 0)
	{
		dwMask <<= 1;
		cidr++;
	}
	if (dwMask == 0) return cidr;
	else return INVALID_CIDR;
}


void AddressPoolDlg::setMask(BYTE cidr)
{
	DWORD dwMask = 0;
	DWORD maxBit = 1 << 31;
	BYTE i;

	for (i = 0; i < cidr; i++)
	{
		dwMask >>= 1;
		dwMask |= maxBit;
	}
	maskBox.SetAddress(dwMask);
}


void AddressPoolDlg::onMaskChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	BYTE cidr = getCIDR();

	if (cidr != INVALID_CIDR) cidrSpin.SetPos(cidr);

	*pResult = 0;
}


void AddressPoolDlg::onCidrChange()
{
	BYTE cidr = (BYTE)GetDlgItemInt(IDC_SETNATAPMASKCIDR, NULL, 0);

	if ((cidr >= 0) && (cidr <= 32)) setMask(cidr);
}


void AddressPoolDlg::onSetButtonClicked()
{
	ipAddressStructure startIp, endIp;
	DWORD dwAddr;
	BYTE cidrSpinBox = (BYTE)GetDlgItemInt(IDC_SETNATAPMASKCIDR, NULL, 0);
	BYTE cidr;

	cidr = getCIDR();

	if ((cidr == INVALID_CIDR) || (cidrSpinBox < 0) || (cidrSpinBox > 32))
	{
		AfxMessageBox(_T("The entered subnet mask or CIDR format is invalid!"));
		return;
	}
	startIp.mask = cidr;
	endIp.mask = cidr;

	startIpBox.GetAddress(dwAddr);
	dwAddr <<= cidr;
	if ((dwAddr == 0) || (((~dwAddr) >> cidr) == 0))
	{
		AfxMessageBox(_T("Invalid first IP address!"));
		return;
	}
	startIpBox.GetAddress(startIp.dw);

	lastIpBox.GetAddress(dwAddr);
	dwAddr <<= cidr;
	if ((dwAddr == 0) || (((~dwAddr) >> cidr) == 0))
	{
		AfxMessageBox(_T("Invalid last IP address!"));
		return;
	}
	lastIpBox.GetAddress(endIp.dw);

	if ((startIp.dw >> (32 - cidr)) != (endIp.dw >> (32 - cidr)))
	{
		AfxMessageBox(_T("The IP addresses are not from the same subnet!"));
		return;
	}

	if (startIp.dw > endIp.dw)
	{
		AfxMessageBox(_T("The first IP address can not be higher than the last!"));
		return;
	}

	theApp.getNatTable()->setPool(startIp, endIp);

	CDialog::OnOK();
}
