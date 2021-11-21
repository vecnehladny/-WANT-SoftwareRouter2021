#include "pch.h"
#include "RoutingTable.h"
#include "softwareRouter.h"


RoutingTable::RoutingTable() :
	Timer(),
	ripEnabled(FALSE),
	time(0),
	ripUpdateInterval(30),
	ripInvalidInterval(180),
	ripFlushInterval(240),
	ripHolddownInterval(180)
{
	setTimeout(1000);
}


RoutingTable::~RoutingTable()
{
}


void RoutingTable::addConnection(Interface* i, BOOL isDirect)
{
	routeStructure newRoute;
	Frame ripMessage;
	Interface* ripOutInterface;

	newRoute.type = CONNECTED;
	newRoute.prefix = i->getPrefixStruct();
	newRoute.administrativeDistance = 0;
	newRoute.metric = 0;
	newRoute.nextHop.hasNextHop = FALSE;
	newRoute.i = i;

	if (isDirect) {
		criticalSectionTable.Lock();
		if (routes.IsEmpty())
		{
			routes.Add(newRoute);
			theApp.getSoftwareRouterDialog()->addRoute(0, newRoute);
		}

		else if ((routes[0].type != CONNECTED)
			|| ((routes[0].prefix.dw == newRoute.prefix.dw) && (routes[0].prefix.mask < newRoute.prefix.mask))
			|| ((routes[0].prefix.dw != newRoute.prefix.dw) && (i->getId() < routes[0].i->getId())))
		{
			routes.InsertAt(0, newRoute);
			theApp.getSoftwareRouterDialog()->addRoute(0, newRoute);
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
		}
		criticalSectionTable.Unlock();

		if (!isRipEnabled())
		{
			return;
		}

		if (i->getId() == 1)
		{
			ripOutInterface = theApp.getInterface(2);
		}
		else
		{
			ripOutInterface = theApp.getInterface(1);
		}
		if (ripOutInterface->isEnabled())
		{
			ripMessage.addRipRoute(newRoute.prefix, 0);
			ripMessage.generateRipResponse(ripOutInterface->getIpAddressStruct());
			ripOutInterface->sendFrame(&ripMessage);
		}
	}
}


void RoutingTable::removeConnection(Interface* i, BOOL isDirect)
{
	Frame ripMessage;
	Interface* ripOutInterface;

	if (isDirect) {
		criticalSectionTable.Lock();
		for (int j = 0; j < routes.GetCount(); j++)
			if (((routes[j].type == CONNECTED) && (routes[j].i == i))
				|| ((routes[j].type == RIP) && (routes[j].updateSource == i)))
			{
				ripMessage.addRipRoute(routes[j].prefix, 16);
				routes.RemoveAt(j);
				theApp.getSoftwareRouterDialog()->removeRoute(j--);
			}
		criticalSectionTable.Unlock();

		if (!isRipEnabled())
		{
			return;
		}

		if (i->getId() == 1) {
			ripOutInterface = theApp.getInterface(2);
		}
		else
		{
			ripOutInterface = theApp.getInterface(1);
		}

		if (ripOutInterface->isEnabled())
		{
			ripMessage.generateRipResponse(ripOutInterface->getIpAddressStruct());
			ripOutInterface->sendFrame(&ripMessage);
		}
	}
}


int RoutingTable::matchPrefixes(ipAddressStructure& prefix1, ipAddressStructure& prefix2)
{
	if (prefix2.mask == 0) return 1;
	if ((prefix1.dw >> (32 - prefix2.mask)) == (prefix2.dw >> (32 - prefix2.mask))) return 1;

	return 0;
}


