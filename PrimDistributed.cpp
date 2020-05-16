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
	//double ans = 0; //所求边权之和
	MSTCost = 0;
	MPI_Init(NULL, NULL);
	const int taille = graph->getVertSize();
	
	int startpoint = 1; // 以顶点1作为出发点
	vector<bool> visited = {0, 1,}; // 初始化访问过的节点的记录
	for (int i = 1; i < taille; i++)
	{
		
		visited.push_back(0);
	}
	vector<double> costs; // 初始化与访问过的节点邻接的距离的记录
	vector<int> closest; // 初始化 每一节点到已访问过的节点中最近节点的指标
	for (int i = 0; i < taille + 1; i++)
	{
		costs.push_back(0);
		closest.push_back(1);
	}

	for (int i = 2; i < taille + 1; i++) //初始化costs
	{
		costs[i] = min(graph->getEdgeCost(1, i), graph->getEdgeCost(i, 1));
	}


	// 通过调用以下方法来得到所有可以工作的进程数量
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	assert(world_size > 1);
	// 得到当前进程的秩
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int number_per_process[2]; //分配给每个进程指定的处理顶点
	if (world_rank != world_size - 1) 
	{
		number_per_process[0] = world_rank * (int)(taille / world_size) + 1;
		number_per_process[1] = number_per_process[0] + (int)(taille / world_size);
	}
	if (world_rank == world_size - 1)
	{
		number_per_process[0] = world_rank * (int)(taille / world_size) + 1;
		number_per_process[1] = taille;
	}
	MPI_Barrier(MPI_COMM_WORLD); //Wait until all processes are ready


	auto count = 1; // 计数已加入的定点数，初始化为1
	while (count < taille) //控制循环
	{
		count += 1; 
		int cur = 1; //初始化查找所需的变量
		int minor = INF; //当前进程距离已知MST较小距离
		int in_which_process = world_rank; //记录当前节点rank

		for (int j = number_per_process[0]; j <= number_per_process[1]; j++) //找到当前进程中未加入的节点中距离当前已得森林中最近的点
		{
			if ((!visited[j]) && costs[j] < minor)
			{
				minor = costs[j];
				cur = j;
			}
		}

		int sub_data[3] = {cur, minor, in_which_process}; //存储当前进程的最优顶点，其cost，和所在进程

		MPI_Barrier(MPI_COMM_WORLD); //主进程准备收集信息
		int* sub_datas = NULL; //主进程收集信息缓冲区
		if (world_rank == 0)
		{
			sub_datas = (int*)malloc(sizeof(int) * world_size * 3);
		}
		MPI_Gather(sub_data, 3, MPI_INT, sub_datas, 3, MPI_INT, 0, MPI_COMM_WORLD); //主进程 0 收集信息

		if (world_rank == 0) //主进程从收进来的数据中找出到已有分支最近的点
		{
			int cur_0 = 1;
			int minor_0 = INF;
			int in_which_proc = 1;
			for (int i = 0; i < world_size; i++)
			{
				auto ind = sub_datas[i * 3];
				if (!visited[ind] && costs[ind] < minor_0)
				{
					cur_0 = ind;
					minor_0 = sub_datas[1 + i * 3];
					in_which_proc = sub_datas[2 + i * 3];
				}
			}
			sub_data[0] = cur_0;
			sub_data[1] = minor_0;
			sub_data[2] = in_which_proc;
		}

		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(sub_data, 3, MPI_INT, 0, MPI_COMM_WORLD); //信息传回各进程

		if (sub_data[1] == INF)
		{
			cout << "Not Connected" << endl;
			return -1;
		}

		visited[sub_data[0]] = true; //标记点已加入
		MPI_Barrier(MPI_COMM_WORLD);

		if (world_rank = sub_data[2]) //在管理此最近顶点的进程上寻找其对应加入的边
		{
			int edge_number = 0;
			if (graph->getEdgeCost(sub_data[0], closest[sub_data[0]]) > graph->getEdgeCost(
				closest[sub_data[0]], sub_data[0])) // 选择加入的边
			{
				edge_number = graph->findEdge(closest[sub_data[0]], sub_data[0]);
			}
			else
			{
				edge_number = graph->findEdge(sub_data[0], closest[sub_data[0]]);
			}
			MSTedges.push_back(edge_number);

			MSTCost += sub_data[1];
		}

		MPI_Barrier(MPI_COMM_WORLD);
		for (int j = number_per_process[0]; j < number_per_process[1] + 1; j++) //在每个进程中更新每个未访问的点到已知部分的最小距离（只更新自己管理的部分）
		{
			if (!visited[j] && costs[j] > min(graph->getEdgeCost(j, sub_data[0]), graph->getEdgeCost(sub_data[0], j)))
			{
				costs[j] = min(graph->getEdgeCost(j, sub_data[0]), graph->getEdgeCost(sub_data[0], j));
				closest[j] = sub_data[0];
			}
		}
	}
	MPI_Finalize();
	return MSTCost;
}

void PrimPaSolver::printMST()
{
	for (auto i = MSTedges.begin(); i < MSTedges.end(); i++)
	{
		cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
	}
}
