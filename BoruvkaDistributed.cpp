#include "BoruvkaDistributed.hpp"

#include <algorithm>
#include<iostream>
#include <mpi.h>
#include<cstdio>
#include <cstdlib>
#include <cassert>
#include <unordered_set>

BoruvkaPaSolver::BoruvkaPaSolver()
{
	graph = new GraphInEdge();
	MSTCost = -1;
}

BoruvkaPaSolver::~BoruvkaPaSolver()
{
	// delete graph;
}


BoruvkaPaSolver::BoruvkaPaSolver(GraphInEdge* graph)
{
	this->graph = graph;
	MSTCost = -1;
}

void BoruvkaPaSolver::ReadFile(string filename)
{
	graph->ReadFile(filename);
}

double BoruvkaPaSolver::CalcMST()
{
	MSTedges.clear();
	MSTCost = 0;

	const int taille = graph->getVertSize();

	double ans = 0; //??????
	int NumEdge = 0; //?????????
	const int TailleEdges = graph->edges.size(); //????
	vector<int> father(graph->getVertSize() + 1); //??????
	for (int i = 0; i < graph->getVertSize() + 1; i++) { father[i] = i; }
	vector<int> cheapest(graph->getVertSize() + 1); //??components?????????
	int NumCompos = graph->getVertSize() + 1; //??????+1
	int it_count = 0;
	vector<int> edges_mst = {};

	
	while (NumCompos > 2) //?????????2
	{
		vector<int> temp = {};
		for (int i = 0; i < graph->getVertSize() + 1; i++)
		{
			temp.push_back(findFather(father, i));
		}
		unordered_set<int> setcomponents(temp.begin(), temp.end()); //???????
		vector<int> components(setcomponents.begin(), setcomponents.end());
		NumCompos = components.size();

	MPI_Init(NULL, NULL);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	assert(world_size > 1);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int number_per_process[2]; //?????????????
	if (world_rank != world_size - 1)
	{
		number_per_process[0] = world_rank * (int)(TailleEdges / world_size) + 1;
		number_per_process[1] = number_per_process[0] + (int)(TailleEdges / world_size);
	}
	if (world_rank == world_size - 1)
	{
		number_per_process[0] = world_rank * (int)(TailleEdges / world_size) + 1;
		number_per_process[1] = TailleEdges;
	}
	MPI_Barrier(MPI_COMM_WORLD); //Wait until all processes are ready

	while (NumCompos > 2)
	{

		
		for (int i = 0; i < graph->getVertSize() + 1; i++) { cheapest[i] = -1; } //?????components?????????? -1

		for (int i = number_per_process[0]; i < number_per_process[1] + 1; i++) //???????
		{
			int faU = findFather(father, graph->edges[i].u); //????u????????
			int faV = findFather(father, graph->edges[i].v);
			int cost = graph->edges[i].cost;
			if (faU != faV)
			{
				if (cheapest[faU] == -1) //work on the component of U
				{
					cheapest[faU] = i;
				}
				else
				{
					if (cost < graph->edges[cheapest[faU]].cost)
					{
						cheapest[faU] = i;
					}
				}

				if (cheapest[faV] == -1) //work on the component of V
				{
					cheapest[faV] = i;
				}
				else
				{
					if (cost < graph->edges[cheapest[faV]].cost)
					{
						cheapest[faV] = i;
					}
				}
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);

		vector<int> CollCheapest;
		// int* CollCheapests = NULL; //??????????
		if (world_rank == 0)
		{
			CollCheapest.resize(world_size * (graph->getVertSize() + 1));
			// CollCheapests = (int*)malloc(sizeof(int) * world_size * (graph->getVertSize() + 1));
		}

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Gather(cheapest.data(), graph->getVertSize() + 1, MPI_INT, 
					CollCheapests.data(), graph->getVertSize() + 1, MPI_INT, 0, MPI_COMM_WORLD);

		if(world_rank==0)//??????????
		{
			vector<int>
		}
		MPI_Barrier(MPI_COMM_WORLD);
	
	}


	return 0;
}

void BoruvkaPaSolver::printMST()
{
	for (auto i = MSTedges.begin(); i < MSTedges.end(); i++)
	{
		cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
	}
}