int RoutingTable::compareRoutes(routeStructure& r1, routeStructure& r2)
{
	if ((r1.administrativeDistance == r2.administrativeDistance) && (r1.prefix.dw == r2.prefix.dw)
		&& (r1.prefix.mask == r2.prefix.mask) && (r1.type == r2.type)
		&& ((r1.type != RIP) || (r1.updateSource == r2.updateSource)))
	{
		return 1;
	}
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
			if (routes[i].nextHop.hasNextHop)
			{
				if (nexthop) *nexthop = &routes[i].nextHop;
				foundInterface = doLookup(routes[i].nextHop, nexthop);
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


int RoutingTable::addRoute(routeStructure r)
{
	CSingleLock lock(&criticalSectionTable);
	return addRouteUnsafe(r);
}


int RoutingTable::addRouteUnsafe(routeStructure r)
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
		return 0;
	}

	for (i = 0; i < count; i++)
	{
		if (compareRoutes(routes[i], r))
		{
			if ((routes[i].type == RIP) && (routes[i].isHolddown))
			{
				return 1;
			}
			routes[i].timer = 0;
			if (routes[i].metric != r.metric)
			{
				routes.RemoveAt(i);
				theApp.getSoftwareRouterDialog()->removeRoute(i);
				return addRoute(r);
			}
			return 1;
		}
		if (isDefault(r))
		{
			if (isDefault(routes[i]))
			{
				if (routes[i].administrativeDistance > r.administrativeDistance)
				{
					break;
				}
				if ((routes[i].administrativeDistance == r.administrativeDistance) && (routes[i].metric > r.metric))
				{
					break;
				}
			}
		}
		else
		{
			if (routes[i].administrativeDistance > r.administrativeDistance)
			{
				break;
			}
			if ((routes[i].administrativeDistance == r.administrativeDistance) && (routes[i].metric > r.metric)) 
			{ 
				break; 
			}
			if ((routes[i].administrativeDistance == r.administrativeDistance)
				&& (routes[i].metric == r.metric)
				&& (routes[i].prefix.dw == r.prefix.dw)
				&& (routes[i].prefix.mask < r.prefix.mask))
			{
				break;
			}
			if (isDefault(routes[i]))
			{
				break;
			}
		}
	}

	if (i == count)
	{
		routes.Add(r);
		theApp.getSoftwareRouterDialog()->addRoute(count, r);
	}
	else
	{
		routes.InsertAt(i, r);
		theApp.getSoftwareRouterDialog()->addRoute(i, r);
	}

	return 0;
}


int RoutingTable::removeStaticRoute(int index)
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


int RoutingTable::removeRouteUnsafe(int index) 
{
	if (index < routes.GetCount())
	{
		routes.RemoveAt(index);
		theApp.getSoftwareRouterDialog()->removeRoute(index);
		return 0;
	}

	return 1;
}


void RoutingTable::Timeout()
{
	BOOL defaultStaticAdded = FALSE;
	Frame ripMessage[2];
	Interface* interface1 = theApp.getInterface(1);
	Interface* interface2 = theApp.getInterface(2);

	criticalSectionTime.Lock();
	criticalSectionTable.Lock();

	time += 1;
	theApp.getSoftwareRouterDialog()->setRipUpdateStatus(ripUpdateInterval - time);
	for (int i = 0; i < routes.GetCount(); i++)
	{
		if (routes[i].type == RIP)
		{
			routes[i].timer += 1;
			if ((routes[i].timer >= ripInvalidInterval) && (routes[i].metric != 16))
			{
				changeToInvalidUnsafe(i);
			}
			if ((ripHolddownInterval) && (routes[i].timer >= ripInvalidInterval)
				&& (routes[i].timer <= (ripInvalidInterval + ripHolddownInterval)))
			{
				routes[i].isHolddown = TRUE;
			}
			else
			{
				routes[i].isHolddown = FALSE;
			}
			if (time == ripUpdateInterval) ripMessage[routes[i].updateSource->getId() - 1].addRipRoute(routes[i].prefix, routes[i].metric);
			if (routes[i].timer >= ripFlushInterval) removeRouteUnsafe(i--);
		}
		else if (time == ripUpdateInterval)
			if (routes[i].type == CONNECTED)
			{
				ripMessage[routes[i].i->getId() - 1].addRipRoute(routes[i].prefix, routes[i].metric);
			}
			else if ((!defaultStaticAdded) && (routes[i].type == STATIC) && (isDefault(routes[i])))
			{
				ripMessage[0].addRipRoute(routes[i].prefix, routes[i].metric);
				ripMessage[1].addRipRoute(routes[i].prefix, routes[i].metric);
				defaultStaticAdded = TRUE;
			}
	}
	if (time == ripUpdateInterval)
	{
		time = 0;
	}
	theApp.getSoftwareRouterDialog()->setRipUpdateStatus(ripUpdateInterval - time);

	criticalSectionTable.Unlock();
	criticalSectionTime.Unlock();

	if ((ripMessage[0].getRipRouteCount()) && (interface2->isEnabled()))
	{
		ripMessage[0].generateRipResponse(interface2->getIpAddressStruct());
		interface2->sendFrame(&ripMessage[0]);
	}
	if ((ripMessage[1].getRipRouteCount()) && (interface1->isEnabled()))
	{
		ripMessage[1].generateRipResponse(interface1->getIpAddressStruct());
		interface1->sendFrame(&ripMessage[1]);
	}
}


