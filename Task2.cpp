//
// Created by Shikang on 5/9/2020.
//
#include <ctime>

#include "Kruskal.hpp"
#include "PrimAndBoruvka.hpp"
#include <iostream>

using namespace std;

int main()
{
    string filename = "8192nodep81.txt";
    GraphInEdge* gie = new GraphInEdge();
    gie->ReadFile(filename);
    //gie->addEdge(Edge(1, 2, 2));
    //gie->addEdge(Edge(2, 3, 1));
    //gie->addEdge(Edge(3, 1, 2));
    //gie->addEdge(Edge(4, 5, 1));
    //gie->addEdge(Edge(1, 3, 3));
    //gie->addEdge(Edge(1, 4, 1));
    //cout << gie->getGraphName() << endl;

    KruskalSolver Kruskal(gie);


    clock_t start = clock();
    double var = Kruskal.CalcMST();
    clock_t end = clock();
    cout << var << endl;
    cout << "It spend " << (end - start) * 1.0 / (CLOCKS_PER_SEC) << " seconds" << endl;
	
    //cout << Kruskal.CalcMST() << endl;
    //Kruskal.printMST();

    //PrimSolver prim(gie);
    ////gie->findEdge(1, 6);
    //cout << prim.CalcMST() << endl;
    //prim.printMST();
    //
    //BoruvkaSolver boruv(gie);
    //PrimSolver prim(gie);

    //cout << boruv.CalcMST() << prim.CalcMST() << Kruskal.CalcMST() << endl;
    //prim.printMST();
    //boruv.printMST();
    //Kruskal.printMST();
    return 0;

}
