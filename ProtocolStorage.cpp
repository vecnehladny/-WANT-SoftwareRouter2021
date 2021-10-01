#include "pch.h"
#include "ProtocolStorage.h"


ProtocolStorage* ProtocolStorage::instance = NULL;
CCriticalSection ProtocolStorage::criticalSection;

ProtocolStorage::ProtocolStorage():
	eth2File(NULL),
	ipFile(NULL),
	portsFile(NULL)
{
	eth2File = fopen("eth2.txt", "r");
	ipFile = fopen("ip.txt", "r");
	portsFile = fopen("ports.txt", "r");

	if (eth2File) initEth2Protocols();
	if (ipFile) initIpProtocols();
	if (portsFile)
	{
		initTcpPorts();
		initUdpPorts();
	}
}


ProtocolStorage::~ProtocolStorage()
{
}


ProtocolStorage* ProtocolStorage::getInstance()
{
	if (instance == NULL)
	{
		criticalSection.Lock();
		if (instance == NULL) {
			instance = new ProtocolStorage();
		} 
		criticalSection.Unlock();
	}

	return instance;
}


CString ProtocolStorage::validateFiles()
{
	CString error(_T("Can't open:"));

	if ((eth2File) && (ipFile) && (portsFile)) {
		return _T("");
	}
	if (!eth2File) {
		error.AppendFormat(_T("\r\neth2.txt"));
	}
	if (!ipFile) {
		error.AppendFormat(_T("\r\nip.txt"));
	}
	if (!portsFile) {
		error.AppendFormat(_T("\r\nports.txt"));
	}

	return error;
}


CString ProtocolStorage::getEth2ProtocolName(WORD type)
{
	char tmp[100], scanstr[50];
	char namestr[50], numstr[10];
	int found = 0;
	CStringA name;

	sprintf(scanstr, "%%[^\t]s%.4X", type);
	sprintf(numstr, "%.4X", type);
	eth2FileCriticalSection.Lock();
	while (fgets(tmp, 100, eth2File) != NULL) if ((strstr(tmp, numstr) != NULL) && (sscanf(tmp, scanstr, namestr) > 0)) {
		found = 1;
		break;
	}
	rewind(eth2File);
	eth2FileCriticalSection.Unlock();
	if (found) name.Format("%s", namestr);
	else name.Format("0x%X", type);

	return CString(name);
}


WORD ProtocolStorage::getEth2ProtocolNum(CStringA Name)
{
	WORD num;
	char tmp[100], scanstr[50];

	sprintf(scanstr, "%s\t%%hX", Name);
	eth2FileCriticalSection.Lock();
	while (fgets(tmp, 100, eth2File) != NULL) if ((strstr(tmp, Name) != NULL) && (sscanf(tmp, scanstr, &num) > 0)) break;
	rewind(eth2File);
	eth2FileCriticalSection.Unlock();

	return num;
}


void ProtocolStorage::initEth2Protocols()
{
	char tmp[100], namestr[50];

	eth2FileCriticalSection.Lock();
	while (fgets(tmp, 100, eth2File) != NULL) if (sscanf(tmp, "%[^\t]s", namestr) > 0) eth2Protocols.Add(CStringA(namestr));
	rewind(eth2File);
	eth2FileCriticalSection.Unlock();
}


int ProtocolStorage::getEth2ProtocolIndex(WORD key)
{
	int i;

	for (i = 0; i < eth2Protocols.GetCount(); i++) if (eth2Protocols[i].Compare(CStringA(getEth2ProtocolName(key))) == 0) return i;
	return -1;
}


CArray<CStringA>& ProtocolStorage::getEth2Protocols()
{
	return eth2Protocols;
}


CString ProtocolStorage::getIpProtocolName(BYTE type)
{
	char tmp[100], scanstr[50];
	char namestr[50], numstr[10];
	int found = 0;
	CStringA name;

	sprintf(scanstr, "%%[^\t]s%u", type);
	sprintf(numstr, "%u", type);
	ipFileCriticalSection.Lock();
	while (fgets(tmp, 100, ipFile) != NULL) if ((strstr(tmp, numstr) != NULL) && (sscanf(tmp, scanstr, namestr) > 0)) {
		found = 1;
		break;
	}
	rewind(ipFile);
	ipFileCriticalSection.Unlock();
	if (found) name.Format("%s", namestr);
	else name.Format("%u", type);

	return CString(name);
}


