#include "Node.h"

using namespace std;

Node::Node() {
	Node::ip = 0;
}

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

std::string Node::getDomainName()
{
	return domainName;
}

std::unordered_set<std::uint32_t>* Node::getLinks()
{
	return &links;
}
