#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

class Node {
	std::uint32_t ip;
	std::string domainName;
	std::unordered_set<std::uint32_t> links;
public:
	Node();
	Node(std::uint32_t ip);
	void setDomainName(std::string name);
	bool addLink(Node* node);

	std::uint32_t getIp();
	std::string getDomainName();
	std::unordered_set<std::uint32_t>* getLinks();
};
