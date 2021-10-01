#include "pch.h"
#include "RoutingTable.h"
#include "softwareRouter.h"


RoutingTable::RoutingTable()
{
}


RoutingTable::~RoutingTable()
{
}


void RoutingTable::addConnection(Interface* i, BOOL isDirect)
{
	routeStructure newRoute;

	if (isDirect) {
		newRoute.type = CONNECTED;
		newRoute.prefix = i->GetPrefixStruct();
		newRoute.administrativeDistance = 0;
		newRoute.metric = 0;
		newRoute.NextHop.HasNextHop = FALSE;
		newRoute.i = i;

		if (routes.IsEmpty())
		{
			routes.Add(newRoute);
			theApp.getSoftwareRouterDialog()->InsertRoute(0, newRoute);
			return;
		}

		if ((routes[0].type != CONNECTED)
			|| ((routes[0].prefix.dw == newRoute.prefix.dw) && (TableEntry[0].prefix.SubnetMaskCIDR < newRoute.prefix.SubnetMaskCIDR))
			|| ((routes[0].prefix.dw != newRoute.prefix.dw) && (i->GetIndex() < TableEntry[0].i->GetIndex())))
		{
			TableEntry.InsertAt(0, newRoute);
			theApp.GetRouterDlg()->InsertRoute(0, newRoute);
			return;
		}

		if (TableEntry.GetCount() == 1)
		{
			TableEntry.Add(newRoute);
		}
		else
		{
			TableEntry.InsertAt(1, newRoute);
		}
		theApp.GetRouterDlg()->InsertRoute(1, newRoute);
	}
}


void RoutingTable::RemoveDirectlyConnected(Interface* i)
{
	int j;

	for (j = 0; j < TableEntry.GetCount(); j++)
		if ((TableEntry[j].type == CONNECTED) && (TableEntry[j].i->GetIndex() == i->GetIndex()))
		{
			TableEntry.RemoveAt(j);
			theApp.GetRouterDlg()->RemoveRoute(j);
			break;
		}
}


int RoutingTable::ComparePrefixes(IPaddr& prefix1, IPaddr& prefix2)
{
	if ((prefix1.dw >> (32 - prefix1.SubnetMaskCIDR)) == (prefix2.dw >> (32 - prefix2.SubnetMaskCIDR))) return 1;

	return 0;
}