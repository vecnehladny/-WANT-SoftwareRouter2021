#pragma once

#include <agents.h>
#define ETH2_HEADER_LENGTH 14
#define ARP_LENGTH 28

using namespace Concurrency;

enum FRAME_TYPE {
	ETH2,
	RAW,
	SNAP,
	LLC
};

struct ipAddressStructure {
	union {

		BYTE octets[4];
		DWORD dw = 0;
	};
	union {
		BYTE mask = 0x00;
		BYTE hasNextHop;
	};
};

struct frameStructure {
	u_int length;
	u_char* data;
};

struct macAddressStructure {
	BYTE section[6];
};

struct ripResponseStructure {
	ipAddressStructure prefix;
	BYTE metric;
};

class Frame {
public:
	Frame(void);
	Frame(u_int frameLength);
	~Frame(void);

private:
	unbounded_buffer<frameStructure*> buffer;
	u_char* frame;
	u_int length;
	CArray<ripResponseStructure> routeList;
	WORD calculateChecksum(int count, u_char* address);

public:
	void addFrame(u_int length, const u_char* data);
	void getFrame(void);
	macAddressStructure getSourceMacAddress(void);
	void setSourceMacAddress(macAddressStructure mac);
	macAddressStructure getDestinationMacAddress(void);
	void setDestinationMacAddress(macAddressStructure mac);
	u_char* getData(void);
	u_int getLength(void);
	WORD getLayer3Type(void);
	BYTE getLayer4Type(void);
	WORD getLayer4SourcePort(void);
	WORD getLayer4DestinationPort(void);
	ipAddressStructure getSourceIpAddress(void);
	ipAddressStructure getDestinationIpAddress(void);
	BYTE getTTL(void);
	void decreaseTTL(void);
	int validateChecksum(void);
	void clear(void);
	int isRequestArp(void);
	int isReplyArp(void);
	void generateArpRequest(macAddressStructure senderMac, ipAddressStructure senderIp, ipAddressStructure targetIp);
	void generateArpReply(macAddressStructure senderMac, ipAddressStructure senderIp);
	macAddressStructure getArpSenderMac(void);
	ipAddressStructure getArpSenderIp(void);
	ipAddressStructure getArpTargetIp(void);
	int isIcmpEchoRequest(void);
	void generateIcmpEchoReply(ipAddressStructure localIp);
	void generateTtlExceeded(Frame* old, ipAddressStructure localIp, WORD IPHdrID);
	void fillIcmpChecksum(void);
	int isIcmpChecksumValid(void);
	void setLayer3Type(WORD type);
	BYTE getUpperByte(WORD number);
	BYTE getLowerByte(WORD number);
	void setSourceIpAddress(ipAddressStructure ip);
	void setDestinationIpAddress(ipAddressStructure ip);
	WORD merge(BYTE upper, BYTE lower);
	void fillTcpChecksum(void);
	void fillIpChecksum(void);
	FRAME_TYPE getType(void);
	int isMulticast(void);
	int isRipMessage(void);
	int generateRawRipPacket(ipAddressStructure localIp, ipAddressStructure* destinationIp, int dataLength);
	void generateRipRequest(ipAddressStructure localIp);
	void generateRipResponse(ipAddressStructure localIp, ipAddressStructure* destinationIp = NULL);
	void addRipRoute(ipAddressStructure prefix, BYTE metric);
	int getRipRouteCount(void);
	CArray<ripResponseStructure>& getRipRoutesFromPacket(void);
	void fillUdpChecksum(void);
	void setLayer4SourcePort(WORD port);
	void setLayer4DestinationPort(WORD port);
	WORD getIcmpId(void);
	void setIcmpId(WORD id);
};
