
// softwareRouterDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "softwareRouter.h"
#include "softwareRouterDlg.h"
#include "afxdialogex.h"
#include "SetIpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CsoftwareRouterDlg dialog



CsoftwareRouterDlg::CsoftwareRouterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SOFTWAREROUTER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsoftwareRouterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INT1DEVICENAME, int1DeviceNameText);
	DDX_Control(pDX, IDC_INT1MACADDR, int1MacAddressText);
	DDX_Control(pDX, IDC_INT1IPADDR, int1IpAddressText);
	DDX_Control(pDX, IDC_INT1SETIPBTN, int1SetIpButton);
	DDX_Control(pDX, IDC_INT1ENABLEBTN, int1EnableButton);
	DDX_Control(pDX, IDC_INT2IPADDR, int2IpAddressText);
	DDX_Control(pDX, IDC_INT2MACADDR, int2MacAddressText);
	DDX_Control(pDX, IDC_INT2DEVICENAME, int2DeviceNameText);
	DDX_Control(pDX, IDC_INT2SETIPBTN, int2SetIpButton);
	DDX_Control(pDX, IDC_INT2ENABLEBTN, int2EnableButton);
	DDX_Control(pDX, IDC_ROUTINGTABLE, routingTableBox);
	DDX_Control(pDX, IDC_ARPTABLE, arpTableBox);
	DDX_Control(pDX, IDC_ROUTINGTABLEADD, addRouteButton);
	DDX_Control(pDX, IDC_ROUTINGTABLEREMOVE, removeRouteButton);
	DDX_Control(pDX, IDC_ARPTABLECLEAR, clearArpTableButton);
	DDX_Control(pDX, IDC_ARPTABLESENDREQUEST, sendArpRequestButton);
}

BEGIN_MESSAGE_MAP(CsoftwareRouterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_INT1ENABLEBTN, &CsoftwareRouterDlg::onInt1EnableButtonClicked)
	ON_BN_CLICKED(IDC_INT2ENABLEBTN, &CsoftwareRouterDlg::onInt2EnableButtonClicked)
	ON_MESSAGE(WM_SETIP_MESSAGE, &CsoftwareRouterDlg::onSetIpMessage)
	ON_BN_CLICKED(IDC_INT1SETIPBTN, &CsoftwareRouterDlg::onInt1SetIpButtonClicked)
	ON_BN_CLICKED(IDC_INT2SETIPBTN, &CsoftwareRouterDlg::onInt2SetIpButtonClicked)
	ON_MESSAGE(WM_ADDROUTE_MESSAGE, &CsoftwareRouterDlg::onAddRouteMessage)
	ON_MESSAGE(WM_REMOVEROUTE_MESSAGE, &CsoftwareRouterDlg::onRemoveRouteMessage)
	ON_BN_CLICKED(IDC_ROUTINGTABLEADD, &CsoftwareRouterDlg::onAddStaticRouteButtonClicked)
	ON_BN_CLICKED(IDC_ROUTINGTABLEREMOVE, &CsoftwareRouterDlg::onRemoveStaticRouteButtonClicked)
	ON_BN_CLICKED(IDC_ARPTABLECLEAR, &CsoftwareRouterDlg::onClearArpTableButtonClicked)
	ON_BN_CLICKED(IDC_ARPTABLESENDREQUEST, &CsoftwareRouterDlg::onSendArpRequestButtonClicked)
	ON_MESSAGE(WM_ADDARP_MESSAGE, &CsoftwareRouterDlg::onAddArp)
	ON_MESSAGE(WM_REMOVEARP_MESSAGE, &CsoftwareRouterDlg::onRemoveArp)
	ON_MESSAGE(WM_SENDARPREQUEST_MESSAGE, &CsoftwareRouterDlg::onSendArpRequest)
END_MESSAGE_MAP()


// CsoftwareRouterDlg message handlers

BOOL CsoftwareRouterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	initInterfacesInfos();
	initRoutingTable();
	initArpTable();
	theApp.startThreads();
	sendArpRequestButton.SetTextColor(RGB(255, 0, 0));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CsoftwareRouterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CsoftwareRouterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CsoftwareRouterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CsoftwareRouterDlg::initInterfacesInfos()
{
	int1DeviceNameText.SetWindowTextW(theApp.getInterface(1)->getDescription());
	int2DeviceNameText.SetWindowTextW(theApp.getInterface(2)->getDescription());

	int1MacAddressText.SetWindowTextW(theApp.getInterface(1)->getMacAddress());
	int2MacAddressText.SetWindowTextW(theApp.getInterface(2)->getMacAddress());
}

void CsoftwareRouterDlg::enableInterface(Interface* i, CMFCButton* enableButton)
{
	if (!i->isIpAddressSet())
	{
		AfxMessageBox(_T("You need to configure IP address"));
		return;
	}

	i->enable();
	enableButton->SetWindowTextW(_T("Disable"));
}


