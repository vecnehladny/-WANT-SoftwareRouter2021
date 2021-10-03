#include "pch.h"
#include "Frame.h"


Frame::Frame(void) :
	frame(NULL),
	length(0)
{
}


Frame::~Frame(void)
{
}

void Frame::addFrame(u_int length, const u_char* data_const)
{
	frameStructure* toAdd = (frameStructure*)malloc(sizeof(frameStructure));

	toAdd->length = length;
	toAdd->data = (u_char*)malloc(length);
	memcpy(toAdd->data, data_const, length);

	send(buffer, toAdd);
}


void Frame::getFrame(void)
{
	frameStructure* toExtract = receive(buffer);

	if (frame) free(frame);
	length = toExtract->length;
	frame = toExtract->data;

	free(toExtract);
}


macAddressStructure Frame::getSourceMacAddress(void)
{
	macAddressStructure toReturn;

	for (int i = 6; i < 12; i++) {
		toReturn.section[i - 6] = frame[i];
	} 
	return toReturn;
}


void Frame::setSourceMacAddress(macAddressStructure mac)
{
	for (int i = 6; i < 12; i++) {
		frame[i] = mac.section[i - 6];
	}
}


macAddressStructure Frame::getDestinationMacAddress(void)
{
	macAddressStructure toReturn;

	for (int i = 0; i < 6; i++) {
		toReturn.section[i] = frame[i];
	}
	return toReturn;
}


void Frame::setDestinationMacAddress(macAddressStructure mac)
{
	for (int i = 0; i < 6; i++) {
		frame[i] = mac.section[i];
	}
}


u_char* Frame::getData(void)
{
	return frame;
}


u_int Frame::getLength(void)
{
	return length;
}


WORD Frame::getLayer3(void)
{
	WORD num = frame[12] << 8;
	num |= frame[13];
	return num;
}


BYTE Frame::getLayer4(void)
{
	return frame[23];
}


WORD Frame::getLayer4SourcePort(void)
{
	WORD num = frame[ETH2_HEADER_LENGTH + (ETH2_HEADER_LENGTH)] << 8;
	num |= frame[ETH2_HEADER_LENGTH + ((frame[14] & 0x0F) * 4) + 1];
	return num;
}


WORD Frame::getLayer4DestinationPort(void)
{
	WORD num = frame[ETH2_HEADER_LENGTH + (ETH2_HEADER_LENGTH) + 2] << 8;
	num |= frame[ETH2_HEADER_LENGTH + ((frame[14] & 0x0F) * 4) + 3];
	return num;
}


ipAddressStructure Frame::getSourceIpAddress(void)
{
	ipAddressStructure toReturn;
	
	for (int i = 26; i < 30; i++) {
		toReturn.octets[3 - i + 26] = frame[i];
	}
	return toReturn;
}


ipAddressStructure Frame::getDestinationIpAddress(void)
{
	ipAddressStructure toReturn;

	for (int i = 30; i < 34; i++) {
		toReturn.octets[3 - i + 30] = frame[i];
	}
	return toReturn;
}


BYTE Frame::getTtl(void)
{
	return frame[22];
}


void Frame::decTtl(void)
{
	frame[22] -= 1;
}


WORD Frame::calculateChecksum(int count, u_char* address)
{
	register DWORD sum = 0;

	while (count > 1)
	{
		sum += *((WORD*)address++);
		count -= 2;
	}

	if (count > 0) sum += *address;

	while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);

	return (WORD)(~sum);
}


int Frame::validateChecksum(void)
{
	if (calculateChecksum((frame[14] & 0x0F) * 4, frame + ETH2_HEADER_LENGTH) == 0) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}