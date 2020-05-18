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


	double ans = 0; //所求边权之和
	int NumEdge = 0; //记录最小生成树边数
	const int TailleEdges = graph->edges.size(); //图的边数
	vector<int> father(graph->getVertSize() + 1); //初始化并查集
	for (int i = 0; i < graph->getVertSize() + 1; i++) { father[i] = i; }
	vector<int> cheapest(graph->getVertSize() + 1); //每个components在当前步要加入的边
	int NumCompos = graph->getVertSize() + 1; //连通分支数目+1

	vector<int> edges_mst = {};

	MPI_Init(NULL, NULL);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	//assert(world_size > 1);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int number_per_process[2]; //分配给每个进程指定的处理边
	if (world_rank != world_size - 1)
	{
		number_per_process[0] = world_rank * (int)(TailleEdges / world_size);
		number_per_process[1] = number_per_process[0] + (int)(TailleEdges / world_size) - 1;
	}
	if (world_rank == world_size - 1)
	{
		number_per_process[0] = world_rank * (int)(TailleEdges / world_size);
		number_per_process[1] = TailleEdges - 1;
	}
	MPI_Barrier(MPI_COMM_WORLD); //Wait until all processes are ready
	//cout << "THE EDGES EACH PROCESS TREAT ARE" << endl;
	//cout << "From " << number_per_process[0] << " to " << number_per_process[1] << endl;


	int father_no_use = 0;
	while (NumCompos > 2) //当分支数目大于等于2
	{
		//cout << "zhemeduobian "<<NumEdge << " World " <<world_rank<<endl;
		vector<int> temp = {};
		for (int i = 0; i < graph->getVertSize() + 1; i++)
		{
			temp.push_back(findFather(father, i));
		}

		
		unordered_set<int> setcomponents(temp.begin(), temp.end()); //存储目前的分支
		vector<int> components(setcomponents.begin(), setcomponents.end());
		NumCompos = components.size();
		//cout << NumCompos<<"COMPONUMBER" << endl;

		for (int i = 0; i < graph->getVertSize() + 1; i++) { cheapest[i] = -1; } //初始化每个components在当前步要加入的边为 -1
		for (int i = number_per_process[0]; i < number_per_process[1] + 1; i++) //每个进程遍历边
		{
			//cout << "A" << world_rank << endl;
			int faU = findFather(father, graph->edges[i].u); //查询端点u所在集合的根结点
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
		//每个进程遍历边结束
		MPI_Barrier(MPI_COMM_WORLD);

		vector<int> CollCheapest;
		// int* CollCheapests = NULL; //主进程收集信息缓冲区
		if (world_rank == 0)
		{
			CollCheapest.resize(world_size * (graph->getVertSize() + 1));
			// CollCheapests = (int*)malloc(sizeof(int) * world_size * (graph->getVertSize() + 1));
		}

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Gather(cheapest.data(), graph->getVertSize() + 1, MPI_INT,
		           CollCheapest.data(), graph->getVertSize() + 1, MPI_INT, 0, MPI_COMM_WORLD);

		
		if (world_rank == 0) //主进程找出最便宜的边
		{
			//cout << "SIZE " << CollCheapest.size() << endl;
			for (int i = 0; i < NumCompos; i++)
			{
				int cur = components[i]; //现在考虑的component
				cur = findFather(father, cur);
				//cout << cur << "CUR" << endl;
				
				if (cur != 0)
				{
					for (int i = 0; i < world_size; i++) //遍历每个进程传回来的数据
					{
						//cout << "INDEX "<< CollCheapest[cur + i * (graph->getVertSize() + 1)] <<"ANTOHER"<< cur + i * (graph->getVertSize() + 1) << endl;
						if (CollCheapest[cur + i * (graph->getVertSize() + 1)] != -1) { //注意由于有些子图不连通会导致一些component的cheapest的边还是-1
							int faU = findFather(
								father, graph->edges[CollCheapest[cur + i * (graph->getVertSize() + 1)]].u); //查询端点u所在集合的根结点
							int faV = findFather(
								father, graph->edges[CollCheapest[cur + i * (graph->getVertSize() + 1)]].v);
							int cost = graph->edges[CollCheapest[cur + i * (graph->getVertSize() + 1)]].cost;
							//cout << world_rank << " RANK" << endl;

							//cout <<cur + i * (graph->getVertSize() + 1) << "A" << endl;

							if (faU != faV)
							{
								if (cheapest[faU] == -1) //work on the component of U
								{
									cheapest[faU] = CollCheapest[cur + i * (graph->getVertSize() + 1)];
								}
								else
								{
									if (cost < graph->edges[cheapest[faU]].cost)
									{
										cheapest[faU] = CollCheapest[cur + i * (graph->getVertSize() + 1)];
									}
								}

								if (cheapest[faV] == -1) //work on the component of V
								{
									cheapest[faV] = CollCheapest[cur + i * (graph->getVertSize() + 1)];
								}
								else
								{
									if (cost < graph->edges[cheapest[faV]].cost)
									{
										cheapest[faV] = CollCheapest[cur + i * (graph->getVertSize() + 1)];
									}
								}
							}
						}
					}
				}
			}

			
			
			for (int i = 0; i < NumCompos; i++) //把每个成分最近的边加入树中，并收缩边
			{
				int cur = components[i];
				if (cur != 0)
				{
					if (cheapest[cur] != -1)
					{
						//cout << "KKK" << endl;
						int Indu = findFather(father, graph->edges[cheapest[cur]].u);
						int Indv = findFather(father, graph->edges[cheapest[cur]].v);
						//cout << graph->edges[cheapest[cur]].u << "AND" << graph->edges[cheapest[cur]].v << endl;
						father[Indu] = Indv;
						father_no_use = findFather(father, Indu);
						if (!is_element_in_vector(edges_mst, cheapest[cur])) // 确认之前未加入，Boruvka算法的每一轮可能加入重复的边。
						{
							MSTedges.push_back(cheapest[cur]);
							edges_mst.push_back(cheapest[cur]);
							ans += graph->edges[cheapest[cur]].cost;
							//cout << "A "<<Indu<<" B "<<Indv << endl;
							NumEdge += 1;
						}
					}
				}
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Bcast(father.data(), graph->getVertSize() + 1, MPI_INT, 0, MPI_COMM_WORLD); //广播father到各个进程
		MPI_Barrier(MPI_COMM_WORLD);
		
		vector<int> temp2 = {};
		for (int i = 0; i < graph->getVertSize() + 1; i++) { temp2.push_back(findFather(father, i)); }
		std::unordered_set<int> setcomponents2(temp2.begin(), temp2.end()); //更新存储目前的分支
		vector<int> components2(setcomponents2.begin(), setcomponents2.end());
		NumCompos = components2.size(); //更新分支数目
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	if (world_rank != 0) 
	{// cout << "NOT MAIN PROCESS"<<endl;
		return -2; }

	//cout << "THIS IS THE MAIN PROCESS" << "AND ANS IS "<<ans<<endl;
	return ans;
}

void BoruvkaPaSolver::printMST()
{
	for (auto i = MSTedges.begin(); i < MSTedges.end(); i++)
	{
		cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
	}
}
