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

struct frame {
	u_int length;
	u_char* data;
};

class Frame {
public:
	Frame(void);
	~Frame(void);

};
