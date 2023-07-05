#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IcmpLayer.h>
#include <PcapLiveDeviceList.h>
#include <signal.h>

#include "Node.h"
#include "NetMap.h"
#include "TraceRoute.h"
#include "ThreadManager.h"

using namespace std;

void ctrlHandler(int signum) {
	halt = true;
	cout << "Exiting, please wait..." << endl;
}

void traceWorker(ThreadManager* threadManager, int id) {
	vector<uint8_t> randomIp(4);
	random_device rd;
	mt19937 gen(rd());

	while (!halt) {
		// Generate random ip
		for (int i = 0; i < 4; i++) {
			uniform_int_distribution<> distr(threadManager->dstIpStart[i], threadManager->dstIpEnd[i]);
			randomIp[i] = distr(gen);
		}

		// Send pings till time out
		pcpp::IPv4Address randomIPv4 = randomIp.data();
		int ttl = 1;
		cv_status status = cv_status::no_timeout;
		while (status != cv_status::timeout) {
			traceRoute(threadManager->dev, *threadManager->ethLayer, randomIPv4, ttl, id);

			// Wait for reply
			unique_lock<mutex> lck(threadManager->cv_m[id]);
			status = threadManager->cv[id].wait_for(lck, chrono::seconds(1));

			ttl++;
		}

		// Add finished route to map
		threadManager->saveRoute(id);
	}
}

void parseIcmpPacket(pcpp::RawPacket* rawPacket, pcpp::PcapLiveDevice* dev, void* manager) {
	ThreadManager* threadManager = (ThreadManager*)manager;
	pcpp::Packet parsedPacket(rawPacket);

	pcpp::IPv4Layer* ipLayer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
	pcpp::IPv4Address srcIp = ipLayer->getSrcIPv4Address();

	// Filter out sent packets
	if (srcIp != threadManager->dev->getIPv4Address()) {
		pcpp::IcmpLayer* icmpLayer = parsedPacket.getLayerOfType<pcpp::IcmpLayer>();

		// Test if responce is a TTL exceed
		if (icmpLayer->getMessageType() == pcpp::IcmpMessageType::ICMP_TIME_EXCEEDED) {
			uint8_t* replyData = icmpLayer->getData();
			replyData += 33;
			
			// Add node to route
			threadManager->addReply(*replyData, false, srcIp.toInt());
		}
		else if (icmpLayer->getMessageType() == pcpp::IcmpMessageType::ICMP_ECHO_REPLY){
			// Add final node to route
			pcpp::icmp_echo_reply* replyData = icmpLayer->getEchoReplyData();
			threadManager->addReply(replyData->header->id >> 8, true, srcIp.toInt());
		}
	}
}

ThreadManager::ThreadManager(pcpp::PcapLiveDevice* dev, pcpp::EthLayer* ethLayer, vector<uint8_t> dstIpStart, vector<uint8_t> dstIpEnd, int threadCount, NetMap* map) {
	cv = vector<condition_variable>(threadCount);
	cv_m = vector<mutex>(threadCount);
	nodes = vector<list<Node>>(threadCount);

	ThreadManager::map = map;
	ThreadManager::dev = dev;
	ThreadManager::ethLayer = ethLayer;
	ThreadManager::dstIpStart = dstIpStart;
	ThreadManager::dstIpEnd = dstIpEnd;

	// Create Ctrl+C catch.
	signal(SIGINT, ctrlHandler);

	dev->startCapture(parseIcmpPacket, this);

	threads = vector<thread>(threadCount);
	for (int i = 0; i < threadCount; i++) {
		threads[i] = thread(traceWorker, this, i);
	}

	for (thread& t : threads) {
		if (t.joinable()) {
			t.join();
		}
	}

	dev->stopCapture();
	dev->close();
}

void ThreadManager::addReply(int id, bool final, uint32_t ip) {
	if (!final) {
		cv[id].notify_one();
	}
	nodes[id].push_back(Node(ip));
	cout << "Added node: " << pcpp::IPv4Address(ip) << endl;
}

void ThreadManager::saveRoute(int id) {
	map->addRoute(nodes[id]);
	nodes[id] = list<Node>();
}
