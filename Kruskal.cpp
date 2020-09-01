//
// Created by Shikang on 5/9/2020.
//

#include "Kruskal.hpp"

#include <assert.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

KruskalSolver::KruskalSolver() {
    graph = new GraphInEdge();
    MSTCost = -1;
}

KruskalSolver::~KruskalSolver() {
    // delete graph;
}

KruskalSolver::KruskalSolver(GraphInEdge* graph) {
    this->graph = graph;
    MSTCost = -1;
}

void KruskalSolver::setGraph(GraphInEdge* gie) { graph = gie; }

void KruskalSolver::ReadFile(string filename) { graph->ReadFile(filename); }

double KruskalSolver::CalcMST() {
    /*
     * Kruskal's algorithm : find MST
     * Params:
     * return: sum of weights of MST
     */
    MSTedges.clear();
    vector<int> father(graph->getVertSize() +
                       1);  // Initialize UnionFind, starts from 1 to vertSize+1
    double ans = 0;         // sum of weights
    int NumEdge = 0;        // Size of SMT
    for (int i = 0; i < graph->getVertSize() + 1; i++) father[i] = i;
    std::sort(graph->edges.begin(), graph->edges.end(),
              Edge::cmp);                           // Sort edges
    for (int i = 0; i < graph->getEdgeSize(); ++i)  // traverse edge
    {
        int faU = findFather(father, graph->edges[i].u);
        int faV = findFather(father, graph->edges[i].v);
        if (faU != faV) {
            father[faU] = faV;  // Union the component
            ans += graph->edges[i].cost;
            NumEdge++;
            MSTedges.push_back(i);
            if (NumEdge == graph->getVertSize() - 1)  // The algo end
                break;
        }
    }
    if (NumEdge != graph->getVertSize() - 1)  // Not connected
        return -1;
    else {
        MSTCost = ans;
        return ans;  // sum of weights of SMT
    }
}

void KruskalSolver::printMST() {
    for (auto i = MSTedges.begin(); i < MSTedges.end(); i++) {
        cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
    }
}

vector<Edge> KruskalSolver::getMSTedges() {
    vector<Edge> ve;
    for (auto i = MSTedges.begin(); i != MSTedges.end(); i++) {
        ve.push_back(graph->edges[*i]);
    }
    return ve;
}
