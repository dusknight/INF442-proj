//
// Created by Shikang on 4/19/2020.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <assert.h>
#include <unordered_set>

#include "Graph.h"


using namespace std;


const double DEFAULT_COST = 1;

Edge::Edge(int u_, int v_, double cost_) : u(u_), v(v_), cost(cost_)
{
}

Edge::Edge(const Edge& e)
{
	u = e.u;
	v = e.v;
	cost = e.cost;
}

bool Edge::cmp(Edge a, Edge b)
{
	return a.cost < b.cost;
}

int Graph::getEdgeSize() const { return edgeSize; }
int Graph::getVertSize() const { return vertSize; }

Graph::Graph()
{
	vertSize = 0;
	edgeSize = 0;
}


GraphInEdge::GraphInEdge() : Graph()
{
	graphName = "";
}

GraphInEdge::GraphInEdge(GraphInEdge& gie)
{
	graphName = gie.graphName;
	edges = vector<Edge>(gie.edges);
}

string GraphInEdge::getGraphName() const { return graphName; }

int GraphInEdge::findEdge(int u, int v)
{
	// TODO: implement
	int l = this->edges.size();
	int i = 0;
	while (i < l)
	{
		if (edges[i].u == u)
		{
			if (edges[i].v == v)
			{
				//std::cout << "The edge is located in "<< i <<"th position in edges"<<std::endl;
				return i;
			}
		}
		i += 1;
	}
	return -1;
}

void GraphInEdge::ReadFile(string filename)
{
	string line;
	ifstream ifs(filename);
	if (ifs.is_open())
	{
		int vertCount = 0;
		int blancCount = 0;

		// get first line
		getline(ifs, graphName);
		cout << "[INFO] reading graph: " << graphName << endl;
		// get the other lines
		while (getline(ifs, line))
		{
			if (line.empty() || line == "\r")
			{
				blancCount++;
				continue;
			}
			int node_u, node_v;
			if (blancCount == 2)
			{
				if (line.length() < 2) continue; // if is not edge;
				istringstream iss(line);
				vector<string> results((istream_iterator<std::string>(iss)),
					istream_iterator<std::string>());
				node_u = stoi(results.front());
				if (node_u > vertCount) vertCount = node_u;
				//                int level = stoi(results[1]);
				for (int i = 2; i < results.size(); i++)
				{
					string tmp = results[i];
					if (tmp[0] == '[') tmp = tmp.substr(1);
					if (tmp[tmp.length() - 1] == ',' || tmp[tmp.length() - 1] == ']') tmp = tmp.substr(
						0, tmp.length() - 1);
					node_v = stoi(tmp);
					if (node_v > vertCount) vertCount = node_v;
					edges.push_back(Edge(node_u, node_v, DEFAULT_COST));
				}
			}
			// cout << line << '\n';
		}
		ifs.close();
		edgeSize = edges.size();
		vertSize = vertCount;
	}
	else
	{
		cerr << "Cannot open file: " << filename << endl;
	}
}

vector<vector<Edge>> GraphInEdge::toAdjecentList()
{
	vector<vector<Edge>> vve;
	for (int i = 0; i < vertSize+1; i++) { // ATTENTION: vertId starts from 1
		vector<Edge> ve;
		vve.push_back(ve);
	}

	for (auto ie = edges.begin(); ie != edges.end(); ie++) {
		int from = ie->u;
		int to = ie->v;
		vve[from].push_back(*ie);
	}
	return vve;
}

double GraphInEdge::getEdgeCost(int u, int v)
{
	// TODO
	int l = this->edges.size();
	int i = 0;
	while (i < l)
	{
		if (edges[i].u == u)
		{
			if (edges[i].v == v)
			{
				//std::cout << "The edge is located in " << i << "th position in edges"<<std::endl;
				return edges[i].cost;
			}
		}
		i += 1;
	}

	return INF;
}


int findFather(vector<int>& father, int x)
{
	/*
	 * Union-Find
	 * return the root of Union-find set where x belongs
	 */
	int a = x;
	while (x != father[x])
		x = father[x];
	while (a != father[a])
	{
		int z = a;
		a = father[a];
		father[z] = x;
	}
	return x;
}

bool is_element_in_vector(vector<int> v, int element)
{
	vector<int> ::iterator it;
	it = find(v.begin(), v.end(), element);
	if (it != v.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GraphInEdge::addEdge(Edge e)
{
	if (findEdge(e.u, e.v) != -1) return;
	edges.push_back(e);
	edgeSize++;
	if (e.u > vertSize) vertSize = e.u;
	if (e.v > vertSize) vertSize = e.v;
}

