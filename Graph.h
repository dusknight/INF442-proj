//
// Created by Shikang on 4/19/2020.
//

#ifndef INF442_P3_GRAPH_H
#define INF442_P3_GRAPH_H

#include <vector>
#include <string>
#include <map>

constexpr auto INF = 1000000;

using namespace std;

struct Edge {
	int u, v;
	double cost;
	Edge(int u_, int v_, double cost_);
	Edge(const Edge &e);
	static bool cmp(Edge a, Edge b);
};

class Graph {
protected:
	int vertSize; // size of vertex
	int edgeSize;
public:
	Graph();
	//    ~Graph();
	int getVertSize() const;
	int getEdgeSize() const;
	virtual void ReadFile(string filename) = 0;
	virtual double getEdgeCost(int u, int v) = 0;
};

class GraphInEdge :public Graph {
	/*
	 * Direced Graph, implemented by a vector of edges.
	 */
protected:
	vector<Edge> edges;
	string graphName;
	friend class KruskalSolver;
	friend class PrimSolver;
	friend class BoruvkaSolver;
public:
	GraphInEdge();
	GraphInEdge(GraphInEdge& gie);
	//    ~GraphInEdge();
	void addEdge(Edge e);
	string getGraphName() const;
	double getEdgeCost(int u, int v) override;
	int findEdge(int u, int v);
	void ReadFile(string filename) override;
	vector<vector<Edge>> toAdjecentList();
};

class MSTSolver {
private:
	Graph* graph;
public:
	virtual double CalcMST() = 0;
	virtual void printMST() = 0;
	virtual void ReadFile(string filename) = 0;
};

int findFather(vector<int>& father, int x);
bool is_element_in_vector(vector<int> v, int element);

#endif //INF442_P3_GRAPH_H
