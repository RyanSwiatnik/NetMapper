#pragma once

#include <mutex>
#include <vector>

class IpIterator {
	uint8_t ipStart[4];
	uint8_t ipCurrent[4];
	uint8_t ipEnd[4];
	std::mutex iteratorLock;
	int iterateIndex = -1;
	bool iteratableIndex[4];
	bool finished = false;
public:
	IpIterator(std::vector<uint8_t> ipStart, std::vector<uint8_t> ipEnd);
	pcpp::IPv4Address iterate();
};
