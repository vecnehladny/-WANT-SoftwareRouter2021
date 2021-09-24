#pragma once
#include <afxwin.h>
#include "resource.h"

class CSetIntDlg :
    public CDialog
{
    DECLARE_DYNAMIC(CSetIntDlg)

public:
    CSetIntDlg(CWnd* pParent = NULL);
    virtual ~CSetIntDlg();

    enum { IDD = IDD_SET_INT_DIALOG };

protected:

    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
};

