#pragma once

#include "Frame.h"
#include <pcap.h>

class Interface {
public:
	Interface(int id);
	~Interface(void);

private:
	int id;
	CStringA name;
	CString description;
	CString macAddress;
	macAddressStructure macAddressStruct;
	ipAddressStructure ipAddressStruct;
	BOOL enabled;
	BOOL ipAddressIsSet;
	Frame* frames;
	WORD ipHeaderId;
	pcap_t* handle;
	CCriticalSection criticalSectionEnabling;
	CCriticalSection criticalSectionIp;
	CCriticalSection criticalSectionHandle;
	CCriticalSection criticalSectionSend;
	ipAddressStructure prefixStruct;

public:
	int getId();
	CStringA getName();
	void setName(CStringA name);
	CString getDescription();
	void setDescription(CString description);
	BOOL isEnabled();
	BOOL isIpAddressSet();
	void enable();
	void disable();
	CString getMacAddress();
	void setMacAddress(PBYTE macAddress);
	macAddressStructure getMacAddressStruct();
	CString getIpAddress();
	void setIpAddress(BYTE octet1, BYTE octet2, BYTE octet3, BYTE octet4, BYTE mask);
	void setIpAddress(ipAddressStructure newIpAddressStruct);
	ipAddressStructure getIpAddressStruct();
	BYTE getMask();
	CString getPrefix(void);
	ipAddressStructure getPrefixStruct(void);
	void setPrefix(void);
	ipAddressStructure getBroadcastIPAddress(void);
	int isIpLocal(ipAddressStructure& ip);
	int isIpInLocalNetwork(ipAddressStructure& ip);
	WORD generateIpHeaderId(void);
	int openAdapter(void);
	pcap_t* getPcapHandle(void);
	void startReceive(void);
	static UINT receiveThread(void* pParam);
	int sendFrame(Frame* buffer, ipAddressStructure* nextHop = NULL, BOOL UseARP = TRUE);
	Frame* getFrames(void);
};