#include "pch.h"
#include "RoutingTableListCtrl.h"


CRoutingTableListCtrl::CRoutingTableListCtrl()
{
}


CRoutingTableListCtrl::~CRoutingTableListCtrl()
{
}


BOOL CRoutingTableListCtrl::onNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch (((NMHDR*)lParam)->code)
	{
	case HDN_BEGINTRACKW:
	case HDN_BEGINTRACKA:
		*pResult = TRUE;
		return TRUE;
	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}