#include "pch.h"
#include "softwareRouter.h"
#include "AddStaticRouteDlg.h"
#include "afxdialogex.h"
#include "Frame.h"


// StaticRouteDlg dialog

IMPLEMENT_DYNAMIC(AddStaticRouteDlg, CDialog)

AddStaticRouteDlg::AddStaticRouteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AddStaticRouteDlg::IDD, pParent)
{

}

AddStaticRouteDlg::~AddStaticRouteDlg()
{
}

void AddStaticRouteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADDSTATICROUTEPREFIX, prefixBox);
	DDX_Control(pDX, IDC_ADDSTATICROUTEMASK, maskBox);
	DDX_Control(pDX, IDC_ADDSTATICROUTEMASKCIDR, cidrBox);
	DDX_Control(pDX, IDC_ADDSTATICROUTEMASKCIDRSPIN, cidrSpin);
	DDX_Control(pDX, IDC_ADDSTATICROUTENEXTHOP, nextHopBox);
	DDX_Control(pDX, IDC_ADDSTATICROUTEINTERFACE, interfaceComboBox);
	DDX_Control(pDX, IDC_ADDSTATICROUTEAD, administrativeDistanceBox);
	DDX_Control(pDX, IDC_ADDSTATICROUTEADSPIN, administrativeDistanceSpin);
}


BEGIN_MESSAGE_MAP(AddStaticRouteDlg, CDialog)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_ADDSTATICROUTEMASK, &AddStaticRouteDlg::onMaskChange)
	ON_EN_CHANGE(IDC_ADDSTATICROUTEMASKCIDR, &AddStaticRouteDlg::onCidrChange)
	ON_CBN_SELCHANGE(IDC_ADDSTATICROUTEINTERFACE, &AddStaticRouteDlg::onInterfaceSelect)
	ON_BN_CLICKED(IDOK, &AddStaticRouteDlg::onAddButtonClicked)
END_MESSAGE_MAP()


// StaticRouteDlg message handlers

BOOL AddStaticRouteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	cidrSpin.SetBuddy(GetDlgItem(IDC_ADDSTATICROUTEMASKCIDR));
	cidrSpin.SetRange32(0, 32);
	interfaceComboBox.SetCurSel(0);
	administrativeDistanceSpin.SetBuddy(GetDlgItem(IDC_ADDSTATICROUTEAD));
	administrativeDistanceSpin.SetRange32(1, 254);
	administrativeDistanceSpin.SetPos(1);

	return TRUE;
}


BYTE AddStaticRouteDlg::getCidr()
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


void AddStaticRouteDlg::setMask(BYTE cidr)
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


void AddStaticRouteDlg::onMaskChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	BYTE cidr = getCidr();

	if (cidr != INVALID_CIDR) cidrSpin.SetPos(cidr);

	*pResult = 0;
}


void AddStaticRouteDlg::onCidrChange()
{
	BYTE cidr = (BYTE)GetDlgItemInt(IDC_ADDSTATICROUTEMASKCIDR, NULL, 0);

	if ((cidr >= 0) && (cidr <= 32)) setMask(cidr);
}


void AddStaticRouteDlg::onInterfaceSelect()
{
	if (interfaceComboBox.GetCurSel() != 0) nextHopBox.EnableWindow(FALSE);
	else nextHopBox.EnableWindow(TRUE);
}


void AddStaticRouteDlg::onAddButtonClicked()
{
	routeStructure newRoute;
	DWORD dwAddr;
	BYTE cidr = (BYTE)GetDlgItemInt(IDC_ADDSTATICROUTEMASKCIDR, NULL, 0);
	BYTE ad = (BYTE)GetDlgItemInt(IDC_ADDSTATICROUTEAD, NULL, 0);

	newRoute.prefix.mask = getCidr();

	if ((newRoute.prefix.mask == INVALID_CIDR) || (cidr < 0) || (cidr > 32))
	{
		AfxMessageBox(_T("Mask is invalid"));
		return;
	}

	prefixBox.GetAddress(dwAddr);
	dwAddr <<= newRoute.prefix.mask;
	if (dwAddr != 0)
	{
		AfxMessageBox(_T("Prefix is invalid"));
		return;
	}

	if ((ad < 1) || (ad > 254))
	{
		AfxMessageBox(_T("Administrative distance is invalid"));
		return;
	}

	prefixBox.GetAddress(newRoute.prefix.octets[3], newRoute.prefix.octets[2], newRoute.prefix.octets[1], newRoute.prefix.octets[0]);

	newRoute.type = STATIC;
	newRoute.administrativeDistance = ad;
	newRoute.metric = 0;

	if (interfaceComboBox.GetCurSel() == 0)
	{
		newRoute.NextHop.hasNextHop = TRUE;
		nextHopBox.GetAddress(newRoute.NextHop.octets[3], newRoute.NextHop.octets[2], newRoute.NextHop.octets[1], newRoute.NextHop.octets[0]);
		newRoute.i = NULL;
	}
	else
	{
		newRoute.NextHop.hasNextHop = FALSE;
		if (interfaceComboBox.GetCurSel() == 1) newRoute.i = theApp.getInterface(1);
		else newRoute.i = theApp.getInterface(2);
	}

	theApp.getRoutingTable()->addRoute(newRoute);

	CDialog::OnOK();
}