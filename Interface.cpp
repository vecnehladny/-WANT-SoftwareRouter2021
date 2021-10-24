#include "pch.h"
#include "Interface.h"
#include "softwareRouter.h"


Interface::Interface(int id) :
	macAddress(_TEXT("unknown")),
	id(id),
	enabled(FALSE),
	ipAddressIsSet(FALSE),
	handle(NULL)
{
	for (int i = 0; i < 6; i++) {
		macAddressStruct.section[i] = 0x00;
	}
	for (int i = 0; i < 4; i++) {
		macAddressStruct.section[i] = 0x00;
	}
	ipAddressStruct.mask = 0;
	frames = new Frame();
}

Interface::~Interface()
{
}

int Interface::getId()
{
	return id;
}

CStringA Interface::getName()
{
	return name;
}

CString Interface::getDescription()
{
	return description;
}

void Interface::setName(CStringA name)
{
	this->name = name;
}

void Interface::setDescription(CString description)
{
	this->description = description;
}

BOOL Interface::isEnabled()
{
	BOOL result;

	criticalSectionEnabling.Lock();
	result = enabled;
	criticalSectionEnabling.Unlock();

	return result;
}

BOOL Interface::isIpAddressSet()
{
	return ipAddressIsSet;
}

void Interface::enable()
{
	criticalSectionEnabling.Lock();
	if (enabled != TRUE) {
		enabled = TRUE;
		theApp.getRoutingTable()->addConnection(this, TRUE);
		theApp.getArpTable()->addBroadcastOfInterface(this);
		startReceive();
	}	
	criticalSectionEnabling.Unlock();
}

void Interface::disable() 
{
	criticalSectionEnabling.Lock();
	if (enabled != FALSE) {
		enabled = FALSE;
		theApp.getRoutingTable()->removeConnection(this, TRUE);
		theApp.getArpTable()->clearArpTable(this);
	}
	criticalSectionEnabling.Unlock();
}

CString Interface::getMacAddress()
{
	return macAddress;
}

macAddressStructure Interface::getMacAddressStruct()
{
	return macAddressStruct;
}

void Interface::setMacAddress(PBYTE macAddress)
{
	this->macAddress.Empty();
	for (int i = 0; i < 6; i++) {
		if (i == 5) {
			this->macAddress.AppendFormat(_T("%.2X"), (int)macAddress[i]);
		}
		else {
			this->macAddress.AppendFormat(_T("%.2X-"), (int)macAddress[i]);
		}
		this->macAddressStruct.section[i] = macAddress[i];
	}
}

CString Interface::getIpAddress()
{
	CString toReturn;

	criticalSectionIp.Lock();
	toReturn.Format(
		_T("%u.%u.%u.%u/%u"),
		ipAddressStruct.octets[3],
		ipAddressStruct.octets[2],
		ipAddressStruct.octets[1],
		ipAddressStruct.octets[0],
		ipAddressStruct.mask);
	criticalSectionIp.Unlock();

	return toReturn;
}

ipAddressStructure Interface::getIpAddressStruct()
{
	CSingleLock lock(&criticalSectionIp);

	lock.Lock();
	return ipAddressStruct;
}

void Interface::setIpAddress(BYTE octet1, BYTE octet2, BYTE octet3, BYTE octet4, BYTE mask){
	criticalSectionIp.Lock();
	ipAddressStruct.octets[0] = octet1;
	ipAddressStruct.octets[1] = octet2;
	ipAddressStruct.octets[2] = octet3;
	ipAddressStruct.octets[3] = octet4;
	ipAddressStruct.mask = mask;
	setPrefix();
	ipHeaderId = 0;
	criticalSectionIp.Unlock();

	ipAddressIsSet = TRUE;
}

BYTE Interface::getMask()
{
	CSingleLock lock(&criticalSectionIp);

	lock.Lock();
	return ipAddressStruct.mask;
}

void Interface::setIpAddress(ipAddressStructure newIpAddressStruct) {
	criticalSectionIp.Lock();
	ipAddressStruct = newIpAddressStruct;
	setPrefix();
	ipHeaderId = 0;
	criticalSectionIp.Unlock();

	ipAddressIsSet = TRUE;
}

CString Interface::getPrefix(void)
{
	ipAddressStructure prefix = getPrefixStruct();
	CString prefixString;

	criticalSectionIp.Lock();
	prefixString.Format(
		_TEXT("%u.%u.%u.%u/%u"),
		prefix.octets[3], 
		prefix.octets[2],
		prefix.octets[1], 
		prefix.octets[0],
		prefix.mask);
	criticalSectionIp.Unlock();

	return prefixString;
}


ipAddressStructure Interface::getPrefixStruct(void)
{
	ipAddressStructure prefix;

	CSingleLock lock(&criticalSectionIp);

	lock.Lock();
	return prefixStruct;

	return prefix;
}

ipAddressStructure Interface::getBroadcastIPAddress(void)
{
	ipAddressStructure BcIP;
	DWORD wcMask = ~0;

	criticalSectionIp.Lock();
	wcMask >>= ipAddressStruct.mask;
	BcIP.dw = ipAddressStruct.dw | wcMask;
	BcIP.mask = ipAddressStruct.mask;
	criticalSectionIp.Unlock();

	return BcIP;
}