void CsoftwareRouterDlg::disableInterface(Interface* i, CMFCButton* disableButton)
{
	i->disable();
	disableButton->SetWindowTextW(_T("Enable"));
}


void CsoftwareRouterDlg::onInt1EnableButtonClicked()
{
	Interface* i = theApp.getInterface(1);

	if (i->isEnabled()) {
		disableInterface(i, &int1EnableButton);
	}
	else {
		enableInterface(i, &int1EnableButton);
	}
}


void CsoftwareRouterDlg::onInt2EnableButtonClicked()
{
	Interface* i = theApp.getInterface(2);

	if (i->isEnabled()) {
		disableInterface(i, &int2EnableButton);
	}
	else {
		enableInterface(i, &int2EnableButton);
	}
}

void CsoftwareRouterDlg::setIpAddr(Interface* i, ipAddressStructure newIpAddressStruct)
{
	i->setIpAddress(newIpAddressStruct);
	SendMessage(WM_SETIP_MESSAGE, 0, (LPARAM)i);
	if (i->isEnabled())
	{
		i->disable();
		i->enable();
	}
}

afx_msg LRESULT CsoftwareRouterDlg::onSetIpMessage(WPARAM wParam, LPARAM lParam)
{
	Interface* i = (Interface*)lParam;

	if (i->getId() == 1) int1IpAddressText.SetWindowTextW(i->getIpAddress());
	else int2IpAddressText.SetWindowTextW(i->getIpAddress());

	return 0;
}

void CsoftwareRouterDlg::onInt1SetIpButtonClicked()
{
	AfxBeginThread(CsoftwareRouterDlg::editIpAddrThread, theApp.getInterface(1));
}


void CsoftwareRouterDlg::onInt2SetIpButtonClicked()
{
	AfxBeginThread(CsoftwareRouterDlg::editIpAddrThread, theApp.getInterface(2));
}

UINT CsoftwareRouterDlg::editIpAddrThread(void* pParam)
{
	SetIpDlg setIpDlg((Interface*)pParam);
	setIpDlg.DoModal();

	return 0;
}

void CsoftwareRouterDlg::initRoutingTable(void)
{
	routingTableBox.SetExtendedStyle(routingTableBox.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_AUTOSIZECOLUMNS);
	routingTableBox.InsertColumn(0, _T("Type"), LVCFMT_CENTER, 36);
	routingTableBox.InsertColumn(1, _T("Network"), LVCFMT_CENTER, 112);
	routingTableBox.InsertColumn(2, _T("AD"), LVCFMT_CENTER, 30);
	routingTableBox.InsertColumn(3, _T("Metric"), LVCFMT_CENTER, 41);
	routingTableBox.InsertColumn(4, _T("Next Hop"), LVCFMT_CENTER, 112);
	routingTableBox.InsertColumn(5, _T("Interface"), LVCFMT_CENTER, 57);
	autoResizeCols(&routingTableBox);
}


void CsoftwareRouterDlg::autoResizeCols(CListCtrl* control)
{
	int columns = control->GetHeaderCtrl()->GetItemCount();

	control->SetRedraw(FALSE);
	control->SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	control->SetColumnWidth(1, 200);
	control->SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
	control->SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
	control->SetColumnWidth(4, 200);
	control->SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
	control->SetRedraw(TRUE);
}

void CsoftwareRouterDlg::addRoute(int index, routeStructure& r)
{
	int* indexptr = (int*)malloc(sizeof(int));
	routeStructure* rptr = (routeStructure*)malloc(sizeof(routeStructure));
	*indexptr = index;
	*rptr = r;
	SendMessage(WM_ADDROUTE_MESSAGE, (WPARAM)indexptr, (LPARAM)rptr);
}


afx_msg LRESULT CsoftwareRouterDlg::onAddRouteMessage(WPARAM wParam, LPARAM lParam)
{
	int* index = (int*)wParam;
	routeStructure* r = (routeStructure*)lParam;
	CString tmp;

	switch (r->type)
	{
	case CONNECTED:
		tmp.Format(_T("C"));
		break;

	case STATIC:
		tmp.Format(_T("S"));
		break;

	case RIP:
		tmp.Format(_T("R"));
		break;
	}
	if ((r->prefix.dw == 0) && (r->prefix.mask == 0)) tmp.AppendFormat(_T("*"));
	routingTableBox.InsertItem(*index, tmp);

	tmp.Format(
		_T("%u.%u.%u.%u/%u"), 
		r->prefix.octets[3],
		r->prefix.octets[2],
		r->prefix.octets[1],
		r->prefix.octets[0],
		r->prefix.mask);
	routingTableBox.SetItemText(*index, 1, tmp);

	tmp.Format(_T("%u"), r->administrativeDistance);
	routingTableBox.SetItemText(*index, 2, tmp);

	tmp.Format(_T("%u"), r->metric);
	routingTableBox.SetItemText(*index, 3, tmp);

	if (r->NextHop.hasNextHop) { 
		tmp.Format(
			_T("%u.%u.%u.%u"), 
			r->NextHop.octets[3],
			r->NextHop.octets[2],
			r->NextHop.octets[1],
			r->NextHop.octets[0]); 
	}
	else { 
		tmp.Format(_T("-")); 
	}
	routingTableBox.SetItemText(*index, 4, tmp);

	if (!r->i) tmp.Format(_T("-"));
	else if (r->NextHop.hasNextHop) tmp.Format(_T("\"Int %d\""), r->i->getId());
	else tmp.Format(_T("Int %d"), r->i->getId());
	routingTableBox.SetItemText(*index, 5, tmp);

	free(index);
	free(r);

	return 0;
}


