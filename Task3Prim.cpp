//
// Created by Mingyu on a random day.
//
#include <mpi.h>

#include <ctime>
#include <iostream>

#include "Kruskal.hpp"
#include "PrimAndBoruvka.hpp"
#include "PrimDistributed.hpp"

using namespace std;

int main() {
    string filename = "4096p21.txt";
    GraphInEdge* gie = new GraphInEdge();
    gie->ReadFile(filename);
    // gie->addEdge(Edge(1, 2, 2));
    // gie->addEdge(Edge(2, 3, 1));
    // gie->addEdge(Edge(3, 1, 2));
    // gie->addEdge(Edge(4, 5, 1));
    // gie->addEdge(Edge(1, 3, 3));
    // gie->addEdge(Edge(1, 4, 1));
    // cout << gie->getGraphName() << endl;
    //
    auto test = gie->createAdjmatrix();

    // cout << test[2][3].u << endl;
    PrimPaSolver solver(gie);
    // pps.CalcMST();
    clock_t start = clock();
    double var = solver.CalcMST();
    clock_t end = clock();
    cout << var << endl;
    cout << "It spend " << (end - start) * 1.0 / (CLOCKS_PER_SEC) << " seconds"
         << endl;
    // cout << pps.CalcMST() << endl;
    // pps.printMST();

    // KruskalSolver Kruskal(gie);
    ////cout << Kruskal.CalcMST() << endl;
    ////Kruskal.printMST();

    ////PrimSolver prim(gie);
    //////gie->findEdge(1, 6);
    ////cout << prim.CalcMST() << endl;
    ////prim.printMST();
    ////
    // BoruvkaSolver boruv(gie);
    // PrimSolver prim(gie);

    // cout << boruv.CalcMST() << prim.CalcMST() << Kruskal.CalcMST() << endl;
    // prim.printMST();
    // boruv.printMST();
    // Kruskal.printMST();
    return 0;
}