void RoutingTable::resetTime(void)
{
	criticalSectionTime.Lock();
	time = 0;
	criticalSectionTime.Unlock();
}


void RoutingTable::getRipTimeIntervals(UINT& update, UINT& invalid, UINT& flush, UINT& holddown)
{
	update = ripUpdateInterval;
	invalid = ripInvalidInterval;
	flush = ripFlushInterval;
	holddown = ripHolddownInterval;
}


void RoutingTable::setRipTimeIntervals(UINT update, UINT invalid, UINT flush, UINT holddown)
{
	criticalSectionTime.Lock();
	if (ripUpdateInterval != update) time = 0;
	ripUpdateInterval = update;
	ripInvalidInterval = invalid;
	ripFlushInterval = flush;
	ripHolddownInterval = holddown;
	criticalSectionTime.Unlock();
}


BOOL RoutingTable::isRipEnabled(void)
{
	BOOL result;

	criticalSectionRip.Lock();
	result = ripEnabled;
	criticalSectionRip.Unlock();

	return result;
}


void RoutingTable::enableRip(void)
{
	criticalSectionRip.Lock();
	if (!ripEnabled) 
	{
		ripEnabled = TRUE;
	}
	criticalSectionRip.Unlock();
}


void RoutingTable::disableRip(void)
{
	criticalSectionRip.Lock();
	if (ripEnabled) 
	{
		ripEnabled = FALSE;
	}
	criticalSectionRip.Unlock();
}


UINT RoutingTable::startRipProcess(void* pParam)
{
	RoutingTable* routingTable = theApp.getRoutingTable();

	routingTable->resetTime();
	routingTable->enableRip();
	routingTable->sendRipInitMessages();
	theApp.getSoftwareRouterDialog()->setRipUpdateStatus(theApp.getRoutingTable()->ripUpdateInterval);
	routingTable->startTimer();

	return 0;
}


UINT RoutingTable::stopRipProcess(void* pParam)
{
	RoutingTable* routingTable = theApp.getRoutingTable();

	routingTable->stopTimer();
	routingTable->disableRip();
	theApp.getSoftwareRouterDialog()->setRipUpdateStatus(-1);
	routingTable->removeAllRipRoutes();

	return 0;
}


void RoutingTable::sendRipInitMessages(void)
{
	BOOL defaultStaticAdded = FALSE;
	Frame ripMessage[2];
	Interface* interface1 = theApp.getInterface(1);
	Interface* interface2 = theApp.getInterface(2);

	criticalSectionTable.Lock();
	for (int i = 0; i < routes.GetCount(); i++)
		if (routes[i].type == CONNECTED)
		{
			ripMessage[routes[i].i->getId() - 1].addRipRoute(routes[i].prefix, routes[i].metric);
		}
		else if ((!defaultStaticAdded) && (routes[i].type == STATIC) && (isDefault(routes[i])))
		{
			ripMessage[0].addRipRoute(routes[i].prefix, routes[i].metric);
			ripMessage[1].addRipRoute(routes[i].prefix, routes[i].metric);
			defaultStaticAdded = TRUE;
		}
	criticalSectionTable.Unlock();

	if (interface1->isEnabled())
	{
		if (ripMessage[1].getRipRouteCount())
		{
			ripMessage[1].generateRipResponse(interface1->getIpAddressStruct());
			interface1->sendFrame(&ripMessage[1]);
		}
		ripMessage[1].generateRipRequest(interface1->getIpAddressStruct());
		interface1->sendFrame(&ripMessage[1]);
	}
	if (interface2->isEnabled())
	{
		if (ripMessage[0].getRipRouteCount())
		{
			ripMessage[0].generateRipResponse(interface2->getIpAddressStruct());
			interface2->sendFrame(&ripMessage[0]);
		}
		ripMessage[0].generateRipRequest(interface2->getIpAddressStruct());
		interface2->sendFrame(&ripMessage[0]);
	}
}


