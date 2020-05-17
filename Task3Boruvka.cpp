//
// Created by Mingyu on 5/15/2020.
//
#include "PrimAndBoruvka.hpp"

#include "Kruskal.hpp"
#include "BoruvkaDistributed.hpp"
#include <iostream>

using namespace std;

int main()
{
    string filename = "test_in.txt";
    GraphInEdge* gie = new GraphInEdge();
    // gie->ReadFile(filename);
    gie->addEdge(Edge(1, 2, 2));
    gie->addEdge(Edge(2, 3, 1));
    gie->addEdge(Edge(3, 1, 2));
    gie->addEdge(Edge(4, 5, 1));
    gie->addEdge(Edge(1, 3, 3));
    gie->addEdge(Edge(1, 4, 1));
    cout << gie->getGraphName() << endl;

    BoruvkaPaSolver bor(gie);
    bor.CalcMST();
    //KruskalSolver Kruskal(gie);
    //cout << Kruskal.CalcMST() << endl;
    //Kruskal.printMST();

    //PrimSolver prim(gie);
    ////gie->findEdge(1, 6);
    //cout << prim.CalcMST() << endl;
    //prim.printMST();
    //
   // BoruvkaSolver boruv(gie);
    //PrimSolver prim(gie);

    //cout << boruv.CalcMST() << prim.CalcMST() << Kruskal.CalcMST() << endl;
    //prim.printMST();
    //boruv.printMST();
    //Kruskal.printMST();
    return 0;

}