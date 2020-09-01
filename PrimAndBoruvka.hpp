//
// Created by Mingyu ZHANG on 5/9/2020.
//

#ifndef INF442_P3_PRIMANDBORUVKA_HPP
#define INF442_P3_PRIMANDBORUVKA_HPP

#include "Graph.h"
class PrimSolver : public MSTSolver {
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

class BoruvkaSolver : public MSTSolver {
   private:
    GraphInEdge* graph;
    vector<int> MSTedges;
    double MSTCost;

   public:
    BoruvkaSolver();
    ~BoruvkaSolver();
    BoruvkaSolver(GraphInEdge* graph);
    void ReadFile(string filename);
    double CalcMST() override;
    void printMST() override;
};

#endif  // INF442_P3_PRIMANDBORUVKA_HPP
