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
	newRoute.type = CONNECTED;
	newRoute.prefix = i->getPrefixStruct();
	newRoute.administrativeDistance = 0;
	newRoute.metric = 0;
	newRoute.NextHop.hasNextHop = FALSE;
	newRoute.i = i;

	if (isDirect) {
		criticalSectionTable.Lock();
		if (routes.IsEmpty())
		{
			routes.Add(newRoute);
			theApp.getSoftwareRouterDialog()->addRoute(0, newRoute);
			criticalSectionTable.Unlock();
			return;
		}

		else if ((routes[0].type != CONNECTED)
			|| ((routes[0].prefix.dw == newRoute.prefix.dw) && (routes[0].prefix.mask < newRoute.prefix.mask))
			|| ((routes[0].prefix.dw != newRoute.prefix.dw) && (i->getId() < routes[0].i->getId())))
		{
			routes.InsertAt(0, newRoute);
			theApp.getSoftwareRouterDialog()->addRoute(0, newRoute);
			criticalSectionTable.Unlock();
			return;
		}

		else
		{
			if (routes.GetCount() == 1)
			{
				routes.Add(newRoute);
			}
			else
			{
				routes.InsertAt(1, newRoute);
			}
			theApp.getSoftwareRouterDialog()->addRoute(1, newRoute);
			criticalSectionTable.Lock();
		}
	}
}


void RoutingTable::removeConnection(Interface* i, BOOL isDirect)
{
	if (isDirect) {
		criticalSectionTable.Lock();
		for (int j = 0; j < routes.GetCount(); j++)
			if ((routes[j].type == CONNECTED) && (routes[j].i->getId() == i->getId()))
			{
				routes.RemoveAt(j);
				theApp.getSoftwareRouterDialog()->removeRoute(j);
				break;
			}
		criticalSectionTable.Unlock();
	}
}


int RoutingTable::matchPrefixes(ipAddressStructure& prefix1, ipAddressStructure& prefix2)
{
	if ((prefix1.dw >> (32 - prefix1.mask)) == (prefix2.dw >> (32 - prefix2.mask))) return 1;

	return 0;
}


int RoutingTable::compareRoutes(routeStructure& r1, routeStructure& r2)
{
	if ((r1.administrativeDistance == r2.administrativeDistance) && (r1.metric == r2.metric)
		&& (r1.prefix.dw == r2.prefix.dw) && (r1.prefix.mask == r2.prefix.mask)
		&& (r1.type == r2.type)) return 1;
	return 0;
}


int RoutingTable::isDefault(routeStructure& r)
{
	if ((r.prefix.dw == 0) && (r.prefix.mask == 0)) return 1;
	return 0;
}


Interface* RoutingTable::doLookup(ipAddressStructure& address, ipAddressStructure** nexthop)
{
	Interface* foundInterface;

	for (int i = 0; i < routes.GetCount(); i++)
		if (matchPrefixes(address, routes[i].prefix))
		{
			if (routes[i].NextHop.hasNextHop)
			{
				if (nexthop) *nexthop = &routes[i].NextHop;
				foundInterface = doLookup(routes[i].NextHop, nexthop);
				if (routes[i].i != foundInterface)
				{
					theApp.getSoftwareRouterDialog()->removeRoute(i);
					routes[i].i = foundInterface;
					theApp.getSoftwareRouterDialog()->addRoute(i, routes[i]);
				}
				return foundInterface;
			}
			return routes[i].i;
		}

	return NULL;
}


void RoutingTable::addRoute(routeStructure r)
{
	int i;
	int count;
	CSingleLock lock(&criticalSectionTable);

	lock.Lock();
	count = routes.GetCount();
	if (routes.IsEmpty())
	{
		routes.Add(r);
		theApp.getSoftwareRouterDialog()->addRoute(0, r);
		return;
	}

	for (i = 0; i < count; i++)
	{
		if (compareRoutes(routes[i], r)) return;
		if (isDefault(r))
		{
			if (isDefault(routes[i]))
			{
				if (routes[i].administrativeDistance > r.administrativeDistance) break;
				if ((routes[i].administrativeDistance == r.administrativeDistance) && (routes[i].metric > r.metric)) break;
			}
		}
		else
		{
			if (routes[i].administrativeDistance > r.administrativeDistance) break;
			if ((routes[i].administrativeDistance == r.administrativeDistance) && (routes[i].metric > r.metric)) break;
			if ((routes[i].administrativeDistance == r.administrativeDistance)
				&& (routes[i].metric == r.metric)
				&& (routes[i].prefix.dw == r.prefix.dw)
				&& (routes[i].prefix.mask < r.prefix.mask)) break;
			if (isDefault(routes[i])) break;
		}
	}

	if (i == count)
	{
		routes.Add(r);
		theApp.getSoftwareRouterDialog()->addRoute(count, r);
		return;
	}
	else
	{
		routes.InsertAt(i, r);
		theApp.getSoftwareRouterDialog()->addRoute(i, r);
		return;
	}
}


int RoutingTable::removeRoute(int index)
{
	CSingleLock lock(&criticalSectionTable);

	lock.Lock();
	if (routes[index].type != STATIC) {
		return 1;
	}
	else {
		routes.RemoveAt(index);
		theApp.getSoftwareRouterDialog()->removeRoute(index);
		return 0;
	}	
}