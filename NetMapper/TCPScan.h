#pragma once

#include <iostream>
#include <fstream>
#include <set>
#include <mutex>
#include <thread>
#include <vector>

void TCPScan(const int srcPort, int threadCount, pcpp::IPv4Address srcIpAddr, std::vector<uint8_t> dstIpStart, std::vector<uint8_t> dstIpEnd, uint16_t dstPort);
