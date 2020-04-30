//
// Created by Shikang on 4/19/2020.
//

#ifndef INF442_P3_GRAPH_H
#define INF442_P3_GRAPH_H

#include <vector>
#include <string>
#include <map>

using namespace std;

struct Edge{
    int u, v;
    double cost;
    Edge(int u_, int v_, double cost_);
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
    virtual void ReadFile(string filename)=0;
    virtual double getEdgeCost(int u, int v)=0;
};

class GraphInEdge :public Graph{
    /*
     * Oriented Graph, implemented by a vector of edges.
     */
private:
    vector<Edge> edges;
    string graphName;
    vector<int> MSTedges;
    double MSTCost;
public:
    GraphInEdge();
//    ~GraphInEdge();
    void addEdge(Edge e);
    string getGraphName() const;
    double getEdgeCost(int u, int v) override;
    bool findEdge(int u, int v);
    void ReadFile(string filename) override;
    double Kruskal();
    void printMST();
};


#endif //INF442_P3_GRAPH_H
