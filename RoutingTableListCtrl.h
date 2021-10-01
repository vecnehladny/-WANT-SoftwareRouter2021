#pragma once
#include "afxcmn.h"
class CRoutingTableListCtrl : public CListCtrl
{
public:
	CRoutingTableListCtrl();
	~CRoutingTableListCtrl();
protected:
	virtual BOOL onNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};