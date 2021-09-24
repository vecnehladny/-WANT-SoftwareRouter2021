#include "pch.h"
#include "SetIntDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CSetIntDlg, CDialog)

CSetIntDlg::CSetIntDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetIntDlg::IDD, pParent)
{

}

CSetIntDlg::~CSetIntDlg() 
{

}

void CSetIntDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetIntDlg, CDialog)
END_MESSAGE_MAP()
