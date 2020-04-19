//
// Created by Shikang on 4/19/2020.
//

#ifndef INF442_P3_GRAPH_H
#define INF442_P3_GRAPH_H

#include <vector>
#include <string>
#include <map>
struct Edge{
    int u, v;
    int w;
};

class Graph {
private:
    int size; // size of nodes
    vector<Edge> E;
public:
    void ReadFile(string filename);
    Graph();
    ~Graph();
    int getVectSize();
    int getEdgeSize();
};

class SimpleGraph :Graph{
private:
public:
    SimpleGraph(Graph g);
    int getWeight(int u, int v);
};



#endif //INF442_P3_GRAPH_H
