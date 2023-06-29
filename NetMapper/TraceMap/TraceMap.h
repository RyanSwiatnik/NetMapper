#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <fstream>

void TraceMap(int threadCount, pcpp::IPv4Address srcIpAddr, std::vector<uint8_t> dstIpStart, std::vector<uint8_t> dstIpEnd);
