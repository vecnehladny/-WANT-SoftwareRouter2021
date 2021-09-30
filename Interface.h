#pragma once

#include "Frame.h"

struct macAddressStructure {
	BYTE b[6];
};

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

};