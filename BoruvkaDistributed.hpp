//
// Created by Mingyu ZHANG on a random day.
//

#ifndef INF442_P3_BoruvkaDISTRIBUTED_HPP
#define INF442_P3_BoruvkaDISTRIBUTED_HPP

#include "Graph.h"
class BoruvkaPaSolver :public MSTSolver {
private:
    GraphInEdge* graph;
    vector<int> MSTedges;
    double MSTCost;
public:
    BoruvkaPaSolver();
    ~BoruvkaPaSolver();
    BoruvkaPaSolver(GraphInEdge* graph);
    void ReadFile(string filename);
    double CalcMST() override;
    void printMST() override;
};



#endif //INF442_P3_BoruvkaDISTRIBUTED_HPP