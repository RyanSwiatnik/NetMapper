// NetMapper.cpp : Defines the entry point for the application.
//

#pragma comment(lib, "Ws2_32.lib")

#include <IPv4Layer.h>
#include "TCPScan.h"
#include "NetMapper.h"
#include "TraceMap/TraceMap.h"

using namespace std;

int main(int argc, char** argv) {
	// Default variables
	const int srcPort = 4294;
	int threadCount = 15;
	Mode mode = tcpScan;

	pcpp::IPv4Address srcIpAddr;
	vector<uint8_t> dstIpStart = { 0,0,0,0 };
	vector<uint8_t> dstIpEnd = { 255,255,255,255 };
	uint8_t mask = 0;
	uint16_t dstPort = 80;

	// Load command arguments
	for (int i = 1; i < argc; i += 2) {
		stringstream ss;
		switch (argv[i][1]) {
		case 's':
			srcIpAddr = (string)argv[i + 1];
			break;
		case 'd':
			ss = stringstream(argv[i + 1]);
			for (int i = 0; i < 4; i++) {
				std::string item;
				getline(ss, item, '.');
				dstIpStart[i] = stoi(item);
			}
			break;
		case 'D':
			ss = stringstream(argv[i + 1]);
			for (int i = 0; i < 4; i++) {
				std::string item;
				getline(ss, item, '.');
				dstIpEnd[i] = stoi(item);
			}
			break;
		case 'm':
			mask = stoi(argv[i + 1]);
			break;
		case 'p':
			dstPort = stoi(argv[i + 1]);
			break;
		case 't':
			threadCount = stoi(argv[i + 1]);
			break;
		case 'M':
			mode = traceMap;
			i--;
			break;
		case 'h':
			mode = none;
			cout << "Net Mapper help" << endl <<
				"Usage: netmapper [options]" << endl <<
				"   -s Network adapter IP address" << endl <<
				"   -d Destination IP range start (0.0.0.0)" << endl <<
				"   -D Destination IP range end (255.255.255.255)" << endl <<
				"   -m Destination IP mask" << endl <<
				"   -p Destination port (80)" << endl <<
				"   -M Start trace route map" << endl <<
				"   -t Thread count (15)" << endl;
			break;
		default:
			cout << "Invalid argument: " << argv[i] << endl;
		}
	}

	// Calculate IP range from IP mask.
	if (mask != 0) {
		uint32_t startIp = (
			dstIpStart[0] << 24 |
			dstIpStart[1] << 16 |
			dstIpStart[2] << 8 |
			dstIpStart[3]);

		uint32_t ipMask = INT32_MAX << (32 - mask);
		uint32_t endIp = startIp | ~(ipMask);
		startIp = startIp & ipMask;

		dstIpStart[0] = (startIp & 0xFF000000) >> 24;
		dstIpStart[1] = (startIp & 0x00FF0000) >> 16;
		dstIpStart[2] = (startIp & 0x0000FF00) >> 8;
		dstIpStart[3] = (startIp & 0x000000FF);

		dstIpEnd[0] = (endIp & 0xFF000000) >> 24;
		dstIpEnd[1] = (endIp & 0x00FF0000) >> 16;
		dstIpEnd[2] = (endIp & 0x0000FF00) >> 8;
		dstIpEnd[3] = (endIp & 0x000000FF);
	}

	switch (mode) {
	case tcpScan:
		TCPScan(srcPort, threadCount, srcIpAddr, dstIpStart, dstIpEnd, dstPort);
		break;
	case traceMap:
		TraceMap(threadCount, srcIpAddr, dstIpStart, dstIpEnd);
		break;
	}
}
