#pragma once

#include "Interface.h"
#include "afxtempl.h"
#include "Timer.h"

enum ROUTE_TYPE { 
	CONNECTED, 
	STATIC, 
	RIP };

struct routeStructure {
	ROUTE_TYPE type;
	ipAddressStructure prefix;
	BYTE administrativeDistance;
	BYTE metric;
	ipAddressStructure nextHop;
	Interface* i;
	UINT timer;
	BOOL isHolddown;
	Interface* updateSource;
};

class RoutingTable : private Timer
{
public:
	RoutingTable();
	~RoutingTable();
private:
	CArray<routeStructure> routes;
	BOOL ripEnabled;
	UINT time;
	UINT ripInvalidInterval;
	UINT ripFlushInterval;
	UINT ripHolddownInterval;
	CCriticalSection criticalSectionRip;
	CCriticalSection criticalSectionTime;
public:
	void addConnection(Interface* i, BOOL isDirect);
	void removeConnection(Interface* i, BOOL isDirect);
	int matchPrefixes(ipAddressStructure& prefix1, ipAddressStructure& prefix2);
	int compareRoutes(routeStructure& r1, routeStructure& r2);
	int isDefault(routeStructure& r);
	int addRoute(routeStructure r);
	int removeStaticRoute(int index);
	CCriticalSection criticalSectionTable;
	Interface* doLookup(ipAddressStructure& address, ipAddressStructure** nextHop = NULL);
	UINT ripUpdateInterval;
	virtual void Timeout();
	void resetTime(void);
	void getRipTimeIntervals(UINT& update, UINT& invalid, UINT& flush, UINT& holddown);
	void setRipTimeIntervals(UINT update, UINT invalid, UINT flush, UINT holddown);
	BOOL isRipEnabled(void);
	void enableRip(void);
	void disableRip(void);
	static UINT startRipProcess(void* pParam);
	static UINT stopRipProcess(void* pParam);
	void sendRipInitMessages(void);
	void processRipRequest(Frame* buffer, Interface* sourceInterface);
	void processRipResponse(Frame* buffer, Interface* sourceInterface);
	int addRouteUnsafe(routeStructure r);
	int removeRouteUnsafe(int index);
	int removeRipRoute(ipAddressStructure prefix, Interface* sourceInterface);
	void removeAllRipRoutes(void);
	void changeToInvalidUnsafe(int index);
};