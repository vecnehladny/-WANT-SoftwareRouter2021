#include "pch.h"
#include "NatTable.h"
#include "softwareRouter.h"

#define ICMP_ID 1
#define TCP_ID 6
#define UDP_ID 17


NatTable::NatTable(void) : 
	poolSet(FALSE),
	dynamicNatEnabled(FALSE),
	patEnabled(FALSE)
{
	reservation.InitHashTable(717);
	icmpIdReservation.InitHashTable(717);
	setTimeout(5000);
	startTimer();
}


NatTable::~NatTable(void)
{
}


int NatTable::translate(Frame* buffer, Interface* sourceInterface)
{
	int i;
	NAT_MODE interfaceNatMode = sourceInterface->getNATmode();
	ipAddressStructure srcIP, destIP;
	BYTE prot;
	WORD srcPort = 0, destPort = 0, icmpId = 0;
	BOOL insideDone = FALSE, outsideDone = FALSE;

	if (interfaceNatMode == DISABLED) return 0;

	srcIP = buffer->getSourceIpAddress();
	destIP = buffer->getDestinationIpAddress();
	if ((interfaceNatMode == INSIDE) && (sourceInterface->isIpInLocalNetwork(destIP))) return 0;
	prot = buffer->getLayer4Type();
	if ((prot == TCP_ID) || (prot == UDP_ID))
	{
		srcPort = buffer->getLayer4SourcePort();
		destPort = buffer->getLayer4DestinationPort();
	}
	else if (prot == ICMP_ID) icmpId = buffer->getIcmpId();

	criticalSectionTable.Lock();
	for (i = 0; i < translations.GetCount(); i++)
	{
		if ((!insideDone) && (translations[i].mode == INSIDE))   // nat mode INSIDE
		{
			if (interfaceNatMode == INSIDE)   // interface mode INSIDE
			{
				if ((translations[i].type != PAT) && (translations[i].local.dw == srcIP.dw))
				{
					if (translations[i].isPortForward)
					{
						if ((translations[i].layer4Protocol == prot) && (translations[i].localPort == srcPort))
						{
							buffer->setSourceIpAddress(translations[i].global);
							buffer->setLayer4SourcePort(translations[i].globalPort);
							if (prot == TCP_ID) buffer->fillIcmpChecksum();
							else buffer->fillUdpChecksum();
							insideDone = TRUE;
						}
					}
					else
					{
						buffer->setSourceIpAddress(translations[i].global);
						resetTranslationTimeout(i);
						insideDone = TRUE;
					}
				}
				if ((translations[i].type == PAT) && (translations[i].local.dw == srcIP.dw) && (translations[i].layer4Protocol == prot))
				{
					if (((prot == TCP_ID) || (prot == UDP_ID)) && (translations[i].localPort == srcPort))
					{
						buffer->setSourceIpAddress(translations[i].global);
						buffer->setLayer4SourcePort(translations[i].globalPort);
						if (prot == TCP_ID) buffer->fillTcpChecksum();
						else buffer->fillUdpChecksum();
						resetTranslationTimeout(i);
						insideDone = TRUE;
					}
					if ((prot == ICMP_ID) && (translations[i].localIcmpId == icmpId))
					{
						buffer->setSourceIpAddress(translations[i].global);
						buffer->setIcmpId(translations[i].globalIcmpId);
						buffer->fillIcmpChecksum();
						resetTranslationTimeout(i);
						insideDone = TRUE;
					}
				}
			}
			else   // interface mode OUTSIDE
			{
				if ((translations[i].type != PAT) && (translations[i].global.dw == destIP.dw))
				{
					if (translations[i].isPortForward)
					{
						if ((translations[i].layer4Protocol == prot) && (translations[i].globalPort == destPort))
						{
							buffer->setDestinationIpAddress(translations[i].local);
							buffer->setLayer4DestinationPort(translations[i].localPort);
							if (prot == TCP_ID) buffer->fillTcpChecksum();
							else buffer->fillUdpChecksum();
							outsideDone = TRUE;
						}
					}
					else
					{
						buffer->setDestinationIpAddress(translations[i].local);
						resetTranslationTimeout(i);
						outsideDone = TRUE;
					}
				}
				if ((translations[i].type == PAT) && (translations[i].global.dw == destIP.dw) && (translations[i].layer4Protocol == prot))
				{
					if (((prot == TCP_ID) || (prot == UDP_ID)) && (translations[i].globalPort == destPort))
					{
						buffer->setDestinationIpAddress(translations[i].local);
						buffer->setLayer4DestinationPort(translations[i].localPort);
						if (prot == TCP_ID) buffer->fillTcpChecksum();
						else buffer->fillUdpChecksum();
						resetTranslationTimeout(i);
						insideDone = TRUE;
					}
					if ((prot == ICMP_ID) && (translations[i].globalIcmpId == icmpId))
					{
						buffer->setDestinationIpAddress(translations[i].local);
						buffer->setIcmpId(translations[i].localIcmpId);
						buffer->fillIcmpChecksum();
						resetTranslationTimeout(i);
						insideDone = TRUE;
					}
				}
			}
		}

		if ((!outsideDone) && (translations[i].mode == OUTSIDE))   // NAT mode OUTSIDE
		{
			if (interfaceNatMode == INSIDE)   // interface mode INSIDE
			{
				if (translations[i].local.dw == destIP.dw)
				{
					if (translations[i].isPortForward)
					{
						if ((translations[i].layer4Protocol == prot) && (translations[i].localPort == destPort))
						{
							buffer->setDestinationIpAddress(translations[i].global);
							buffer->setLayer4SourcePort(translations[i].globalPort);
							if (prot == TCP_ID) buffer->fillTcpChecksum();
							else buffer->fillUdpChecksum();
							insideDone = TRUE;
						}
					}
					else
					{
						buffer->setDestinationIpAddress(translations[i].global);
						insideDone = TRUE;
					}
				}
			}
			else   // interface mode OUTSIDE
			{
				if (translations[i].global.dw == srcIP.dw)
				{
					if (translations[i].isPortForward)
					{
						if ((translations[i].layer4Protocol == prot) && (translations[i].globalPort == srcPort))
						{
							buffer->setSourceIpAddress(translations[i].local);
							buffer->setLayer4SourcePort(translations[i].localPort);
							if (prot == TCP_ID) buffer->fillTcpChecksum();
							else buffer->fillUdpChecksum();
							outsideDone = TRUE;
						}
					}
					else
					{
						buffer->setSourceIpAddress(translations[i].local);
						outsideDone = TRUE;
					}
				}
			}
		}
	}
	criticalSectionTable.Unlock();

	if ((!insideDone) && (interfaceNatMode == INSIDE))
	{
		if ((isDynamicNatEnabled()) && (createDynamicEntry(buffer) == 0)) return 0;
		if ((isPatEnabled()) && (createOverload(buffer, sourceInterface) == 0)) return 0;
		return 1;
	}

	return 0;
}


