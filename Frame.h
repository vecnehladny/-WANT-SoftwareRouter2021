#pragma once

#include <agents.h>
#define ETH2_HEADER_LENGTH 14

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
		BYTE dw;
	};
	union {
		BYTE mask;
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

class Frame {
public:
	Frame(void);
	~Frame(void);

private:
	unbounded_buffer<frameStructure*> buffer;
	u_char* frame;
	u_int length;
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
	WORD getLayer3(void);
	BYTE getLayer4(void);
	WORD getLayer4SourcePort(void);
	WORD getLayer4DestinationPort(void);
	ipAddressStructure getSourceIpAddress(void);
	ipAddressStructure getDestinationIpAddress(void);
	BYTE getTtl(void);
	void decTtl(void);
	int validateChecksum(void);

};
