#pragma once

#include <vector>

void TraceMap(int threadCount, pcpp::IPv4Address srcIpAddr, std::vector<uint8_t> dstIpStart, std::vector<uint8_t> dstIpEnd);
