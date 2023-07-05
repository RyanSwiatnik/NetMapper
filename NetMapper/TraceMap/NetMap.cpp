#include <IPv4Layer.h>

#include "Node.h"
#include "NetMap.h"

using namespace std;

NetMap::NetMap()
{
}

void NetMap::addRoute(list<Node> nodes)
{
	Node* previousNode = nullptr;
	for (Node newNode : nodes) {
		pair<unordered_map<std::uint32_t, Node>::iterator, bool> addResult = addNode(newNode);
		Node* node = &addResult.first->second;
		if (previousNode != nullptr) {
			previousNode->addLink(node);
			//node->addLink(previousNode);
		}
		previousNode = node;

		// If new node
		if (addResult.second) {
			cout << "Added node: " << pcpp::IPv4Address(node->getIp()) << endl;
			// TODO: add new node reverse DNS lookup.
		}
	}
}

pair<unordered_map<std::uint32_t, Node>::iterator, bool> NetMap::addNode(Node node)
{
	// TODO: make thread safe.
	return nodes.insert(make_pair(node.getIp(), node));
}

Node* NetMap::getNode(uint32_t ip)
{
	return &nodes[ip];
}

unordered_map<uint32_t, Node>* NetMap::getNodes() {
	return &nodes;
}
