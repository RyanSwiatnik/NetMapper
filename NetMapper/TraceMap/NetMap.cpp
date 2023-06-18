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
		Node* node = &addNode(newNode).first->second;
		if (previousNode != nullptr) {
			previousNode->addLink(node);
			//node->addLink(previousNode);
		}
		previousNode = node;
	}
}

pair<unordered_map<std::uint32_t, Node>::iterator, bool> NetMap::addNode(Node node)
{
	return nodes.insert(make_pair(node.getIp(), node));
}

Node* NetMap::getNode(uint32_t ip)
{
	return &nodes[ip];
}

unordered_map<uint32_t, Node>* NetMap::getNodes() {
	return &nodes;
}
