//
// Created by Mingyu on 5/15/2020.
//
#include <time.h>

#include <iostream>

#include "BoruvkaDistributed.hpp"
#include "Kruskal.hpp"
#include "PrimAndBoruvka.hpp"

using namespace std;

int main() {
    string filename = "4096p21.txt";
    GraphInEdge* gie = new GraphInEdge();
    gie->ReadFile(filename);
    auto test = gie->createAdjmatrix();
    // gie->addEdge(Edge(1, 2, 2));
    // gie->addEdge(Edge(2, 3, 1));
    // gie->addEdge(Edge(3, 1, 2));
    // gie->addEdge(Edge(4, 5, 1));
    // gie->addEdge(Edge(1, 3, 3));
    // gie->addEdge(Edge(1, 4, 1));
    // cout << gie->getGraphName() << endl;

    BoruvkaPaSolver bor(gie);
    clock_t start = clock();
    double var = bor.CalcMST();
    clock_t end = clock();
    cout << var << endl;
    cout << "It spend " << (end - start) * 1.0 / (CLOCKS_PER_SEC) << " seconds"
         << endl;
    // bor.printMST();
    // KruskalSolver Kruskal(gie);
    // cout << Kruskal.CalcMST() << endl;
    // Kruskal.printMST();

    // PrimSolver prim(gie);
    ////gie->findEdge(1, 6);
    // cout << prim.CalcMST() << endl;
    // prim.printMST();
    //
    // BoruvkaSolver boruv(gie);
    // PrimSolver prim(gie);

    // cout << boruv.CalcMST() << prim.CalcMST() << Kruskal.CalcMST() << endl;
    // prim.printMST();
    // boruv.printMST();
    // Kruskal.printMST();
    return 0;
}
