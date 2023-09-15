#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IcmpLayer.h>
#include <NetworkUtils.h>

#include <UdpLayer.h>
#include <DnsLayer.h>

#include "PacketFactory.h"

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

void reverseDns(pcpp::PcapLiveDevice* dev, pcpp::EthLayer ethLayer, pcpp::IPv4Address query) {
	pcpp::IPv4Address srcIpAddr = dev->getIPv4Address();
	pcpp::IPv4Address dnsServer = dev->getDefaultGateway();
	
	// Flip Ip for reverse DNS
	uint32_t queryInt = query.toInt();
	uint32_t flippedQueryInt = 
		((queryInt & 0xff000000) >> 24) |
		((queryInt & 0x00ff0000) >> 8) |
		((queryInt & 0x0000ff00) << 8) |
		((queryInt & 0x000000ff) << 24);
	pcpp::IPv4Address flippedQuery(flippedQueryInt);

	// Create new IPv4 layer
	pcpp::IPv4Layer ipLayer(srcIpAddr, dnsServer);
	ipLayer.getIPv4Header()->timeToLive = 255;

	// Create new UDP layer
	pcpp::UdpLayer udpLayer(52575, 53);

	//Create new DNS layer
	pcpp::DnsLayer dnsLayer;
	dnsLayer.addQuery(flippedQuery.toString() + ".in-addr.arpa", pcpp::DNS_TYPE_PTR, pcpp::DNS_CLASS_IN);
	dnsLayer.getDnsHeader()->transactionID = 69;
	dnsLayer.getDnsHeader()->recursionDesired = 1;

	// Create packet
	pcpp::Packet packet(100);
	packet.addLayer(&ethLayer);
	packet.addLayer(&ipLayer);
	packet.addLayer(&udpLayer);
	packet.addLayer(&dnsLayer);

	packet.computeCalculateFields();

	dev->sendPacket(&packet);
}
