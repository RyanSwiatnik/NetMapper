// NetMapper.cpp : Defines the entry point for the application.
//

#pragma comment(lib, "Ws2_32.lib")

#include <IPv4Layer.h>
#include "TCPScan.h"
#include "NetMapper.h"
#include "TraceMap.h"

using namespace std;

int main(int argc, char** argv) {
	// Default variables
	const int srcPort = 4294;
	int threadCount = 15;
	Mode mode = tcpScan;

	pcpp::IPv4Address srcIpAddr;
	vector<uint8_t> dstIpStart = { 0,0,0,0 };
	vector<uint8_t> dstIpEnd = { 255,255,255,255 };
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
		case 'p':
			dstPort = stoi(argv[i + 1]);
			break;
		case 't':
			threadCount = stoi(argv[i + 1]);
			break;
		case 'm':
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
				"   -p Destination port (80)" << endl <<
				"   -m Start trace route map" << endl <<
				"   -t Thread count (15)" << endl;
			break;
		default:
			cout << "Invalid argument: " << argv[i] << endl;
		}
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