void NatTable::addTranslation(translationStructure newRule)
{
	int i, count;
	CSingleLock lock(&criticalSectionTable);

	lock.Lock();
	count = translations.GetCount();
	if (translations.IsEmpty())
	{
		translations.Add(newRule);
		theApp.getSoftwareRouterDialog()->addTranslation(0, newRule);
		return;
	}

	for (i = 0; i < count; i++)
	{
		if (newRule.type == STATICNAT)
		{
			if ((!newRule.isPortForward) && ((translations[i].global.dw == newRule.global.dw) || (translations[i].local.dw == newRule.local.dw))) return;
			if ((newRule.isPortForward) && (translations[i].layer4Protocol == newRule.layer4Protocol)
				&& (((translations[i].global.dw == newRule.global.dw) && (translations[i].globalPort == newRule.globalPort))
					|| ((translations[i].local.dw == newRule.local.dw) && (translations[i].localPort == newRule.localPort)))) return;
			if (translations[i].type != STATICNAT) break;
		}
		if ((newRule.type == DYNAMICNAT) && (translations[i].type == PAT)) break;
		if ((newRule.type == PAT) && (i > 0) && (translations[i - 1].type == PAT)
			&& (translations[i].layer4Protocol != newRule.layer4Protocol) && (translations[i - 1].layer4Protocol == newRule.layer4Protocol)) break;
	}

	if (i == count)
	{
		translations.Add(newRule);
		theApp.getSoftwareRouterDialog()->addTranslation(count, newRule);
	}
	else
	{
		translations.InsertAt(i, newRule);
		theApp.getSoftwareRouterDialog()->addTranslation(i, newRule);
	}
}


