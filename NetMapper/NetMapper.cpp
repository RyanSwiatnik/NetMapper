// NetMapper.cpp : Defines the entry point for the application.
//

#pragma comment(lib, "Ws2_32.lib")

#include <ws2tcpip.h>
#include <iostream>

#include <EthLayer.h>
#include <IPv4Layer.h>
#include <TcpLayer.h>

#include <NetworkUtils.h>
#include <PcapLiveDeviceList.h>

#include <chrono>

using namespace std;

bool tcpPing(const char* host, const char* port) {
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return NULL;
    }

    // Create a TCP socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return NULL;
    }

    // Set the timeout for the socket
    int timeout = 1000; // milliseconds
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    // Resolve the host name
    addrinfo* result = NULL;
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    iResult = getaddrinfo(host, port, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }

    // Connect to the target
    iResult = connect(sock, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "connect failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(sock);
        WSACleanup();
        return false;
    }

    std::cout << "TCP ping to " << host << ":" << port << " succeeded" << std::endl;

    // Clean up
    freeaddrinfo(result);
    closesocket(sock);
    WSACleanup();

    return true;
}

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

    dev->sendPacket(&packet);
    
    return true;
}

void parsePacket(pcpp::RawPacket* rawPacket, pcpp::PcapLiveDevice* dev, void* hosts) {
    pcpp::Packet parsedPacket(rawPacket);

    pcpp::IPv4Layer* ipLayer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
    pcpp::TcpLayer* tcpLayer = parsedPacket.getLayerOfType<pcpp::TcpLayer>();

    if (tcpLayer->getTcpHeader()->synFlag == 1) {
        cout << ipLayer->getSrcIPAddress().toString() + " Open" << endl;
    }
    else {
        cout << ipLayer->getSrcIPAddress().toString() + " Closed" << endl;
    }
}

int main() {
    const int srcPort = 4294;
    
    // Test inputs
    pcpp::IPv4Address srcIpAddr = "10.10.10.15";
    uint8_t dstIpStart[] = { 10,10,10,1 };
    uint8_t dstIpEnd[] = { 10,10,10,254 };
    int dstPort = 80;

    // TODO: Add host result storage.
    void* hosts = 0;

    // Identify and configure device.
    pcpp::PcapLiveDevice* dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(srcIpAddr);
    pcpp::PcapLiveDevice::DeviceConfiguration deviceConfig(pcpp::PcapLiveDevice::DeviceMode::Normal, pcpp::PcapLiveDevice::PcapDirection::PCPP_IN);
    pcpp::PortFilter recieveFilter(srcPort, pcpp::Direction::DST);

    // Get the MAC address of the interface
    pcpp::MacAddress srcMacAddr = dev->getMacAddress();
    double arpResTO = 0;
    pcpp::MacAddress dstMacAddr = pcpp::NetworkUtils::getInstance().getMacAddress(dev->getDefaultGateway(), dev, arpResTO, srcMacAddr, srcIpAddr, 10);

    // Create static IP layers
    pcpp::EthLayer ethLayer(srcMacAddr, dstMacAddr);
    pcpp::TcpLayer tcpLayer(srcPort, dstPort);
    tcpLayer.getTcpHeader()->synFlag = 1;

    // Create IP iterator
    uint32_t iterator;
    uint32_t startIP = (
        dstIpStart[0] << 24 |
        dstIpStart[1] << 16 |
        dstIpStart[2] << 8 |
        dstIpStart[3]);
    uint32_t endIP = (
        dstIpEnd[0] << 24 |
        dstIpEnd[1] << 16 |
        dstIpEnd[2] << 8 |
        dstIpEnd[3]);
    
    // Start packet listening.
    dev->open(deviceConfig);
    dev->setFilter(recieveFilter);
    dev->startCapture(parsePacket, hosts);

    // TODO: Add multithreaded packet sending.
    auto start = chrono::high_resolution_clock::now();
    for (iterator = startIP; iterator <= endIP; iterator++)
    {
        const uint8_t bytes[4] = {
            (iterator & 0xFF000000) >> 24,
            (iterator & 0x00FF0000) >> 16,
            (iterator & 0x0000FF00) >> 8,
            (iterator & 0x000000FF)};
        pcpp::IPv4Address dstIpAddr = bytes;
        
        rawTcpPing(dev, ethLayer, &dstIpAddr, tcpLayer);
    }
    auto stop = chrono::high_resolution_clock::now();

    Sleep(1000);
    dev->stopCapture();
    dev->close();

    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << duration.count() << endl;
}
