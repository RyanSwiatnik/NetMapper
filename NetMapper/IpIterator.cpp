#include <IPv4Layer.h>
#include "IpIterator.h"

using namespace std;

IpIterator::IpIterator(vector<uint8_t> ipStart, vector<uint8_t> ipEnd) {
	copy(ipStart.begin(), ipStart.end(), IpIterator::ipStart);
	copy(ipEnd.begin(), ipEnd.end(), IpIterator::ipEnd);
	copy(ipStart.begin(), ipStart.end(), begin(ipCurrent));

	for (int i = 0; i < 4; i++) {
		if (IpIterator::ipStart[i] != IpIterator::ipEnd[i]) {
			iteratableIndex[i] = true;
			if (iterateIndex == -1) {
				iterateIndex = i;
			}
		}
		else {
			iteratableIndex[i] = false;
		}
	}
}

pcpp::IPv4Address IpIterator::iterate()
{
	if (finished) {
		return pcpp::IPv4Address();
	}
	int i = iterateIndex;

	iteratorLock.lock();
	pcpp::IPv4Address dstIp = ipCurrent;

	while (ipCurrent[i] == ipEnd[i]) {
		if (i > 2) {
			finished = true;
			iteratorLock.unlock();
			return dstIp;
		}
		else {
			ipCurrent[i] = ipStart[i];
			i++;
			if (iteratableIndex[i]) {
				ipCurrent[i]++;
			}
		}
	}
	if (i == iterateIndex) {
		ipCurrent[iterateIndex]++;
	}

	iteratorLock.unlock();
	return dstIp;
}
