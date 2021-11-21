#include "pch.h"
#include "ArpTable.h"
#include "SoftwareRouter.h"

ArpTable::ArpTable()
{
}


ArpTable::~ArpTable()
{
}

void ArpTable::initArpTable(void)
{
	arpStructure toAdd;

	toAdd.ipAddress.octets[3] = 224;
	toAdd.ipAddress.octets[2] = 0;
	toAdd.ipAddress.octets[1] = 0;
	toAdd.ipAddress.octets[0] = 9;
	toAdd.macAddress.section[0] = 0x01;
	toAdd.macAddress.section[1] = 0x00;
	toAdd.macAddress.section[2] = 0x5e;
	toAdd.macAddress.section[3] = 0x00;
	toAdd.macAddress.section[4] = 0x00;
	toAdd.macAddress.section[5] = 0x09;
	toAdd.i = NULL;
	arps.Add(toAdd);
	CsoftwareRouterDlg* softwareRouterDialog = theApp.getSoftwareRouterDialog();
	softwareRouterDialog->addArp(0, toAdd);

	for (int i = 0; i < 4; i++) {
		toAdd.ipAddress.octets[i] = 255;
	}
	for (int i = 0; i < 6; i++) {
		toAdd.macAddress.section[i] = 0xFF;
	}
	arps.Add(toAdd);
	softwareRouterDialog->addArp(0, toAdd);
}

int ArpTable::getMacAddress(ipAddressStructure ipAddress, Interface* intface, macAddressStructure* result)
{
	arpCriticalSection.Lock();
	for (int i = 0; i < arps.GetCount(); i++)
	{
		if (areIpAddressesEqual(ipAddress, arps[i].ipAddress) == 0)
		{
			*result = arps[i].macAddress;
			arpCriticalSection.Unlock();
			return 0;
		}
	}
	arpCriticalSection.Unlock();
	sendArpRequest(ipAddress, intface);

	return 1;
}

void ArpTable::sendArpRequest(ipAddressStructure destinationIp, Interface* intface)
{
	Frame toSend;

	toSend.generateArpRequest(intface->getMacAddressStruct(), intface->getIpAddressStruct(), destinationIp);
	intface->sendFrame(&toSend);
}

void ArpTable::replyToArpRequest(Frame* buffer, Interface* inInterface)
{
	arpStructure toAdd;
	ipAddressStructure arpTargetIp = buffer->getArpTargetIp();
	Interface* foundInterface;
	CSingleLock lock(&arpCriticalSection);

	toAdd.ipAddress = buffer->getArpSenderIp();
	toAdd.macAddress = buffer->getArpSenderMac();
	toAdd.i = inInterface;

	if ((inInterface->isIpLocal(arpTargetIp))
		|| (((foundInterface = theApp.getRoutingTable()->doLookup(arpTargetIp)) != NULL) && (foundInterface != inInterface)))
	{
		buffer->generateArpReply(inInterface->getMacAddressStruct(), arpTargetIp);
		inInterface->sendFrame(buffer);
	}

	if (inInterface->isIpInLocalNetwork(toAdd.ipAddress)) {
		lock.Lock();
		for (int i = 0; i < arps.GetCount(); i++) {
			if (areIpAddressesEqual(toAdd.ipAddress, arps[i].ipAddress) == 0) {
				return;
			}
		}

		arps.InsertAt(0, toAdd);
		theApp.getSoftwareRouterDialog()->addArp(0, toAdd);
	}
}

void ArpTable::proccessArpReply(Frame* buffer, Interface* inInterface)
{
	arpStructure toAdd;
	CSingleLock lock(&arpCriticalSection);

	if (!inInterface->isIpLocal(buffer->getArpTargetIp())) {
		return;
	}

	toAdd.ipAddress = buffer->getArpSenderIp();
	toAdd.macAddress = buffer->getArpSenderMac();
	toAdd.i = inInterface;

	lock.Lock();
	for (int i = 0; i < arps.GetCount(); i++) {
		if (areIpAddressesEqual(toAdd.ipAddress, arps[i].ipAddress) == 0) {
			return;
		}
	}

	arps.InsertAt(0, toAdd);
	theApp.getSoftwareRouterDialog()->addArp(0, toAdd);
}

void ArpTable::addBroadcastOfInterface(Interface* intface)
{
	arpStructure toAdd;

	toAdd.ipAddress = intface->getBroadcastIPAddress();
	for (int i = 0; i < 6; i++) {
		toAdd.macAddress.section[i] = 0xFF;
	}
	toAdd.i = intface;

	arpCriticalSection.Lock();
	arps.InsertAt(0, toAdd);
	theApp.getSoftwareRouterDialog()->addArp(0, toAdd);
	arpCriticalSection.Unlock();
}

void ArpTable::clearArpTable(void)
{
	arpCriticalSection.Lock();
	for (int i = 0; i < arps.GetCount();) {
		if ((arps[i].i != NULL) && (!theApp.isBroadcast(arps[i].macAddress))) {
			arps.RemoveAt(i);
			theApp.getSoftwareRouterDialog()->removeArp(i);
		}
		else {
			i++;
		}
	}
	arpCriticalSection.Unlock();
}

void ArpTable::clearArpTable(Interface* intface)
{
	arpCriticalSection.Lock();
	for (int i = 0; i < arps.GetCount();) {
		if (arps[i].i == intface) {
			arps.RemoveAt(i);
			theApp.getSoftwareRouterDialog()->removeArp(i);
		}
		else {
			i++;
		}
	}
	arpCriticalSection.Unlock();
}

int ArpTable::areIpAddressesEqual(ipAddressStructure& ipAddr1, ipAddressStructure& ipAddr2)
{
	if (ipAddr1.dw != ipAddr2.dw) {
		return 1;
	}
	else {
		return 0;
	}
}
