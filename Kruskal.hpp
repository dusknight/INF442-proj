//
// Created by Shikang on 5/9/2020.
//

#ifndef INF442_P3_KRUSKAL_HPP
#define INF442_P3_KRUSKAL_HPP

#include "Graph.h"

class KruskalSolver :public MSTSolver {
private:
    GraphInEdge* graph;
    vector<int> MSTedges;
    double MSTCost;
public:
    KruskalSolver();
    ~KruskalSolver();
    KruskalSolver(GraphInEdge* graph);
    void setGraph(GraphInEdge* gie);
    GraphInEdge* getGraph() { return graph; };
    void ReadFile(string filename);
    double CalcMST() override;
    void printMST() override;
    vector<Edge> getMSTedges();
};


#endif //INF442_P3_KRUSKAL_HPP
