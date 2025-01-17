//
// Created by Shikang on 4/19/2020.
//

#ifndef INF442_P3_GRAPH_H
#define INF442_P3_GRAPH_H

#include <map>
#include <string>
#include <vector>

constexpr auto INF = 10000000;

using namespace std;

struct Edge {
    int u, v;
    double cost;
    Edge(int u_, int v_, double cost_);
    Edge(const Edge& e);
    Edge();
    static bool cmp(Edge a, Edge b);
};

class Graph {
   protected:
    int vertSize;  // size of vertex
    int edgeSize;

   public:
    Graph();
    //    ~Graph();
    int getVertSize() const;
    int getEdgeSize() const;
    virtual void ReadFile(string filename) = 0;
    virtual double getEdgeCost(int u, int v) = 0;
};

class GraphInEdge : public Graph {
    /*
     * Direced Graph, implemented by a vector of edges.
     */
   protected:
    vector<Edge> edges;
    string graphName;
    vector<vector<int>> adjmatrix;
    friend class KruskalSolver;
    friend class PrimSolver;
    friend class BoruvkaSolver;
    friend class PrimPaSolver;
    friend class BoruvkaPaSolver;

   public:
    GraphInEdge();
    GraphInEdge(const GraphInEdge& gie);
    //    ~GraphInEdge();
    void addEdge(Edge e);
    string getGraphName() const;
    double getEdgeCost(int u, int v) override;
    int findEdge(int u, int v);
    vector<vector<int>> createAdjmatrix();
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

#endif  // INF442_P3_GRAPH_H
