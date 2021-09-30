#include "pch.h"
#include "softwareRouter.h"
#include "SetIpDlg.h"
#include "afxdialogex.h"
#include "Frame.h"


// IPAddrDlg dialog

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
}


BEGIN_MESSAGE_MAP(SetIpDlg, CDialog)
	ON_BN_CLICKED(IDOK, &SetIpDlg::onSetBtnClicked)
END_MESSAGE_MAP()


// IPAddrDlg message handlers


BOOL SetIpDlg::OnInitDialog()
{
	ipAddressStructure ipAddressStruct = setIpInterface->getIpAddressStruct();
	CDialog::OnInitDialog();

	if (setIpInterface->isIpAddressSet())
	{
		setIpAddrBox.SetAddress(ipAddressStruct.octets[0], ipAddressStruct.octets[1], ipAddressStruct.octets[2], ipAddressStruct.octets[3]);
		setMask(setIpInterface->getMask());
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
	BYTE i;

	for (i = 0; i < cidr; i++)
	{
		dwMask >>= 1;
		dwMask |= maxBit;
	}
	setIpMaskBox.SetAddress(dwMask);
}


void SetIpDlg::onSetBtnClicked()
{
	ipAddressStructure ipAddressStruct;
	DWORD dwAddr;

	ipAddressStruct.mask = getCidr();

	if ((ipAddressStruct.mask == INVALID_CIDR))
	{
		AfxMessageBox(_T("Mask is invalid"));
		return;
	}

	setIpAddrBox.GetAddress(dwAddr);
	dwAddr <<= ipAddressStruct.mask;
	if ((dwAddr == 0) || (((~dwAddr) >> ipAddressStruct.mask) == 0))
	{
		AfxMessageBox(_T("Ip address is invalid"));
		return;
	}

	setIpAddrBox.GetAddress(
		ipAddressStruct.octets[0], 
		ipAddressStruct.octets[1],
		ipAddressStruct.octets[2],
		ipAddressStruct.octets[3]);

	theApp.getSoftwareRouterDialog()->setIpAddr(setIpInterface, ipAddressStruct);

	CDialog::OnOK();
}