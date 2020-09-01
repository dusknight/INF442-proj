#include "BoruvkaDistributed.hpp"

#include <mpi.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unordered_set>

BoruvkaPaSolver::BoruvkaPaSolver() {
    graph = new GraphInEdge();
    MSTCost = -1;
}

BoruvkaPaSolver::~BoruvkaPaSolver() {
    // delete graph;
}

BoruvkaPaSolver::BoruvkaPaSolver(GraphInEdge* graph) {
    this->graph = graph;
    MSTCost = -1;
}

void BoruvkaPaSolver::ReadFile(string filename) { graph->ReadFile(filename); }

double BoruvkaPaSolver::CalcMST() {
    MSTedges.clear();
    MSTCost = 0;

    const int taille = graph->getVertSize();

    double ans = 0;   // The sum of weights
    int NumEdge = 0;  ////Number of SMT
    const int TailleEdges = graph->edges.size();
    vector<int> father(graph->getVertSize() + 1);  // Initialize Union-Find
    for (int i = 0; i < graph->getVertSize() + 1; i++) {
        father[i] = i;
    }
    vector<int> cheapest(graph->getVertSize() +
                         1);  // The edge to add for every component
    int NumCompos = graph->getVertSize() + 1;  // Number of Components

    vector<int> edges_mst = {};

    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // assert(world_size > 1);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int number_per_process[2];  // The edges each process work on
    if (world_rank != world_size - 1) {
        number_per_process[0] = world_rank * (int)(TailleEdges / world_size);
        number_per_process[1] =
            number_per_process[0] + (int)(TailleEdges / world_size) - 1;
    }
    if (world_rank == world_size - 1) {
        number_per_process[0] = world_rank * (int)(TailleEdges / world_size);
        number_per_process[1] = TailleEdges - 1;
    }
    MPI_Barrier(MPI_COMM_WORLD);  // Wait until all processes are ready
    // cout << "THE EDGES EACH PROCESS TREAT ARE" << endl;
    // cout << "From " << number_per_process[0] << " to " <<
    // number_per_process[1] << endl;

    int father_no_use = 0;
    while (NumCompos > 2)  // When #Components >1
    {
        // cout << "zhemeduobian "<<NumEdge << " World " <<world_rank<<endl;
        vector<int> temp = {};
        for (int i = 0; i < graph->getVertSize() + 1; i++) {
            temp.push_back(findFather(father, i));
        }

        unordered_set<int> setcomponents(temp.begin(),
                                         temp.end());  // Store the components
        vector<int> components(setcomponents.begin(), setcomponents.end());
        NumCompos = components.size();
        // cout << NumCompos<<"COMPONUMBER" << endl;

        for (int i = 0; i < graph->getVertSize() + 1; i++) {
            cheapest[i] = -1;
        }  // Initialize cheapest edge for each component NULL
        for (int i = number_per_process[0]; i < number_per_process[1] + 1;
             i++)  // Every Process Traverse edge
        {
            // cout << "A" << world_rank << endl;
            int faU = findFather(father, graph->edges[i].u);
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

        MPI_Barrier(MPI_COMM_WORLD);

        vector<int> CollCheapest;

        if (world_rank == 0) {
            CollCheapest.resize(world_size * (graph->getVertSize() + 1));
            // CollCheapests = (int*)malloc(sizeof(int) * world_size *
            // (graph->getVertSize() + 1));
        }

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Gather(cheapest.data(), graph->getVertSize() + 1, MPI_INT,
                   CollCheapest.data(), graph->getVertSize() + 1, MPI_INT, 0,
                   MPI_COMM_WORLD);

        if (world_rank ==
            0)  //  //Main Process find the cheapest edge for every  component
        {
            // cout << "SIZE " << CollCheapest.size() << endl;
            for (int i = 0; i < NumCompos; i++) {
                int cur = components[i];  // The component considering
                cur = findFather(father, cur);
                // cout << cur << "CUR" << endl;

                if (cur != 0) {
                    for (int i = 0; i < world_size;
                         i++)  // Traverse the data gathered from every process
                    {
                        // cout << "INDEX "<< CollCheapest[cur + i *
                        // (graph->getVertSize() + 1)] <<"ANTOHER"<< cur + i *
                        // (graph->getVertSize() + 1) << endl;
                        if (CollCheapest[cur +
                                         i * (graph->getVertSize() + 1)] !=
                            -1) {  // In case some subgraphes are note connected
                            int faU = findFather(
                                father,
                                graph
                                    ->edges[CollCheapest
                                                [cur +
                                                 i * (graph->getVertSize() +
                                                      1)]]
                                    .u);
                            int faV = findFather(
                                father,
                                graph
                                    ->edges[CollCheapest
                                                [cur +
                                                 i * (graph->getVertSize() +
                                                      1)]]
                                    .v);
                            int cost =
                                graph
                                    ->edges[CollCheapest
                                                [cur +
                                                 i * (graph->getVertSize() +
                                                      1)]]
                                    .cost;
                            // cout << world_rank << " RANK" << endl;

                            // cout <<cur + i * (graph->getVertSize() + 1) <<
                            // "A" << endl;

                            if (faU != faV) {
                                if (cheapest[faU] ==
                                    -1)  // work on the component of U
                                {
                                    cheapest[faU] = CollCheapest
                                        [cur + i * (graph->getVertSize() + 1)];
                                } else {
                                    if (cost <
                                        graph->edges[cheapest[faU]].cost) {
                                        cheapest[faU] = CollCheapest
                                            [cur +
                                             i * (graph->getVertSize() + 1)];
                                    }
                                }

                                if (cheapest[faV] ==
                                    -1)  // work on the component of V
                                {
                                    cheapest[faV] = CollCheapest
                                        [cur + i * (graph->getVertSize() + 1)];
                                } else {
                                    if (cost <
                                        graph->edges[cheapest[faV]].cost) {
                                        cheapest[faV] = CollCheapest
                                            [cur +
                                             i * (graph->getVertSize() + 1)];
                                    }
                                }
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < NumCompos;
                 i++)  // Add cheapest edges for each components. Update Father
            {
                int cur = components[i];
                if (cur != 0) {
                    if (cheapest[cur] != -1) {
                        int Indu =
                            findFather(father, graph->edges[cheapest[cur]].u);
                        int Indv =
                            findFather(father, graph->edges[cheapest[cur]].v);

                        father[Indu] = Indv;
                        father_no_use = findFather(father, Indu);
                        if (!is_element_in_vector(
                                edges_mst,
                                cheapest[cur]))  // Avoid adding repeated edges.
                        {
                            MSTedges.push_back(cheapest[cur]);
                            edges_mst.push_back(cheapest[cur]);
                            ans += graph->edges[cheapest[cur]].cost;

                            NumEdge += 1;
                        }
                    }
                }
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(father.data(), graph->getVertSize() + 1, MPI_INT, 0,
                  MPI_COMM_WORLD);  // BroadCast Father
        MPI_Barrier(MPI_COMM_WORLD);

        vector<int> temp2 = {};
        for (int i = 0; i < graph->getVertSize() + 1; i++) {
            temp2.push_back(findFather(father, i));
        }
        std::unordered_set<int> setcomponents2(
            temp2.begin(), temp2.end());  // Update the set of components
        vector<int> components2(setcomponents2.begin(), setcomponents2.end());
        NumCompos = components2.size();
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    if (world_rank != 0) {  // cout << "NOT MAIN PROCESS"<<endl;
        return -2;
    }

    // cout << "THIS IS THE MAIN PROCESS" << "AND ANS IS "<<ans<<endl;
    return ans;
}

void BoruvkaPaSolver::printMST() {
    for (auto i = MSTedges.begin(); i < MSTedges.end(); i++) {
        cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
    }
}
