#include <IPv4Layer.h>
#include "TraceMap.h"
#include "Node.h"
#include "NetMap.h"

using namespace std;

void exportNetJsonGraph(NetMap map) {
	ofstream out("netjsonmap.json");
	unordered_map<uint32_t, Node>* nodes = map.getNodes();

	out << "{" << endl;
	out << "\"type\": \"NetworkGraph\"," << endl;
	out << "\"label\" : \"Internet\"," << endl;
	out << "\"nodes\" : [" << endl;

	string linksString = "";
	for (auto node = nodes->begin(); node != nodes->end(); node++) {
		unordered_set<uint32_t>* links = node->second.getLinks();

		if (node != nodes->begin()) {
			out << "," << endl;
			if (links->size() != 0) {
				linksString += ",\n";
			}
		}
		out << "{\"id\": \"" << to_string(node->first) << "\", \"name\" : \"" << node->second.getDomainName() << "\" }";

		for (auto link = links->begin(); link != links->end(); link++) {
			if (link != links->begin()) {
				linksString += ",\n";
			}
			linksString += "{\"source\": \"" + to_string(node->first) + "\", \"target\" : \"" + to_string(*link) + "\"}";
		}
	}

	out << endl << "]," << endl;
	out << "\"links\": [" << endl;
	out << linksString;
	out << endl << "]" << endl;
	out << "}" << endl;

	out.close();
}

void TraceMap(int threadCount, pcpp::IPv4Address srcIpAddr, vector<uint8_t> dstIpStart, vector<uint8_t> dstIpEnd)
{
}
