#pragma once

#include "Timer.h"
#include "Interface.h"
#include "afxtempl.h"

enum NAT_TYPE { STATICNAT, DYNAMICNAT, PAT };

struct translationStructure {
	BYTE layer4Protocol;
	NAT_TYPE type;
	NAT_MODE mode;
	ipAddressStructure global;
	ipAddressStructure local;
	union {
		WORD globalPort;
		WORD globalIcmpId;
	};
	union {
		WORD localPort;
		WORD localIcmpId;
	};
	BOOL isPortForward;
	UINT timeout;
};

class NatTable : private Timer
{
public:
	NatTable(void);
	~NatTable(void);
private:
	CArray<translationStructure> translations;
	CCriticalSection criticalSectionTable;
	CCriticalSection criticalSectionPool;
	CCriticalSection criticalSectionNat;
	CCriticalSection criticalSectionTime;
	CCriticalSection criticalSectionIcmpId;
	BOOL poolSet;
	ipAddressStructure poolStart, poolEnd;
	CMap<DWORD, DWORD, BOOL, BOOL> reservation;
	CMap<WORD, WORD, BOOL, BOOL> icmpIdReservation;
	BOOL dynamicNatEnabled, patEnabled;
public:
	int translate(Frame* buffer, Interface* sourceInterface);
	void addTranslation(translationStructure newRule);
	int removeTranslation(int index);
	int isGlobalIP(ipAddressStructure toCheck);
	int isPoolSet(void);
	ipAddressStructure getFirstIpAddressInPool(void);
	ipAddressStructure getLastIpAddressInPool(void);
	void setPool(ipAddressStructure start, ipAddressStructure end);
	void resetTranslationTimeout(int index);
	virtual void Timeout();
	void removeAllDynamic(void);
	int isDynamicNatEnabled(void);
	int isPatEnabled(void);
	void enableDynamicNat();
	void disableDynamicNat();
	void enablePat();
	void disablePat();
	int createDynamicEntry(Frame* buffer);
	int createOverload(Frame* buffer, Interface* sourceInterface);
};