void RoutingTable::processRipRequest(Frame* buffer, Interface* sourceInterface)
{
	BOOL defaultStaticAdded = FALSE;
	Frame ripMessage;

	if (!isRipEnabled()) return;

	criticalSectionTable.Lock();
	for (int i = 0; i < routes.GetCount(); i++)
	{
		if (((routes[i].type == RIP) && (routes[i].updateSource != sourceInterface))
			|| ((routes[i].type == CONNECTED) && (routes[i].i != sourceInterface))
			|| ((!defaultStaticAdded) && (routes[i].type == STATIC) && (isDefault(routes[i]))))
		{
			ripMessage.addRipRoute(routes[i].prefix, routes[i].metric);
		}
		if (routes[i].type == STATIC)
		{
			defaultStaticAdded = TRUE;
		}
	}
	criticalSectionTable.Unlock();

	if ((sourceInterface->isEnabled()) && (ripMessage.getRipRouteCount()))
	{
		ripMessage.generateRipResponse(sourceInterface->getIpAddressStruct(), &buffer->getSourceIpAddress());
		ripMessage.setDestinationMacAddress(buffer->getSourceMacAddress());
		sourceInterface->sendFrame(&ripMessage, NULL, FALSE);
	}
}


void RoutingTable::processRipResponse(Frame* buffer, Interface* sourceInterface)
{
	routeStructure newRoute;
	Frame ripMessage;
	Interface* outInterface;

	if (!isRipEnabled())
	{
		return;
	}
	CArray<ripResponseStructure>& received = buffer->getRipRoutesFromPacket();

	for (int i = 0; i < received.GetCount(); i++)
	{
		if (received[i].metric == 16)
		{
			if (!removeRipRoute(received[i].prefix, sourceInterface))
			{
				ripMessage.addRipRoute(received[i].prefix, 16);
			}
			continue;
		}

		newRoute.administrativeDistance = 120;
		newRoute.i = NULL;
		newRoute.isHolddown = 0;
		newRoute.metric = received[i].metric;
		newRoute.nextHop = buffer->getSourceIpAddress();
		newRoute.prefix = received[i].prefix;
		newRoute.timer = 0;
		newRoute.type = RIP;
		newRoute.updateSource = sourceInterface;

		if (!addRoute(newRoute))
		{
			ripMessage.addRipRoute(received[i].prefix, received[i].metric);
		}
	}
	buffer->clear();

	if (!ripMessage.getRipRouteCount()) return;

	if (sourceInterface->getId() == 1) outInterface = theApp.getInterface(2);
	else outInterface = theApp.getInterface(1);

	if (outInterface->isEnabled())
	{
		ripMessage.generateRipResponse(outInterface->getIpAddressStruct());
		outInterface->sendFrame(&ripMessage);
	}
}


int RoutingTable::removeRipRoute(ipAddressStructure prefix, Interface* sourceInterface)
{
	criticalSectionTable.Lock();
	for (int i = 0; i < routes.GetCount(); i++)
	{
		if ((routes[i].type == RIP) && (routes[i].prefix.dw == prefix.dw)
			&& (routes[i].prefix.mask == prefix.mask) && (routes[i].updateSource == sourceInterface))
		{
			routes.RemoveAt(i);
			theApp.getSoftwareRouterDialog()->removeRoute(i);
			criticalSectionTable.Unlock();
			return 0;
		}
	}
	criticalSectionTable.Unlock();

	return 1;
}


void RoutingTable::removeAllRipRoutes(void)
{
	criticalSectionTable.Lock();
	for (int i = 0; i < routes.GetCount(); i++)
	{
		if (routes[i].type == RIP)
		{
			removeRouteUnsafe(i--);
		}
	}
	criticalSectionTable.Unlock();
}


void RoutingTable::changeToInvalidUnsafe(int index)
{
	routeStructure tmp;

	tmp.administrativeDistance = routes[index].administrativeDistance;
	tmp.i = routes[index].i;
	tmp.isHolddown = routes[index].isHolddown;
	tmp.metric = 16;
	tmp.nextHop = routes[index].nextHop;
	tmp.prefix = routes[index].prefix;
	tmp.timer = routes[index].timer;
	tmp.type = routes[index].type;
	tmp.updateSource = routes[index].updateSource;

	removeRouteUnsafe(index);
	addRouteUnsafe(tmp);
}