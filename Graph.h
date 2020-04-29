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
private:
    vector<Edge> edges;
    string graphName;
public:
    GraphInEdge();
//    ~GraphInEdge();
    string getGraphName() const;
    double getEdgeCost(int u, int v);
    bool findEdge(int u, int v);
    void ReadFile(string filename) override;
    int Kruskal();
};


#endif //INF442_P3_GRAPH_H
