#pragma once

#include <unordered_map>
#include <list>
#include <cstdio>

class NetMap {
	std::unordered_map<std::uint32_t, Node> nodes;
public:
	NetMap();
	std::list<std::uint32_t> addRoute(std::list<Node> nodes);
	std::pair<std::unordered_map<std::uint32_t, Node>::iterator, bool> addNode(Node node);

	Node* getNode(std::uint32_t);
	std::unordered_map<std::uint32_t, Node>* getNodes();
};