int Interface::isIpLocal(ipAddressStructure& ip)
{
	CSingleLock lock(&criticalSectionIp);

	lock.Lock();
	if (ipAddressStruct.dw == ip.dw) {
		return 1;
	}
	else {
		return 0;
	}
}


int Interface::isIpInLocalNetwork(ipAddressStructure& ip)
{
	CSingleLock lock(&criticalSectionIp);

	lock.Lock();
	if ((ip.dw >> (32 - prefixStruct.mask)) == (prefixStruct.dw >> (32 - prefixStruct.mask))) {
		return 1;
	}
	else {
		return 0;
	}
}

void Interface::setPrefix(void)
{
	prefixStruct.dw = (ipAddressStruct.dw >> (32 - ipAddressStruct.mask)) << (32 - ipAddressStruct.mask);
	prefixStruct.mask = ipAddressStruct.mask;
}


WORD Interface::generateIpHeaderId(void)
{
	WORD value;

	criticalSectionIp.Lock();
	value = ipHeaderId;
	if (ipHeaderId == 0xFFFF) {
		ipHeaderId = 0;
	}
	else {
		ipHeaderId++;
	}
	criticalSectionIp.Unlock();

	return value;
}

int Interface::openAdapter(void)
{
	char errbuf[PCAP_ERRBUF_SIZE];
	CStringA errorstring;
	int flag = PCAP_OPENFLAG_PROMISCUOUS | PCAP_OPENFLAG_NOCAPTURE_LOCAL | PCAP_OPENFLAG_MAX_RESPONSIVENESS;

	criticalSectionHandle.Lock();
	if (!handle) {
		handle = pcap_open(name, 65536, flag, 1000, NULL, errbuf);
	}
	if (!handle)
	{
		errorstring.Format("Unable to open the adapter on INTERFACE %d!\r\n%s", id, errbuf);
		theApp.getSoftwareRouterDialog()->MessageBox(CString(errorstring), _T("Error"), MB_ICONERROR);
		return 0;
	}
	criticalSectionHandle.Unlock();

	return 1;
}


pcap_t* Interface::getPcapHandle(void)
{
	return handle;
}


void Interface::startReceive(void)
{
	if ((!handle) && (!openAdapter())) {
		return;
	}
	AfxBeginThread(Interface::receiveThread, this);
}


UINT Interface::receiveThread(void* pParam)
{
	Interface* inInterface = (Interface*)pParam;
	Frame* buffer = inInterface->getFrames();
	pcap_t* handle = inInterface->getPcapHandle();
	CStringA errorstring;
	pcap_pkthdr* header = NULL;
	const u_char* frame = NULL;
	int toReturn = 0;

	toReturn = pcap_setbuff(handle, 102400);
	if (toReturn == -1)
	{
		errorstring.Format("Error during the buffer allocation on INTERFACE %d!\r\n%s", inInterface->getId(), pcap_geterr(handle));
		theApp.getSoftwareRouterDialog()->MessageBox(CString(errorstring), _T("Error"), MB_ICONERROR);
		return 0;
	}

	while ((inInterface->isEnabled()) && ((toReturn = pcap_next_ex(handle, &header, &frame)) >= 0))
	{
		if (toReturn == 0) continue;
		buffer->addFrame(header->len, frame);
	}
	if (toReturn == -1)
	{
		errorstring.Format("Error receiving the packets on INTERFACE %d!\r\n%s", inInterface->getId(), pcap_geterr(handle));
		theApp.getSoftwareRouterDialog()->MessageBox(CString(errorstring), _T("Error"), MB_ICONERROR);
	}

	return 0;
}


int Interface::sendFrame(Frame* buffer, ipAddressStructure* NextHop, BOOL UseARP)
{
	macAddressStructure req;
	int retval;

	if (!isEnabled()) {
		return 0;
	}

	buffer->setSourceMacAddress(macAddressStruct);
	if ((UseARP) && (buffer->getLayer3Type() == 0x0800))
	{
		if (NextHop) {
			retval = theApp.getArpTable()->getMacAddress(*NextHop, this, &req);
		}
		else {
			retval = theApp.getArpTable()->getMacAddress(buffer->getDestinationIpAddress(), this, &req);
		}
		if (!retval) {
			buffer->setDestinationMacAddress(req);
		}
		else {
			return 0;
		}
	}

	criticalSectionSend.Lock();

	if ((!handle) && (!openAdapter()))
	{
		criticalSectionSend.Unlock();
		return 1;
	}

	if (pcap_sendpacket(handle, buffer->getData(), buffer->getLength()) != 0)
	{
		CStringA errorstring;
		errorstring.Format("Error sending the packets on INTERFACE %d!\r\n%s", id, pcap_geterr(handle));
		theApp.getSoftwareRouterDialog()->MessageBox(CString(errorstring), _T("Error"), MB_ICONERROR);
		return 1;
	}
	criticalSectionSend.Unlock();

	return 0;
}


Frame* Interface::getFrames(void)
{
	return frames;
}