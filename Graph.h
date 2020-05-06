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
protected:
    vector<Edge> edges;
    string graphName;
    friend class KruskalSolver;
    friend class PrimSolver;
    friend class BoruvkaSolver;
public:
    GraphInEdge();
    GraphInEdge(GraphInEdge &gie);
//    ~GraphInEdge();
    void addEdge(Edge e);
    string getGraphName() const;
    double getEdgeCost(int u, int v) override;
    int findEdge(int u, int v);
    void ReadFile(string filename) override;
};

class MSTSolver {
private:
    Graph* graph;
public:
    virtual double CalcMST()=0;
    virtual void printMST()=0;
    virtual void ReadFile(string filename) = 0;
};

class KruskalSolver :public MSTSolver {
private:
    GraphInEdge *graph;
    vector<int> MSTedges;
    double MSTCost;
public :
    KruskalSolver();
    ~KruskalSolver();
    KruskalSolver(GraphInEdge *graph);
    void ReadFile(string filename);
    double CalcMST() override;
    void printMST() override;
};

class PrimSolver :public MSTSolver {
private:
    GraphInEdge* graph;
    vector<int> MSTedges;
    double MSTCost;
public:
    PrimSolver();
    ~PrimSolver();
    PrimSolver(GraphInEdge* graph);
    void ReadFile(string filename);
    double CalcMST() override;
    void printMST() override;
};

class BoruvkaSolver: public MSTSolver{
private:
    GraphInEdge* graph;
    vector<int> MSTedges;
    double MSTCost;
public:
    BoruvkaSolver();
    ~BoruvkaSolver();
    BoruvkaSolver(GraphInEdge * graph);
    void ReadFile(string filename);
    double CalcMST() override;
    void printMST() override;
};

#endif //INF442_P3_GRAPH_H
