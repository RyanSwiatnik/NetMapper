#pragma once

void traceRoute(pcpp::PcapLiveDevice* dev, pcpp::EthLayer ethLayer, pcpp::IPv4Address dstIpAddr, int ttl, int id);