void CsoftwareRouterDlg::removeRoute(int index)
{
	int* indexptr = (int*)malloc(sizeof(int));

	*indexptr = index;
	SendMessage(WM_REMOVEROUTE_MESSAGE, 0, (LPARAM)indexptr);
}


afx_msg LRESULT CsoftwareRouterDlg::onRemoveRouteMessage(WPARAM wParam, LPARAM lParam)
{
	int* index = (int*)lParam;

	routingTableBox.DeleteItem(*index);
	free(index);

	return 0;
}

afx_msg void CsoftwareRouterDlg::onAddStaticRouteButtonClicked() 
{
	AfxBeginThread(CsoftwareRouterDlg::editRouteThread, NULL);
}
afx_msg void CsoftwareRouterDlg::onRemoveStaticRouteButtonClicked() 
{
	int selected = routingTableBox.GetSelectionMark();

	if (selected == -1) {
		AfxMessageBox(_TEXT("Select route which you want to remove"));
	}
	else {
		if (theApp.getRoutingTable()->removeRoute(selected)) {
			AfxMessageBox(_TEXT("Select static route"));
		}
	}
}

UINT CsoftwareRouterDlg::editRouteThread(void* pParam) 
{
	AddStaticRouteDlg addStaticRouteDialog;

	addStaticRouteDialog.DoModal();

	return 0;

}


void CsoftwareRouterDlg::initArpTable(void)
{
	arpTableBox.SetExtendedStyle(arpTableBox.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	arpTableBox.InsertColumn(0, _T("IP address"), LVCFMT_CENTER, 100);
	arpTableBox.InsertColumn(1, _T("MAC address"), LVCFMT_CENTER, 120);

	theApp.getArpTable()->initArpTable();
}


void CsoftwareRouterDlg::onClearArpTableButtonClicked()
{
	theApp.getArpTable()->clearArpTable();
}

void CsoftwareRouterDlg::addArp(int index, arpStructure& entry)
{
	int* indexptr = (int*)malloc(sizeof(int));
	arpStructure* entryptr = (arpStructure*)malloc(sizeof(arpStructure));
	*indexptr = index;
	*entryptr = entry;
	SendMessage(WM_ADDARP_MESSAGE, (WPARAM)indexptr, (LPARAM)entryptr);
}


afx_msg LRESULT CsoftwareRouterDlg::onAddArp(WPARAM wParam, LPARAM lParam)
{
	int* index = (int*)wParam;
	arpStructure* entry = (arpStructure*)lParam;
	CString tmp;

	tmp.Format(_T("%u.%u.%u.%u"), entry->ipAddress.octets[3], entry->ipAddress.octets[2], entry->ipAddress.octets[1], entry->ipAddress.octets[0]);
	arpTableBox.InsertItem(*index, tmp);

	tmp.Format(_T("%.2X:%.2X:%.2X:%.2X:%.2X:%.2X"), entry->macAddress.section[0], entry->macAddress.section[1], entry->macAddress.section[2], entry->macAddress.section[3], entry->macAddress.section[4], entry->macAddress.section[5]);
	arpTableBox.SetItemText(*index, 1, tmp);

	free(index);
	free(entry);

	return 0;
}


void CsoftwareRouterDlg::removeArp(int index)
{
	int* indexptr = (int*)malloc(sizeof(int));

	*indexptr = index;
	SendMessage(WM_REMOVEARP_MESSAGE, 0, (LPARAM)indexptr);
}


afx_msg LRESULT CsoftwareRouterDlg::onRemoveArp(WPARAM wParam, LPARAM lParam)
{
	int* index = (int*)lParam;

	arpTableBox.DeleteItem(*index);
	free(index);

	return 0;
}


LRESULT CsoftwareRouterDlg::onSendArpRequest(WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}


void CsoftwareRouterDlg::onSendArpRequestButtonClicked()
{
	ipAddressStructure* toSend = (ipAddressStructure*)malloc(sizeof(ipAddressStructure));
	*toSend = theApp.getInterface(1)->getIpAddressStruct();

	toSend->octets[0] = BYTE(0x0001);

	theApp.getArpTable()->sendArpRequest(*toSend, theApp.getInterface(1));
}


void CsoftwareRouterDlg::sendArpRequest()
{
}