int NatTable::removeTranslation(int index)
{
	CSingleLock lock(&criticalSectionTable);

	lock.Lock();
	if (translations[index].type != STATICNAT) return 1;

	translations.RemoveAt(index);
	theApp.getSoftwareRouterDialog()->removeTranslation(index);
	return 0;
}


int NatTable::isGlobalIP(ipAddressStructure toCheck)
{
	int i;
	CSingleLock lock(&criticalSectionTable);

	lock.Lock();
	for (i = 0; i < translations.GetCount(); i++)
	{
		if (translations[i].global.dw == toCheck.dw) return 1;
	}

	return 0;
}


int NatTable::isPoolSet(void)
{
	return poolSet;
}


ipAddressStructure NatTable::getFirstIpAddressInPool(void)
{
	CSingleLock lock(&criticalSectionPool);

	lock.Lock();
	return poolStart;
}


ipAddressStructure NatTable::getLastIpAddressInPool(void)
{
	CSingleLock lock(&criticalSectionPool);

	lock.Lock();
	return poolEnd;
}


void NatTable::setPool(ipAddressStructure start, ipAddressStructure end)
{
	criticalSectionPool.Lock();
	poolStart = start;
	poolEnd = end;
	poolSet = TRUE;
	theApp.getSoftwareRouterDialog()->editPool(poolStart, poolEnd);
	criticalSectionPool.Unlock();
}


void NatTable::resetTranslationTimeout(int index)
{
	criticalSectionTime.Lock();
	translations[index].timeout = 60;
	criticalSectionTime.Unlock();
}


void NatTable::Timeout()
{
	int i;

	criticalSectionTable.Lock();
	criticalSectionTime.Lock();
	for (i = 0; i < translations.GetCount(); i++)
	{
		if (translations[i].type != STATICNAT)
		{
			translations[i].timeout -= 5;
			if (translations[i].timeout == 0)
			{
				if (translations[i].layer4Protocol == ICMP_ID)
				{
					criticalSectionIcmpId.Lock();
					icmpIdReservation.RemoveKey(translations[i].globalIcmpId);
					criticalSectionIcmpId.Unlock();
				}
				if (translations[i].type == DYNAMICNAT)
				{
					criticalSectionPool.Lock();
					reservation.RemoveKey((translations[i].global.dw << translations[i].global.mask) >> translations[i].global.mask);
					criticalSectionPool.Unlock();
				}
				translations.RemoveAt(i);
				theApp.getSoftwareRouterDialog()->removeTranslation(i);
				i--;
			}
			else theApp.getSoftwareRouterDialog()->updateTranslation(i, translations[i].timeout);
		}
	}
	criticalSectionTime.Unlock();
	criticalSectionTable.Unlock();
}


void NatTable::removeAllDynamic(void)
{
	int i;

	criticalSectionTable.Lock();
	for (i = 0; i < translations.GetCount(); i++)
	{
		if (translations[i].type != STATICNAT)
		{
			translations.RemoveAt(i);
			theApp.getSoftwareRouterDialog()->removeTranslation(i);
			i--;
		}
	}
	criticalSectionPool.Lock();
	reservation.RemoveAll();
	criticalSectionPool.Unlock();
	criticalSectionIcmpId.Lock();
	icmpIdReservation.RemoveAll();
	criticalSectionIcmpId.Unlock();
	criticalSectionTable.Unlock();
}


int NatTable::isDynamicNatEnabled(void)
{
	BOOL result;

	criticalSectionNat.Lock();
	result = dynamicNatEnabled;
	criticalSectionNat.Unlock();

	return result;
}


int NatTable::isPatEnabled(void)
{
	BOOL result;

	criticalSectionNat.Lock();
	result = patEnabled;
	criticalSectionNat.Unlock();

	return result;
}


