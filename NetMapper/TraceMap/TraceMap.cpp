#include <IPv4Layer.h>
#include <EthLayer.h>
#include <NetworkUtils.h>
#include <PcapLiveDeviceList.h>

#include "TraceMap.h"
#include "Node.h"
#include "NetMap.h"
#include "ThreadManager.h"

using namespace std;

string toIpString(uint32_t ip) {
	pcpp::IPv4Address Ipv4Address(ip);

	return Ipv4Address.toString();
}

void exportNetJsonGraph(NetMap map) {
	ofstream out("netjsonmap.json");
	unordered_map<uint32_t, Node>* nodes = map.getNodes();

	out << "{" << endl;
	out << "\"type\": \"NetworkGraph\"," << endl;
	out << "\"label\" : \"Internet\"," << endl;
	out << "\"nodes\" : [" << endl;

	string linksString = "";
	for (auto node = nodes->begin(); node != nodes->end(); node++) {
		unordered_set<uint32_t>* links = node->second.getLinks();

		if (node != nodes->begin()) {
			out << "," << endl;
			if (links->size() != 0) {
				linksString += ",\n";
			}
		}
		out << "{\"id\": \"" << toIpString(node->first) << "\", \"name\" : \"" << node->second.getDomainName() << "\" }";

		for (auto link = links->begin(); link != links->end(); link++) {
			if (link != links->begin()) {
				linksString += ",\n";
			}
			linksString += "{\"source\": \"" + toIpString(node->first) + "\", \"target\" : \"" + toIpString(*link) + "\"}";
		}
	}

	out << endl << "]," << endl;
	out << "\"links\": [" << endl;
	out << linksString;
	out << endl << "]" << endl;
	out << "}" << endl;

	out.close();
}

void TraceMap(int threadCount, pcpp::IPv4Address srcIpAddr, vector<uint8_t> dstIpStart, vector<uint8_t> dstIpEnd)
{
	// Identify and configure device.
	pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(srcIpAddr);
	pcpp::PcapLiveDevice::DeviceConfiguration deviceConfig(pcpp::PcapLiveDevice::DeviceMode::Normal, pcpp::PcapLiveDevice::PcapDirection::PCPP_IN);
	pcpp::ProtoFilter recieveFilter(pcpp::ICMP);

	// Get the MAC address of the interface
	pcpp::MacAddress srcMacAddr = dev->getMacAddress();
	double arpResTO = 0;
	pcpp::MacAddress dstMacAddr = pcpp::NetworkUtils::getInstance().getMacAddress(dev->getDefaultGateway(), dev, arpResTO, srcMacAddr, srcIpAddr, 10);

	// Create static IP layers
	pcpp::EthLayer ethLayer(srcMacAddr, dstMacAddr);

	dev->open(deviceConfig);
	dev->setFilter(recieveFilter);

	NetMap map;
	ThreadManager threadManager(dev, &ethLayer, dstIpStart, dstIpEnd, threadCount, &map);

	exportNetJsonGraph(map);
}
