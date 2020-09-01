//
// Created by Shikang on 5/9/2020.
//
#include <ctime>
#include <iostream>

#include "Kruskal.hpp"
#include "PrimAndBoruvka.hpp"

using namespace std;

int main() {
    string filename = "2560p22.txt";
    GraphInEdge* gie = new GraphInEdge();
    gie->ReadFile(filename);
    auto test = gie->createAdjmatrix();

    // cout << test[1][] << endl;
    // gie->addEdge(Edge(1, 2, 2));
    // gie->addEdge(Edge(2, 3, 1));
    // gie->addEdge(Edge(3, 1, 2));
    // gie->addEdge(Edge(4, 5, 1));
    // gie->addEdge(Edge(1, 3, 3));
    // gie->addEdge(Edge(1, 4, 1));
    // cout << gie->getGraphName() << endl;

    // KruskalSolver Kruskal(gie);
    // cout << Kruskal.CalcMST() << endl;
    // Kruskal.printMST();

    // PrimSolver prim(gie);
    ////gie->findEdge(1, 6);
    // cout << prim.CalcMST() << endl;
    // prim.printMST();
    //
    BoruvkaSolver solver(gie);
    clock_t start = clock();
    double var = solver.CalcMST();
    clock_t end = clock();
    cout << var << endl;
    cout << "It spend " << (end - start) * 1.0 / (CLOCKS_PER_SEC) << " seconds"
         << endl;
    // boruv.printMST();

    // PrimSolver prim(gie);
    // cout <<  prim.CalcMST()  << endl;
    // prim.printMST();
    return 0;
}
