#include <ws2tcpip.h>

#include <EthLayer.h>
#include <IPv4Layer.h>
#include <TcpLayer.h>

#include <NetworkUtils.h>
#include <PcapLiveDeviceList.h>

#include "IpIterator.h"
#include "TCPScan.h"

using namespace std;

static ofstream fileOutput;

bool rawTcpPing(pcpp::PcapLiveDevice* dev, pcpp::EthLayer ethLayer, pcpp::IPv4Address* dstIpAddr, pcpp::TcpLayer tcpLayer) {
	pcpp::IPv4Address srcIpAddr = dev->getIPv4Address();

	// Create new IPv4 layer
	pcpp::IPv4Layer ipLayer(srcIpAddr, *dstIpAddr);
	ipLayer.getIPv4Header()->timeToLive = 64;

	// Create packet
	pcpp::Packet packet(100);
	packet.addLayer(&ethLayer);
	packet.addLayer(&ipLayer);
	packet.addLayer(&tcpLayer);

	packet.computeCalculateFields();

	return dev->sendPacket(&packet);
}

void parsePacket(pcpp::RawPacket* rawPacket, pcpp::PcapLiveDevice* dev, void* hosts) {
	set<pcpp::IPv4Address>* knownHosts = (set<pcpp::IPv4Address>*)hosts;
	pcpp::Packet parsedPacket(rawPacket);

	pcpp::IPv4Layer* ipLayer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
	pcpp::TcpLayer* tcpLayer = parsedPacket.getLayerOfType<pcpp::TcpLayer>();
	pcpp::IPv4Address srcIp = ipLayer->getSrcIPv4Address();
	uint16_t srcPort = tcpLayer->getSrcPort();

	// Determine if IP has already been detected.
	if (knownHosts->find(srcIp) == knownHosts->end()) {
		knownHosts->insert(srcIp);

		// Output results.
		string writeString = srcIp.toString();
		if (tcpLayer->getTcpHeader()->synFlag == 1) {
			writeString += ":" + to_string(srcPort);
		}

		cout << writeString << endl;
		fileOutput << writeString << endl;
	}
}

void pingWorker(IpIterator* iterator, pcpp::PcapLiveDevice* dev, pcpp::EthLayer* ethLayer, pcpp::TcpLayer* tcpLayer) {
	const int throttle = 10;
	
	pcpp::IPv4Address* dstIpAddr = &iterator->iterate();
	while (dstIpAddr->isValid()) {
		rawTcpPing(dev, *ethLayer, dstIpAddr, *tcpLayer);
		Sleep(throttle);

		dstIpAddr = &iterator->iterate();
	}
}

void TCPScan(const int srcPort, int threadCount, pcpp::IPv4Address srcIpAddr, vector<uint8_t> dstIpStart, vector<uint8_t> dstIpEnd, uint16_t dstPort)
{
	set<pcpp::IPv4Address> hosts;

	// Identify and configure device.
	pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(srcIpAddr);
	pcpp::PcapLiveDevice::DeviceConfiguration deviceConfig(pcpp::PcapLiveDevice::DeviceMode::Normal, pcpp::PcapLiveDevice::PcapDirection::PCPP_IN);
	pcpp::PortRangeFilter recieveFilter(srcPort, srcPort + threadCount, pcpp::Direction::DST);

	// Get the MAC address of the interface
	pcpp::MacAddress srcMacAddr = dev->getMacAddress();
	double arpResTO = 0;
	pcpp::MacAddress dstMacAddr = pcpp::NetworkUtils::getInstance().getMacAddress(dev->getDefaultGateway(), dev, arpResTO, srcMacAddr, srcIpAddr, 10);

	// Create static IP layers
	pcpp::EthLayer ethLayer(srcMacAddr, dstMacAddr);
	vector<pcpp::TcpLayer> tcpLayers(threadCount);
	for (int i = 0; i < tcpLayers.size(); i++) {
		pcpp::TcpLayer tcpLayer(srcPort + i, dstPort);
		tcpLayer.getTcpHeader()->synFlag = 1;
		tcpLayers[i] = tcpLayer;
	}

	// Create IP iterator
	IpIterator iterator(dstIpStart, dstIpEnd);

	// Start packet listening.
	fileOutput.open("output.txt", ios::app);
	dev->open(deviceConfig);
	dev->setFilter(recieveFilter);
	dev->startCapture(parsePacket, &hosts);

	// Start ping threads
	mutex iteratorLock;
	vector<thread> threads(threadCount);

	auto start = chrono::high_resolution_clock::now();
	for (int i = 0; i < threadCount; i++) {
		threads[i] = thread(pingWorker, &iterator, dev, &ethLayer, &tcpLayers[i]);
	}

	for (thread& t : threads) {
		if (t.joinable()) {
			t.join();
		}
	}
	auto stop = chrono::high_resolution_clock::now();

	Sleep(10000);
	dev->stopCapture();
	dev->close();
	fileOutput.close();

	auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
	cout << duration.count() << endl;
}
