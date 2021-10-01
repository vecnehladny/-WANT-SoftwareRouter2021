#pragma once

#include "Interface.h"
#include "afxtempl.h"

enum ROUTE_TYPE { 
	CONNECTED, 
	STATIC, 
	RIP };

struct routeStructure {
	ROUTE_TYPE type;
	ipAddressStructure prefix;
	BYTE administrativeDistance;
	BYTE metric;
	ipAddressStructure NextHop;
	Interface* i;
};

class RoutingTable
{
public:
	RoutingTable();
	~RoutingTable();
private:
	CArray<routeStructure> routes;
public:
	void addConnection(Interface* i, BOOL isDirect);
	void removeConnection(Interface* i, BOOL isDirect);
	int matchPrefixes(ipAddressStructure& prefix1, ipAddressStructure& prefix2);
	int compareRoutes(routeStructure& r1, routeStructure& r2);
	int isDefault(routeStructure& r);
	void addRoute(routeStructure r);
	int removeRoute(int index);
	Interface* findInterface(ipAddressStructure& address);
};