void NatTable::enableDynamicNat()
{
	criticalSectionNat.Lock();
	dynamicNatEnabled = 1;
	criticalSectionNat.Unlock();
}


void NatTable::disableDynamicNat()
{
	criticalSectionNat.Lock();
	dynamicNatEnabled = 0;
	removeAllDynamic();
	criticalSectionNat.Unlock();
}


void NatTable::enablePat()
{
	criticalSectionNat.Lock();
	patEnabled = 1;
	criticalSectionNat.Unlock();
}


void NatTable::disablePat()
{
	criticalSectionNat.Lock();
	patEnabled = 0;
	removeAllDynamic();
	criticalSectionNat.Unlock();
}


int NatTable::createDynamicEntry(Frame* buffer)
{
	DWORD i;
	DWORD firstNum;
	DWORD lastNum;
	BOOL value;
	translationStructure newRule;
	CSingleLock lock(&criticalSectionPool);

	lock.Lock();
	firstNum = (poolStart.dw << poolStart.mask) >> poolStart.mask;
	lastNum = (poolEnd.dw << poolEnd.mask) >> poolEnd.mask;

	for (i = firstNum; i <= lastNum; i++) if (!reservation.Lookup(i, value)) break;
	if (i > lastNum) return 1;

	if (poolStart.mask == 0) newRule.global.dw = 0;
	else newRule.global.dw = (poolStart.dw >> (32 - poolStart.mask)) << (32 - poolStart.mask);

	newRule.global.dw |= i;
	newRule.global.mask = poolStart.mask;
	newRule.local = buffer->getSourceIpAddress();
	newRule.type = DYNAMICNAT;
	newRule.mode = INSIDE;
	newRule.isPortForward = 0;
	newRule.timeout = 60;

	reservation.SetAt(i, TRUE);
	lock.Unlock();
	addTranslation(newRule);
	buffer->setSourceIpAddress(newRule.global);

	return 0;
}


int NatTable::createOverload(Frame* buffer, Interface* sourceInterface)
{
	WORD i;
	WORD srcPort, srcId;
	BOOL value;
	Interface* outInteface;
	translationStructure newRule;
	CSingleLock lock(&criticalSectionIcmpId);

	if (sourceInterface->getId() == 1) outInteface = theApp.getInterface(2);
	else outInteface = theApp.getInterface(1);

	if (!outInteface->isEnabled()) return 1;

	newRule.layer4Protocol = buffer->getLayer4Type();
	newRule.global = outInteface->getIpAddressStruct();
	newRule.local = buffer->getSourceIpAddress();

	if (newRule.layer4Protocol == TCP_ID)
	{
		srcPort = buffer->getLayer4SourcePort();
		newRule.globalPort = srcPort;
		newRule.localPort = srcPort;
		buffer->setSourceIpAddress(newRule.global);
		buffer->fillTcpChecksum();
	}
	else if (newRule.layer4Protocol == UDP_ID)
	{
		srcPort = buffer->getLayer4SourcePort();
		newRule.globalPort = srcPort;
		newRule.localPort = srcPort;
		buffer->setSourceIpAddress(newRule.global);
		buffer->fillUdpChecksum();
	}
	else if (newRule.layer4Protocol == ICMP_ID)
	{
		srcId = buffer->getIcmpId();
		newRule.localIcmpId = srcId;

		lock.Lock();
		if (icmpIdReservation.Lookup(srcId, value))
		{
			for (i = 1; i <= 65530; i++) if (!icmpIdReservation.Lookup(i, value)) break;
			if (i > 65530) return 1;
		}
		else i = srcId;
		icmpIdReservation.SetAt(i, TRUE);
		lock.Unlock();

		newRule.globalIcmpId = i;
		buffer->setIcmpId(i);
		buffer->setSourceIpAddress(newRule.global);
		buffer->fillIcmpChecksum();
	}
	else return 1;

	newRule.type = PAT;
	newRule.mode = INSIDE;
	newRule.isPortForward = 0;
	newRule.timeout = 60;

	addTranslation(newRule);
	return 0;
}
