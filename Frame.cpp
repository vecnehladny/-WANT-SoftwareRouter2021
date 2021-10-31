#include "pch.h"
#include "Frame.h"


Frame::Frame(void) :
	frame(NULL),
	length(0)
{
}


Frame::Frame(u_int frameLength) 
{
	length = frameLength;
	frame = (u_char*)malloc(frameLength * sizeof(u_char));
}


Frame::~Frame(void)
{
	if (frame) {
		free(frame);
	}
	routeList.RemoveAll();
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


WORD Frame::getLayer3Type(void)
{
	WORD num = frame[12] << 8;
	num |= frame[13];
	return num;
}


BYTE Frame::getLayer4Type(void)
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


BYTE Frame::getTTL(void)
{
	return frame[22];
}


void Frame::decreaseTTL(void)
{
	if (frame[22]) {
		frame[22] -= 1;
	}
}


WORD Frame::calculateChecksum(int count, u_char* address)
{
	register DWORD sum = 0;
	WORD* addressWord = (WORD*)address;

	while (count > 1){
		sum += *addressWord++;
		count -= 2;
	}

	if (count > 0) {
		sum += *(u_char*)addressWord;
	}

	while (sum >> 16) {
		sum = (sum & 0xffff) + (sum >> 16);
	}

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

void Frame::clear(void)
{
	if (frame) {
		free(frame);
	}

	frame = NULL;
	length = 0;
	routeList.RemoveAll();
}

void Frame::setLayer3Type(WORD type)
{
	frame[12] = getUpperByte(type);
	frame[13] = getLowerByte(type);
}

BYTE Frame::getUpperByte(WORD number)
{
	return number >> 8;
}


BYTE Frame::getLowerByte(WORD number)
{
	return number & 0xFF;
}

void Frame::setSourceIpAddress(ipAddressStructure ip)
{
	for (int i = 26; i < 30; i++) {
		frame[i] = ip.octets[3 - i + 26];
	}
}

void Frame::setDestinationIpAddress(ipAddressStructure ip)
{
	for (int i = 30; i < 34; i++) {
		frame[i] = ip.octets[3 - i + 30];
	}
}

WORD Frame::merge(BYTE upper, BYTE lower)
{
	WORD num = upper << 8;
	num |= lower;
	return num;
}

int Frame::isRequestArp(void)
{
	if (merge(frame[20], frame[21]) == 1) {
		return 1;
	}
	else {
		return 0;
	}
}


int Frame::isReplyArp(void)
{
	if (merge(frame[20], frame[21]) == 2) {
		return 1;
	}
	else {
		return 0;
	}
}


void Frame::generateArpRequest(macAddressStructure senderMac, ipAddressStructure senderIp, ipAddressStructure targetIp)
{
	length = ETH2_HEADER_LENGTH + ARP_LENGTH;
	frame = (u_char*)malloc(length * sizeof(u_char));
	
	for (int i = 0; i < 6; i++) {
		frame[i] = 0xFF;
	}

	setLayer3Type(0x0806);

	frame[14] = 0;
	frame[15] = 1;
	frame[16] = 0x08;
	frame[17] = 0;
	frame[18] = 0x06;
	frame[19] = 0x04;
	frame[20] = 0;
	frame[21] = 1;

	for (int i = 22; i < 28; i++) {
		frame[i] = senderMac.section[i - 22];
	}
	for (int i = 28; i < 32; i++) {
		frame[i] = senderIp.octets[3 - i + 28];
	}
	for (int i = 32; i < 38; i++) {
		frame[i] = 0;
	}
	for (int i = 38; i < 42; i++) {
		frame[i] = targetIp.octets[3 - i + 38];
	}
}


void Frame::generateArpReply(macAddressStructure senderMac, ipAddressStructure senderIp)
{
	setDestinationMacAddress(getArpSenderMac());
	frame[21] = 2;

	for (int i = 22; i < 32; i++) {
		frame[i + 10] = frame[i];
	}
	for (int i = 22; i < 28; i++) {
		frame[i] = senderMac.section[i - 22];
	}
	for (int i = 28; i < 32; i++) {
		frame[i] = senderIp.octets[3 - i + 28];
	}
}


macAddressStructure Frame::getArpSenderMac(void)
{
	macAddressStructure sender;

	for (int i = 22; i < 28; i++) {
		sender.section[i - 22] = frame[i];
	}

	return sender;
}


ipAddressStructure Frame::getArpSenderIp(void)
{
	ipAddressStructure sender;

	for (int i = 28; i < 32; i++) {
		sender.octets[3 - i + 28] = frame[i];
	}

	return sender;
}


ipAddressStructure Frame::getArpTargetIp(void)
{
	ipAddressStructure target;

	for (int i = 38; i < 42; i++) {
		target.octets[3 - i + 38] = frame[i];
	}

	return target;
}


int Frame::isIcmpEchoRequest(void)
{
	if ((getLayer4Type() == 1) && (frame[34] == 8)) {
		return 1;
	}
	else {
		return 0;
	}
}


void Frame::generateIcmpEchoReply(ipAddressStructure localIp)
{
	for (int i = 0; i < 6; i++) {
		frame[i] = frame[i + 6];
	}

	for (int i = 26; i < 30; i++){
		frame[i + 4] = frame[i];
		frame[i] = localIp.octets[3 - i + 26];
	}
	SetIpTTL(255);
	fillIpChecksum();

	frame[34] = 0;
	fillIcmpChecksum();
}


void Frame::generateTtlExceeded(Frame* old, ipAddressStructure localIp, WORD ipHeader)
{
	WORD w;

	length = old->getLength() + 28;
	frame = (u_char*)malloc(length * sizeof(u_char));
	setDestinationMacAddress(old->getSourceMacAddress());
	setLayer3Type(0x0800);

	frame[14] = 0x45;
	frame[15] = 0xC0;
	w = length - ETH2_HEADER_LENGTH;
	frame[16] = getUpperByte(w);
	frame[17] = getLowerByte(w);
	frame[18] = getUpperByte(ipHeader);
	frame[19] = getLowerByte(ipHeader);
	frame[20] = 0;
	frame[21] = 0;
	frame[22] = 64;
	frame[23] = 1;
	setSourceIpAddress(localIp);
	setDestinationIpAddress(old->getSourceIpAddress());
	fillTcpChecksum();

	frame[34] = 11;
	frame[35] = 0;
	memcpy(frame + 42, old->getData() + ETH2_HEADER_LENGTH, old->getLength() - ETH2_HEADER_LENGTH);
	fillIcmpChecksum();
}


void Frame::fillIcmpChecksum(void)
{
	int ipHeadrLength = (frame[14] & 0x0F) * 4;
	WORD* chksum_ptr = (WORD*)(frame + ETH2_HEADER_LENGTH + ipHeadrLength + 2);

	*chksum_ptr = 0;
	*chksum_ptr = calculateChecksum(merge(frame[16], frame[17]) - ipHeadrLength, frame + ETH2_HEADER_LENGTH + ipHeadrLength);
}


int Frame::isIcmpChecksumValid(void)
{
	int ipHeadrLength = (frame[14] & 0x0F) * 4;

	if (calculateChecksum(merge(frame[16], frame[17]) - ipHeadrLength, frame + ETH2_HEADER_LENGTH + ipHeadrLength) == 0) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void Frame::fillTcpChecksum(void)
{
	int ipHeadrLength = (frame[14] & 0x0F) * 4;
	u_char* address = frame + ETH2_HEADER_LENGTH + ipHeadrLength;
	WORD* addressWord = (WORD*)address;
	WORD tcpLength = merge(frame[ETH2_HEADER_LENGTH + 2], frame[ETH2_HEADER_LENGTH + 3]) - ipHeadrLength;
	int count = tcpLength;
	WORD* chksum_ptr = addressWord + 8;
	register DWORD sum = 0;

	*chksum_ptr = 0;
	while (count > 1)
	{
		sum += *addressWord++;
		count -= 2;
	}

	if (count > 0) sum += *(u_char*)addressWord;

	addressWord = (WORD*)address;
	sum += 0x0600;
	sum += ((tcpLength & 0xFF) << 8) | (tcpLength >> 8);
	addressWord -= 4;
	sum += *addressWord++;
	sum += *addressWord++;
	sum += *addressWord++;
	sum += *addressWord;

	while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);

	*chksum_ptr = (WORD)(~sum);
}


void Frame::fillIpChecksum(void)
{
	WORD* checksumPointer = (WORD*)(frame + 24);

	*checksumPointer = 0;
	*checksumPointer = calculateChecksum((frame[14] & 0x0F) * 4, frame + ETH2_HEADER_LENGTH);
}


FRAME_TYPE Frame::getType(void) 
{
	if (frame[12] >= 0x06) return ETH2;
	else if ((frame[14] == 0xFF) && (frame[15] == 0xFF)) return RAW;
	else if ((frame[14] == 0xAA) && (frame[15] == 0xAA) && (frame[16] == 0x03)) return SNAP;
	else return LLC;
}

int Frame::isMulticast(void)
{
	if (frame[0] != 0x01) return 0;
	if (frame[1] != 0x00) return 0;
	if (frame[2] != 0x5E) return 0;
	if (frame[3] != 0x00) return 0;
	if (frame[4] != 0x00) return 0;
	if (frame[5] != 0x09) return 0;

	return 1;
}


int Frame::isRipMessage(void)
{
	int index;

	if (getLayer4Type() != 17) return 0;

	if ((getLayer4SourcePort() != 520) || (getLayer4DestinationPort() != 520)) return 0;

	index = ETH2_HEADER_LENGTH + ((frame[14] & 0x0F) * 4) + 8;

	if (frame[index + 1] != 2) return 0;

	if (frame[index] == 1) return 1;

	if (frame[index] == 2) return 2;

	return 0;
}


int Frame::generateRawRipPacket(ipAddressStructure localIp, ipAddressStructure* destinationIp, int dataLength)
{
	WORD w;

	length = ETH2_HEADER_LENGTH + 28 + dataLength;
	if (frame)
	{
		free(frame);
	}
	frame = (u_char*)malloc(length * sizeof(u_char));
	setLayer3Type(0x0800);

	frame[14] = 0x45;
	frame[15] = 0xC0;
	w = length - ETH2_HEADER_LENGTH;
	frame[16] = getUpperByte(w);
	frame[17] = getLowerByte(w);
	frame[18] = 0;
	frame[19] = 0;
	frame[20] = 0;
	frame[21] = 0;
	frame[22] = 2;
	frame[23] = 17;
	setSourceIpAddress(localIp);
	if (destinationIp)
	{
		setDestinationIpAddress(*destinationIp);
	}
	else
	{
		frame[30] = 224;
		frame[31] = 0;
		frame[32] = 0;
		frame[33] = 9;
	}
	fillIpChecksum();

	frame[34] = 2;
	frame[35] = 8;
	frame[36] = 2;
	frame[37] = 8;
	frame[38] = getUpperByte(dataLength + 8);
	frame[39] = getLowerByte(dataLength + 8);

	return ETH2_HEADER_LENGTH + 28;
}


void Frame::generateRipRequest(ipAddressStructure localIp)
{
	int index = generateRawRipPacket(localIp, NULL, 24);

	frame[index++] = 1;
	frame[index++] = 2;
	for (int i = 0; i < 21; i++)
	{
		frame[index++] = 0;
	}
	frame[index] = 0x10;
	fillUdpChecksum();
}


void Frame::generateRipResponse(ipAddressStructure localIp, ipAddressStructure* destinationIp)
{
	int index = generateRawRipPacket(localIp, destinationIp, (routeList.GetCount() * 20) + 4);
	DWORD maxBit = 1 << 31;
	ipAddressStructure mask;

	frame[index++] = 2;
	frame[index++] = 2;
	frame[index++] = 0;
	frame[index++] = 0;

	for (int j = 0; j < routeList.GetCount(); j++)
	{
		frame[index++] = 0;
		frame[index++] = 2;
		frame[index++] = 0;
		frame[index++] = 0;
		for (int i = 3; i >= 0; i--)
		{
			frame[index++] = routeList[j].prefix.octets[i];
		}

		mask.dw = 0;
		for (int i = 0; i < routeList[j].prefix.mask; i++)
		{
			mask.dw >>= 1;
			mask.dw |= maxBit;
		}
		for (int i = 3; i >= 0; i--) 
		{
			frame[index++] = mask.octets[i];
		}

		for (int i = 3; i >= 0; i--)
		{
			frame[index++] = 0;
		}
		frame[index++] = 0;
		frame[index++] = 0;
		frame[index++] = 0;
		frame[index++] = routeList[j].metric;
	}
	fillUdpChecksum();
}


void Frame::addRipRoute(ipAddressStructure prefix, BYTE metric)
{
	ripResponseStructure toAdd;

	toAdd.prefix = prefix;
	toAdd.metric = metric;
	if (toAdd.metric < 16)
	{
		toAdd.metric++;
	}
	routeList.Add(toAdd);
}


int Frame::getRipRouteCount(void)
{
	return routeList.GetCount();
}


CArray<ripResponseStructure>& Frame::getRipRoutesFromPacket(void)
{
	int ipHeaderLength = (frame[14] & 0x0F) * 4;
	int count = (merge(frame[ETH2_HEADER_LENGTH + ipHeaderLength + 4], frame[ETH2_HEADER_LENGTH + ipHeaderLength + 5]) - 12) / 20;
	int index = ETH2_HEADER_LENGTH + ipHeaderLength + 12;
	ripResponseStructure route;
	ipAddressStructure mask;

	while (count)
	{
		index += 4;
		for (int i = 3; i >= 0; i--)
		{
			route.prefix.octets[i] = frame[index++];
		}
		for (int i = 3; i >= 0; i--)
		{
			mask.octets[i] = frame[index++];
		}
		route.prefix.mask = 0;
		while (mask.dw)
		{
			route.prefix.mask++;
			mask.dw <<= 1;
		}
		index += 7;
		route.metric = frame[index++];
		routeList.Add(route);
		count--;
	}

	return routeList;
}


void Frame::fillUdpChecksum(void)
{
	int ipHeaderLength = (frame[14] & 0x0F) * 4;
	u_char* address = frame + ETH2_HEADER_LENGTH + ipHeaderLength;
	WORD* addressWord = (WORD*)address;
	int count = merge(*(address + 4), *(address + 5));
	WORD* checksumPointer = addressWord + 3;
	register DWORD sum = 0;

	*checksumPointer = 0;
	while (count > 1)
	{
		sum += *addressWord++;
		count -= 2;
	}

	if (count > 0)
	{
		sum += *(u_char*)addressWord;
	}

	addressWord = (WORD*)address;
	sum += 0x1100;
	sum += *(addressWord + 2);
	addressWord -= 4;
	sum += *addressWord++;
	sum += *addressWord++;
	sum += *addressWord++;
	sum += *addressWord;

	while (sum >> 16)
	{
		sum = (sum & 0xffff) + (sum >> 16);
	}

	*checksumPointer = (WORD)(~sum);
}