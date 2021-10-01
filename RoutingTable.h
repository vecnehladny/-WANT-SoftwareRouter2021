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
	int comparePrefixes(ipAddressStructure& prefix1, ipAddressStructure& prefix2);
};