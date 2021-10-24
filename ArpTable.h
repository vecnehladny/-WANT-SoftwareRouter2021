#pragma once

#include "Interface.h"

struct arpStructure {
	ipAddressStructure ipAddress;
	macAddressStructure macAddress;
	Interface* i;
};

class ArpTable
{
public:
	ArpTable(void);
	~ArpTable(void);
private:
	CArray<arpStructure> arps;
	CCriticalSection arpCriticalSection;
public:
	void initArpTable(void);
	int getMacAddress(ipAddressStructure ipaddress, Interface* intface, macAddressStructure* result);
	void sendArpRequest(ipAddressStructure destinationIp, Interface* intface);
	void replyToArpRequest(Frame* buffer, Interface* intface);
	void proccessArpReply(Frame* buffer, Interface* intface);
	void addBroadcastOfInterface(Interface* intface);
	void clearArpTable(void);
	void clearArpTable(Interface* intface);
	int areIpAddressesEqual(ipAddressStructure& ipAddr1, ipAddressStructure& ipAddr2);
};