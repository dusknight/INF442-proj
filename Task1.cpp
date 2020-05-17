//
// Created by Shikang on 5/9/2020.
//
#include "PrimAndBoruvka.hpp"
#include "Kruskal.hpp"
#include <iostream>

using namespace std;

int main()
{
    string filename = "Graph_Input_test.txt";
    GraphInEdge* gie = new GraphInEdge();
     gie->ReadFile(filename);
    //gie->addEdge(Edge(1, 2, 2));
    //gie->addEdge(Edge(2, 3, 1));
    //gie->addEdge(Edge(3, 1, 2));
    //gie->addEdge(Edge(4, 5, 1));
    //gie->addEdge(Edge(1, 3, 3));
    //gie->addEdge(Edge(1, 4, 1));
    //cout << gie->getGraphName() << endl;

    //KruskalSolver Kruskal(gie);
    //cout << Kruskal.CalcMST() << endl;
    //Kruskal.printMST();

    //PrimSolver prim(gie);
    ////gie->findEdge(1, 6);
    //cout << prim.CalcMST() << endl;
    //prim.printMST();
    //
    BoruvkaSolver boruv(gie);
    cout << boruv.CalcMST() << endl;
    boruv.printMST();


	//PrimSolver prim(gie);
    //cout <<  prim.CalcMST()  << endl;
    //prim.printMST();
    return 0;

}