BYTE ProtocolStorage::getIpProtocolNum(CStringA Name)
{
	WORD num;
	char tmp[100], scanstr[50];

	sprintf(scanstr, "%s\t%%hu", Name);
	ipFileCriticalSection.Lock();
	while (fgets(tmp, 100, ipFile) != NULL) if ((strstr(tmp, Name) != NULL) && (sscanf(tmp, scanstr, &num) > 0)) break;
	rewind(ipFile);
	ipFileCriticalSection.Unlock();

	return num;
}


void ProtocolStorage::initIpProtocols()
{
	char tmp[100], namestr[50];

	ipFileCriticalSection.Lock();
	while (fgets(tmp, 100, ipFile) != NULL) if (sscanf(tmp, "%[^\t]s", namestr) > 0) ipProtocols.Add(CStringA(namestr));
	rewind(ipFile);
	ipFileCriticalSection.Unlock();
}


int ProtocolStorage::getIpProtocolIndex(BYTE key)
{
	int i;

	for (i = 0; i < ipProtocols.GetCount(); i++) if (ipProtocols[i].Compare(CStringA(getIpProtocolName(key))) == 0) return i;
	return -1;
}


CArray<CStringA>& ProtocolStorage::getIpProtocols()
{
	return ipProtocols;
}


WORD ProtocolStorage::getPortNumber(CStringA AppName)
{
	WORD num = 0;
	char tmp[100], scanstr[50];

	sprintf(scanstr, "%s\t%%*3c\t%%hu", AppName);
	portsFileCriticalSection.Lock();
	while (fgets(tmp, 100, portsFile) != NULL) if ((strstr(tmp, AppName) != NULL) && (sscanf(tmp, scanstr, &num) > 0)) break;
	rewind(portsFile);
	portsFileCriticalSection.Unlock();

	return num;
}


CString ProtocolStorage::getAppName(WORD port, int isExtended)
{
	char tmp[100], scanstr[50];
	char namestr[50], numstr[10];
	int found = 0;
	CStringA name;

	sprintf(scanstr, "%%[^\t]s%%*3c\t%u", port);
	sprintf(numstr, "%u", port);
	portsFileCriticalSection.Lock();
	while (fgets(tmp, 100, portsFile) != NULL) if ((strstr(tmp, numstr) != NULL) && (sscanf(tmp, scanstr, namestr) > 0)) {
		found = 1;
		break;
	}
	rewind(portsFile);
	portsFileCriticalSection.Unlock();
	if ((isExtended) && (found)) name.Format("%s (%u)", namestr, port);
	else if (found) name.Format("%s", namestr);
	else name.Format("%u", port);

	return CString(name);
}


void ProtocolStorage::initTcpPorts()
{
	char tmp[100], namestr[50];

	portsFileCriticalSection.Lock();
	while (fgets(tmp, 100, portsFile) != NULL) if ((strstr(tmp, "TCP") != NULL) && (sscanf(tmp, "%[^\t]s", namestr) > 0)) tcpPorts.Add(CStringA(namestr));
	rewind(portsFile);
	portsFileCriticalSection.Unlock();
}


int ProtocolStorage::getTcpPortIndex(WORD port)
{
	int i;

	for (i = 0; i < tcpPorts.GetCount(); i++) if (tcpPorts[i].Compare(CStringA(getAppName(port))) == 0) return i;
	return -1;
}


void ProtocolStorage::initUdpPorts()
{
	char tmp[100], namestr[50];

	portsFileCriticalSection.Lock();
	while (fgets(tmp, 100, portsFile) != NULL) if ((strstr(tmp, "UDP") != NULL) && (sscanf(tmp, "%[^\t]s", namestr) > 0)) udpPorts.Add(CStringA(namestr));
	rewind(portsFile);
	portsFileCriticalSection.Unlock();
}


int ProtocolStorage::getUdpPortIndex(WORD port)
{
	int i;

	for (i = 0; i < udpPorts.GetCount(); i++) if (udpPorts[i].Compare(CStringA(getAppName(port))) == 0) return i;
	return -1;
}


CArray<CStringA>& ProtocolStorage::getTcpPorts()
{
	return tcpPorts;
}


CArray<CStringA>& ProtocolStorage::getUdpPorts()
{
	return udpPorts;
}