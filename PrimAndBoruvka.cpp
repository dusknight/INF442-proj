//
// Created by Mingyu ZHANG on 5/9/2020.
//

#include "PrimAndBoruvka.hpp"

#include <assert.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

PrimSolver::PrimSolver() {
    graph = new GraphInEdge();
    MSTCost = -1;
}

PrimSolver::~PrimSolver() {
    // delete graph;
}

PrimSolver::PrimSolver(GraphInEdge* graph) {
    this->graph = graph;
    MSTCost = -1;
}

void PrimSolver::ReadFile(string filename) { graph->ReadFile(filename); }

double PrimSolver::CalcMST() {
    /*
     * Prime's algorithm : find MST
     * Params:
     * return: sum of weights of MST
     */
    MSTedges.clear();
    double ans = 0;  // The sum of weights
    const int taille = graph->getVertSize();

    int startpoint = 1;  // Start from vertex 0
    vector<bool> visited = {
        0,
        1,
    };
    for (int i = 1; i < taille; i++) {
        // Initialize of visited record
        visited.push_back(0);
    }
    vector<double> costs;  // Initialize the cost of every vertex to the SMT
    vector<int> closest;   // Initialize the record of the opposite vertex of
                           // each node which satisfy the cheapest condition
    for (int i = 0; i < taille + 1; i++) {
        costs.push_back(0);
        closest.push_back(1);
    }

    for (int i = 2; i < taille + 1; i++)  //
    {
        costs[i] = min(graph->getEdgeCost(1, i), graph->getEdgeCost(i, 1));
    }

    for (int i = 1; i < taille; i++)  // Traverse Vertex
    {
        int cur = 1;  // The vertex to add
        double minor = INF;
        for (int j = 1; j < taille + 1;
             j++)  // Find the cheapest vertex not visited
        {
            if ((!visited[j]) && costs[j] < minor) {
                minor = costs[j];
                cur = j;
            }
        }
        if (minor == INF) {
            cout << " Not connected" << endl;
            return -1;
        }

        visited[cur] = true;
        // int edge_number = 0;
        // if (graph->getEdgeCost(cur, closest[cur]) >
        // graph->getEdgeCost(closest[cur], cur)) // Choose the direction of the
        // edge to add
        //{
        //    edge_number = graph->findEdge(closest[cur], cur);
        //}
        // else
        //{
        //    edge_number = graph->findEdge(cur, closest[cur]);
        //}
        // MSTedges.push_back(edge_number);

        ans += minor;
        for (int j = 1; j < taille + 1; j++)  // Update Cost
        {
            if (!visited[j] && costs[j] > min(graph->getEdgeCost(j, cur),
                                              graph->getEdgeCost(cur, j))) {
                costs[j] =
                    min(graph->getEdgeCost(j, cur), graph->getEdgeCost(cur, j));
                closest[j] = cur;
            }
        }
    }

    MSTCost = ans;
    return ans;
}

void PrimSolver::printMST() {
    for (auto i = MSTedges.begin(); i < MSTedges.end(); i++) {
        cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
    }
}

BoruvkaSolver::BoruvkaSolver() {
    graph = new GraphInEdge();
    MSTCost = -1;
}

BoruvkaSolver::~BoruvkaSolver() {
    // delete graph;
}

BoruvkaSolver::BoruvkaSolver(GraphInEdge* graph) {
    this->graph = graph;
    MSTCost = -1;
}

void BoruvkaSolver::ReadFile(string filename) { graph->ReadFile(filename); }

double BoruvkaSolver::CalcMST() {
    /*
     * Boruvka's algorithm : find MST
     * Params:
     * return: sum of weights of MST
     */
    MSTedges.clear();
    assert(graph->getVertSize());
    double ans = 0;   // The sum of weights
    int NumEdge = 0;  // Number of SMT
    const int TailleEdges = graph->edges.size();

    vector<int> father(graph->getVertSize() + 1);  // Initialize Union-Find
    for (int i = 0; i < graph->getVertSize() + 1; i++) {
        father[i] = i;
    }

    vector<int> cheapest(graph->getVertSize() +
                         1);  // The edge to add for every component

    int NumCompos = graph->getVertSize() + 1;  // Number of Components+1

    int it_count = 0;
    vector<int> edges_mst = {};
    while (NumCompos > 2)  // When the Number of Components >1
    {
        vector<int> temp = {};
        for (int i = 0; i < graph->getVertSize() + 1; i++) {
            temp.push_back(findFather(father, i));
        }
        std::unordered_set<int> setcomponents(temp.begin(), temp.end());  //
        vector<int> components(setcomponents.begin(), setcomponents.end());
        NumCompos = components.size();

        for (int i = 0; i < graph->getVertSize() + 1; i++) {
            cheapest[i] = -1;
        }  // Initialize the cheapest edge found for each component

        for (int i = 0; i < TailleEdges; i++)  // traverse edges
        {
            int faU =
                findFather(father, graph->edges[i].u);  // Check the component
            int faV = findFather(father, graph->edges[i].v);
            int cost = graph->edges[i].cost;
            if (faU != faV) {
                if (cheapest[faU] == -1)  // work on the component of U
                {
                    cheapest[faU] = i;
                } else {
                    if (cost < graph->edges[cheapest[faU]].cost) {
                        cheapest[faU] = i;
                    }
                }

                if (cheapest[faV] == -1)  // work on the component of V
                {
                    cheapest[faV] = i;
                } else {
                    if (cost < graph->edges[cheapest[faV]].cost) {
                        cheapest[faV] = i;
                    }
                }
            }
        }

        for (int i = 0; i < NumCompos; i++) {
            int cur = components[i];
            if (cur != 0) {
                if (cheapest[cur] != -1) {
                    int Indu =
                        findFather(father, graph->edges[cheapest[cur]].u);
                    int Indv =
                        findFather(father, graph->edges[cheapest[cur]].v);
                    father[Indu] = Indv;
                    if (!is_element_in_vector(
                            edges_mst,
                            cheapest[cur]))  // Avoid adding repeated edge
                    {
                        MSTedges.push_back(cheapest[cur]);
                        edges_mst.push_back(cheapest[cur]);
                        ans += graph->edges[cheapest[cur]].cost;
                        NumEdge += 1;
                    }
                }
            }
        }
        vector<int> temp2 = {};
        for (int i = 0; i < graph->getVertSize() + 1; i++) {
            temp2.push_back(findFather(father, i));
        }

        std::unordered_set<int> setcomponents2(temp2.begin(), temp2.end());
        vector<int> components2(setcomponents2.begin(), setcomponents2.end());
        NumCompos = components2.size();  // Update the set of component.
    }
    MSTCost = ans;
    return ans;
}

void BoruvkaSolver::printMST() {
    for (auto i = MSTedges.begin(); i < MSTedges.end(); i++) {
        cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
    }
}
