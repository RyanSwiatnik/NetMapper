#pragma once

#include <vector>
#include <mutex>
#include <random>
#include <chrono>
#include <list>
#include <iostream>

static bool halt = false;

class ThreadManager {
	NetMap* map;
	std::vector<std::list<Node>> nodes;

	std::vector<std::thread> threads;

public:
	pcpp::PcapLiveDevice* dev;
	pcpp::EthLayer* ethLayer;
	std::vector<std::uint8_t> dstIpStart;
	std::vector<std::uint8_t> dstIpEnd;

	std::vector<std::condition_variable> cv;
	std::vector<std::mutex> cv_m;

	ThreadManager(pcpp::PcapLiveDevice* dev, pcpp::EthLayer* ethLayer, std::vector<std::uint8_t> dstIpStart, std::vector<std::uint8_t> dstIpEnd, int threadCount, NetMap* map);
	void addReply(int id, bool final, std::uint32_t ip);
	void addDomainName(std::uint32_t ip, std::string domainName);
	void saveRoute(int id);
};
