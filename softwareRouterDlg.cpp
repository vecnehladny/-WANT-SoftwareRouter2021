
// softwareRouterDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "softwareRouter.h"
#include "softwareRouterDlg.h"
#include "afxdialogex.h"

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
}

BEGIN_MESSAGE_MAP(CsoftwareRouterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_INT1ENABLEBTN, &CsoftwareRouterDlg::onInt1EnableButtonClicked)
	ON_BN_CLICKED(IDC_INT2ENABLEBTN, &CsoftwareRouterDlg::onInt2EnableButtonClicked)
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