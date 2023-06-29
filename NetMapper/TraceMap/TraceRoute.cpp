#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IcmpLayer.h>
#include <NetworkUtils.h>

#include "TraceRoute.h"

#define DATA 0x2e65627574756f79
#define DATALEN 23

using namespace std;

const string message = "com/watch?v=dQw4w9WgXcQ";

void traceRoute(pcpp::PcapLiveDevice* dev, pcpp::EthLayer ethLayer, pcpp::IPv4Address dstIpAddr, int ttl, int id) {
	pcpp::IPv4Address srcIpAddr = dev->getIPv4Address();

	// Create new IPv4 layer
	pcpp::IPv4Layer ipLayer(srcIpAddr, dstIpAddr);
	ipLayer.getIPv4Header()->timeToLive = ttl;

	// Generate ping data
	uint8_t* data = new uint8_t[DATALEN];
	memset(data, 0, DATALEN);
	strcpy((char*)data, message.c_str());
	
	// Create ICMP layer
	pcpp::IcmpLayer icmpLayer;
	icmpLayer.setEchoRequestData(id, 0, DATA, data, DATALEN);

	// Create packet
	pcpp::Packet packet(100);
	packet.addLayer(&ethLayer);
	packet.addLayer(&ipLayer);
	packet.addLayer(&icmpLayer);

	packet.computeCalculateFields();

	dev->sendPacket(&packet);
}
