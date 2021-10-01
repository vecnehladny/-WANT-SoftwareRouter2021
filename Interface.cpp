#include "pch.h"
#include "Interface.h"


Interface::Interface(int id)
	: macAddress(_TEXT("unknown"))
	, id(id)
	, enabled(FALSE)
	, ipAddressIsSet(FALSE)
{
	int i;

	for (i = 0; i < 6; i++) macAddressStruct.b[i] = 0x00;
	for (i = 0; i < 4; i++) macAddressStruct.b[i] = 0x00;
	ipAddressStruct.mask = 0;
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
	return enabled;
}

BOOL Interface::isIpAddressSet()
{
	return ipAddressIsSet;
}

void Interface::enable()
{
	this->enabled = TRUE;
}

void Interface::disable() 
{
	this->enabled = FALSE;
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
		this->macAddressStruct.b[i] = macAddress[i];
	}
}

CString Interface::getIpAddress()
{
	CString toReturn;

	toReturn.Format(
		_T("%u.%u.%u.%u/%u"),
		ipAddressStruct.octets[3],
		ipAddressStruct.octets[2],
		ipAddressStruct.octets[1],
		ipAddressStruct.octets[0],
		ipAddressStruct.mask);

	return toReturn;
}

ipAddressStructure Interface::getIpAddressStruct()
{
	return ipAddressStruct;
}

void Interface::setIpAddress(BYTE octet1, BYTE octet2, BYTE octet3, BYTE octet4, BYTE mask){
	ipAddressStruct.octets[0] = octet1;
	ipAddressStruct.octets[1] = octet2;
	ipAddressStruct.octets[2] = octet3;
	ipAddressStruct.octets[3] = octet4;
	ipAddressStruct.mask = mask;

	ipAddressIsSet = TRUE;
}

BYTE Interface::getMask()
{
	return ipAddressStruct.mask;
}

void Interface::setIpAddress(ipAddressStructure newIpAddressStruct) {
	ipAddressStruct = newIpAddressStruct;

	ipAddressIsSet = TRUE;
}

CString Interface::getPrefix(void)
{
	ipAddressStructure prefix = getPrefixStruct();
	CString prefixString;

	prefixString.Format(
		_TEXT("%u.%u.%u.%u/%u"),
		prefix.octets[3], 
		prefix.octets[2],
		prefix.octets[1], 
		prefix.octets[0],
		prefix.mask);

	return prefixString;
}


ipAddressStructure Interface::getPrefixStruct(void)
{
	ipAddressStructure prefix;

	prefix.dw = (ipAddressStruct.dw >> (32 - ipAddressStruct.mask)) << (32 - ipAddressStruct.mask);
	prefix.mask = ipAddressStruct.mask;

	return prefix;
}