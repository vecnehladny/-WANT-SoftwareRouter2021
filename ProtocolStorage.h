#pragma once
#include "afxtempl.h"

class ProtocolStorage
{
public:
	static ProtocolStorage* getInstance();
private:
	ProtocolStorage();
	~ProtocolStorage();
	static ProtocolStorage* instance;
	static CCriticalSection criticalSection;
	CCriticalSection eth2FileCriticalSection;
	CCriticalSection ipFileCriticalSection;
	CCriticalSection portsFileCriticalSection;
	FILE* eth2File;
	FILE* ipFile;
	FILE* portsFile;
	CArray<CStringA> eth2Protocols;
	CArray<CStringA> ipProtocols;
	CArray<CStringA> tcpPorts;
	CArray<CStringA> udpPorts;
public:
	CString validateFiles(void);
	CString getEth2ProtocolName(WORD type);
	WORD getEth2ProtocolNum(CStringA Name);
	void initEth2Protocols(void);
	int getEth2ProtocolIndex(WORD key);
	CArray<CStringA>& getEth2Protocols(void);
	CString getIpProtocolName(BYTE type);
	BYTE getIpProtocolNum(CStringA Name);
	void initIpProtocols(void);
	int getIpProtocolIndex(BYTE key);
	CArray<CStringA>& getIpProtocols(void);
	WORD getPortNumber(CStringA AppName);
	CString getAppName(WORD port, int isExtended = 0);
	void initTcpPorts(void);
	int getTcpPortIndex(WORD port);
	void initUdpPorts(void);
	int getUdpPortIndex(WORD port);
	CArray<CStringA>& getTcpPorts(void);
	CArray<CStringA>& getUdpPorts(void);

};