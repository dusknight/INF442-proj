#include "PrimDistributed.hpp"

#include <algorithm>
#include<iostream>
#include <mpi.h>
#include<cstdio>
#include <cstdlib>
#include <mpi.h>
#include <cassert>

PrimPaSolver::PrimPaSolver()
{
	graph = new GraphInEdge();
	MSTCost = -1;
}

PrimPaSolver::~PrimPaSolver()
{
	// delete graph;
}


PrimPaSolver::PrimPaSolver(GraphInEdge* graph)
{
	this->graph = graph;
	MSTCost = -1;
}

void PrimPaSolver::ReadFile(string filename)
{
	graph->ReadFile(filename);
}

double PrimPaSolver::CalcMST()
{
	MSTedges.clear();
	int ans = 0; //The sum of weights
	MSTCost = 0;
	const int taille = graph->getVertSize();

	int startpoint = 1; // Start From point 0
	vector<int> visited = {0, 1,}; // Initialize the point visited
	for (int i = 1; i < taille; i++)
	{
		
		visited.push_back(0);
	}
	vector<int> costs; //  Initialize the cost of every vertex to the SMT
	vector<int> closest; //  Initialize the record of the opposite vertex of each node which satisfy the cheapest condition
	for (int i = 0; i < taille + 1; i++)
	{
		costs.push_back(0);
		closest.push_back(1);
	}

	for (int i = 2; i < taille + 1; i++) 
	{
		costs[i] = min(graph->getEdgeCost(1, i), graph->getEdgeCost(i, 1));
	}

	
	
	MPI_Init(NULL, NULL);


	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int number_per_process[2]{0,0}; //The vertex each process treat
	if (world_rank != world_size - 1) 
	{
		number_per_process[0] = world_rank * (int)(taille / world_size) + 1;
		number_per_process[1] = number_per_process[0] + (int)(taille / world_size)-1;
	}
	if (world_rank == world_size - 1)
	{
		number_per_process[0] = world_rank * (int)(taille / world_size) + 1;
		number_per_process[1] = taille;
	}


	MPI_Barrier(MPI_COMM_WORLD); //Wait until all processes are ready
	

	int count = 1; // The number of vertex added

	
	while (count < taille) 
	{

		count += 1;
	
		int cur = 1; //The vertex to add
		int minor = INF; //The cost 
		int in_which_process = world_rank; //The process the 
		
		for (int j = number_per_process[0]; j <= number_per_process[1]; j++) //Find the cheapest vertex for each process
		{
			if ((!visited[j]) && costs[j] < minor)
			{
				minor = costs[j];
				cur = j;
			}
		}

		//cout << "round " << count - 1 <<"peiti "<< minor << endl;

		
		//xcout << "Pro"<<world_rank<<" a " << cur << endl;
		int sub_data[3] = {cur, minor, in_which_process}; //Prepare to send information to main process
		MPI_Barrier(MPI_COMM_WORLD); 
		int* sub_datas = NULL; 
		if (world_rank == 0)
		{
			sub_datas = (int*)malloc(sizeof(int) * world_size * 3);
		}
		MPI_Gather(sub_data, 3, MPI_INT,  sub_datas, 3, MPI_INT, 0, MPI_COMM_WORLD); //Main Process Collect Information


		MPI_Barrier(MPI_COMM_WORLD);
		
		if (world_rank == 0) //Main Process find the cheapest from every process
		{
			int cur_0 = sub_datas[0]; 
			int minor_0 = sub_datas[1];
			int in_which_proc = 0;
			for (int i = 0; i < world_size; i++)
			{
				auto ind = sub_datas[i * 3];
				if (!visited[ind] && sub_datas[1 + i * 3] < minor_0)
				{
					cur_0 = ind;
					minor_0 = sub_datas[1 + i * 3];
					in_which_proc = sub_datas[2 + i * 3];
				}
			}
			sub_data[0] = cur_0;
			sub_data[1] = minor_0;
			sub_data[2] = in_which_proc;
			//cout << 'A' << minor_0 << endl;
		}

		MPI_Barrier(MPI_COMM_WORLD);
		
		MPI_Bcast(sub_data, 3, MPI_INT, 0, MPI_COMM_WORLD); //BroadCast the vertex founded


		
		MPI_Barrier(MPI_COMM_WORLD);

		//cout << count << " woshi " << world_rank <<" da"<<sub_data[0]<< endl;
		
		if (sub_data[1] == INF)
		{
			MPI_Finalize();
			cout << "The Graph is Not Connected" << endl;
			return -1;
		}

		visited[sub_data[0]] = true; //Record Visited
		

		int edge_number = 0;//Initiliaze
		
		MPI_Barrier(MPI_COMM_WORLD);
		//cout << "talaizhao " << sub_data[2] << " WORLD " << world_rank << endl;
	
		if (world_rank == sub_data[2]) //The corresponding process update
		{
			if (graph->getEdgeCost(sub_data[0], closest[sub_data[0]])  >  graph->getEdgeCost(closest[sub_data[0]], sub_data[0])) // Choose the direction to add
			{
				edge_number = graph->findEdge(closest[sub_data[0]], sub_data[0]);
				//cout << "AAA" << endl;
			}
			else
			{
				edge_number = graph->findEdge(sub_data[0], closest[sub_data[0]]);
				//cout << "BBB" << endl;
			}
			
			//MSTedges.push_back(edge_number);
			//MSTCost += sub_data[1];
		}
		
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(&edge_number, 1, MPI_INT, sub_data[2], MPI_COMM_WORLD); //Broadcast the index founded of the edge
		//cout << "ZHEGE edge " << edge_number << endl;
		MSTedges.push_back(edge_number);
		ans += sub_data[1];
		MPI_Barrier(MPI_COMM_WORLD);

		for (int j = number_per_process[0]; j < number_per_process[1] + 1; j++) //Update Cost
		{
			if (!visited[j] && costs[j] > min(graph->getEdgeCost(j, sub_data[0]), graph->getEdgeCost(sub_data[0], j)))
			{
				costs[j] = min(graph->getEdgeCost(j, sub_data[0]), graph->getEdgeCost(sub_data[0], j));
				closest[j] = sub_data[0];
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
		//cout << "heelo end" <<"woshi"<<world_rank<< endl;
	}
	MPI_Finalize();

	MSTCost = ans;
	return ans;
}

void PrimPaSolver::printMST()
{
	for (auto i = MSTedges.begin(); i < MSTedges.end(); i++)
	{
		cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
	}
}
