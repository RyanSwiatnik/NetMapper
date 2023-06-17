#include "Node.h"

using namespace std;

Node::Node() {}

Node::Node(uint32_t ip)
{
	Node::ip = ip;
}

void Node::setDomainName(string name)
{
	domainName = name;
}

bool Node::addLink(Node* node)
{
	return links.insert(node->getIp()).second;
}

uint32_t Node::getIp()
{
	return ip;
}
