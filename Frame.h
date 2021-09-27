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
	BYTE octets[4];
	BYTE mask;
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
