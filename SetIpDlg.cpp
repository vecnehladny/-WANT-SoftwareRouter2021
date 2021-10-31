#include "pch.h"
#include "softwareRouter.h"
#include "SetIpDlg.h"
#include "afxdialogex.h"
#include "Frame.h"


IMPLEMENT_DYNAMIC(SetIpDlg, CDialog)

SetIpDlg::SetIpDlg(Interface* i, CWnd* pParent /*=NULL*/)
	: CDialog(SetIpDlg::IDD, pParent)
{
	setIpInterface = i;
}

SetIpDlg::~SetIpDlg()
{
}

void SetIpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SETIPIP, setIpAddrBox);
	DDX_Control(pDX, IDC_SETIPMASK, setIpMaskBox);
	DDX_Control(pDX, IDC_SETIPMASKCIDR, setIpMaskCidrBox);
	DDX_Control(pDX, IDC_SETIPMASKCIDRSPIN, setIpMaskCidrSpin);
}


BEGIN_MESSAGE_MAP(SetIpDlg, CDialog)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_SETIPMASK, &SetIpDlg::onMaskChange)
	ON_EN_CHANGE(IDC_SETIPMASKCIDR, &SetIpDlg::onMaskCidrChange)
	ON_BN_CLICKED(IDOK, &SetIpDlg::onSetBtnClicked)
END_MESSAGE_MAP()


// IPAddrDlg message handlers


BOOL SetIpDlg::OnInitDialog()
{
	ipAddressStructure ipAddressStruct = setIpInterface->getIpAddressStruct();
	CDialog::OnInitDialog();

	setIpMaskCidrSpin.SetBuddy(GetDlgItem(IDC_SETIPMASKCIDR));
	setIpMaskCidrSpin.SetRange(0, 32);

	if (setIpInterface->isIpAddressSet())
	{
		setIpAddrBox.SetAddress(
			ipAddressStruct.octets[3],
			ipAddressStruct.octets[2],
			ipAddressStruct.octets[1], 
			ipAddressStruct.octets[0]);
		setMask(setIpInterface->getMask());
		setIpMaskCidrSpin.SetPos(setIpInterface->getMask());
	}

	return TRUE;
}


BYTE SetIpDlg::getCidr(void)
{
	DWORD dwMask;
	BYTE cidr = 0;

	setIpMaskBox.GetAddress(dwMask);
	while ((dwMask >> 31) != 0)
	{
		dwMask <<= 1;
		cidr++;
	}
	if (dwMask == 0) return cidr;
	else return INVALID_CIDR;
}


void SetIpDlg::setMask(BYTE cidr)
{
	DWORD dwMask = 0;
	DWORD maxBit = 1 << 31;

	for (int i = 0; i < cidr; i++)
	{
		dwMask >>= 1;
		dwMask |= maxBit;
	}
	setIpMaskBox.SetAddress(dwMask);
}


void SetIpDlg::onSetBtnClicked()
{
	ipAddressStructure ipAddressStructt;
	DWORD dwAddr;
	BYTE cidrField = (BYTE)GetDlgItemInt(IDC_SETIPMASKCIDR, NULL, 0);

	ipAddressStructt.mask = getCidr();

	if ((ipAddressStructt.mask == INVALID_CIDR) || (cidrField < 0) || (cidrField > 32))
	{
		AfxMessageBox(_T("Mask is invalid"));
		return;
	}

	setIpAddrBox.GetAddress(dwAddr);
	dwAddr <<= ipAddressStructt.mask;
	if ((dwAddr == 0) || (((~dwAddr) >> ipAddressStructt.mask) == 0))
	{
		AfxMessageBox(_T("Ip address is invalid"));
		return;
	}

	setIpAddrBox.GetAddress(
		ipAddressStructt.octets[3], 
		ipAddressStructt.octets[2],
		ipAddressStructt.octets[1],
		ipAddressStructt.octets[0]);

	theApp.getSoftwareRouterDialog()->setIpAddr(setIpInterface, ipAddressStructt);

	CDialog::OnOK();
}


void SetIpDlg::onMaskCidrChange()
{
	BYTE cidr = (BYTE)GetDlgItemInt(IDC_SETIPMASKCIDR, NULL, 0);

	if ((cidr >= 0) && (cidr <= 32)) setMask(cidr);
}


void SetIpDlg::onMaskChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	BYTE cidr = getCidr();

	if (cidr != INVALID_CIDR) {
		setIpMaskCidrSpin.SetPos(cidr);
	}

	*pResult = 0;
}