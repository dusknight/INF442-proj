//
// Created by Shikang on 4/19/2020.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <assert.h>
#include <unordered_set>

#include "Graph.h"

#define INF 1000000
using namespace std;


const double DEFAULT_COST = 1;

Edge::Edge(int u_, int v_, double cost_): u(u_), v(v_), cost(cost_)
{
}

bool Edge::cmp(Edge a, Edge b)
{
	return a.cost < b.cost;
}

int Graph::getEdgeSize() const { return edgeSize; }
int Graph::getVertSize() const { return vertSize; }

Graph::Graph()
{
	vertSize = 0;
	edgeSize = 0;
}


GraphInEdge::GraphInEdge(): Graph()
{
	graphName = "";
}

GraphInEdge::GraphInEdge(GraphInEdge& gie)
{
	graphName = gie.graphName;
	edges = vector<Edge>(gie.edges);
}

string GraphInEdge::getGraphName() const { return graphName; }

int GraphInEdge::findEdge(int u, int v)
{
	// TODO: implement
	int l = this->edges.size();
	int i = 0;
	while (i < l)
	{
		if (edges[i].u == u)
		{
			if (edges[i].v == v)
			{
				//std::cout << "The edge is located in "<< i <<"th position in edges"<<std::endl;
				return i;
			}
		}
		i += 1;
	}
	return -1;
}

void GraphInEdge::ReadFile(string filename)
{
	string line;
	ifstream ifs(filename);
	if (ifs.is_open())
	{
		int vertCount = 0;
		int blancCount = 0;

		// get first line
		getline(ifs, graphName);
		cout << "[INFO] reading graph: " << graphName << endl;
		// get the other lines
		while (getline(ifs, line))
		{
			if (line.empty() || line == "\r")
			{
				blancCount++;
				continue;
			}
			int node_u, node_v;
			if (blancCount == 2)
			{
				if (line.length() < 2) continue; // if is not edge;
				istringstream iss(line);
				vector<string> results((istream_iterator<std::string>(iss)),
				                       istream_iterator<std::string>());
				node_u = stoi(results.front());
				if (node_u > vertCount) vertCount = node_u;
				//                int level = stoi(results[1]);
				for (int i = 2; i < results.size(); i++)
				{
					string tmp = results[i];
					if (tmp[0] == '[') tmp = tmp.substr(1);
					if (tmp[tmp.length() - 1] == ',' || tmp[tmp.length() - 1] == ']') tmp = tmp.substr(
						0, tmp.length() - 1);
					node_v = stoi(tmp);
					if (node_v > vertCount) vertCount = node_v;
					edges.push_back(Edge(node_u, node_v, DEFAULT_COST));
				}
			}
			// cout << line << '\n';
		}
		ifs.close();
		edgeSize = edges.size();
		vertSize = vertCount;
	}
	else
	{
		cerr << "Cannot open file: " << filename << endl;
	}
}

double GraphInEdge::getEdgeCost(int u, int v)
{
	// TODO
	int l = this->edges.size();
	int i = 0;
	while (i < l)
	{
		if (edges[i].u == u)
		{
			if (edges[i].v == v)
			{
				//std::cout << "The edge is located in " << i << "th position in edges"<<std::endl;
				return edges[i].cost;
			}
		}
		i += 1;
	}

	return INF;
}


int findFather(vector<int>& father, int x)
{
	/*
	 * Union-Find
	 * return the root of Union-find set where x belongs
	 */
	int a = x;
	while (x != father[x])
		x = father[x];
	while (a != father[a])
	{
		int z = a;
		a = father[a];
		father[z] = x;
	}
	return x;
}

void GraphInEdge::addEdge(Edge e)
{
	if (findEdge(e.u, e.v)) return;
	edges.push_back(e);
	edgeSize ++;
	if (e.u > vertSize) vertSize = e.u;
	if (e.v > vertSize) vertSize = e.v;
}


KruskalSolver::KruskalSolver()
{
	graph = new GraphInEdge();
	MSTCost = -1;
}

KruskalSolver::~KruskalSolver()
{
	delete graph;
}

KruskalSolver::KruskalSolver(GraphInEdge* graph)
{
	this->graph = graph;
	MSTCost = -1;
}

void KruskalSolver::ReadFile(string filename)
{
	graph->ReadFile(filename);
}

double KruskalSolver::CalcMST()
{
	/*
	 * Kruskal's algorithm : find MST
	 * Params:
	 * return: sum of weights of MST
	 */
	MSTedges.clear();
	vector<int> father(graph->getVertSize() + 1); //并查集数组, starts from 1 to vertSize+1
	double ans = 0; //所求边权之和
	int NumEdge = 0; //记录最小生成树边数
	for (int i = 0; i < graph->getVertSize() + 1; i++) //初始化并查集
		father[i] = i;
	std::sort(graph->edges.begin(), graph->edges.end(), Edge::cmp); //所有边按边权从小到大排序
	for (int i = 0; i < graph->getEdgeSize(); ++i) //枚举所有边
	{
		int faU = findFather(father, graph->edges[i].u); //查询端点u所在集合的根结点
		int faV = findFather(father, graph->edges[i].v); //查询端点v所在集合的根结点
		if (faU != faV)
		{
			//如果不在一个集合中
			father[faU] = faV; //合并集合（相当于把测试边加入到最小生成树）
			ans += graph->edges[i].cost;
			NumEdge++; //当前生成树边数加1
			MSTedges.push_back(i);
			if (NumEdge == graph->getVertSize() - 1) //边数等于顶点数减1，算法结束
				break;
		}
	}
	if (NumEdge != graph->getVertSize() - 1) //无法连通时返回-1
		return -1;
	else
	{
		MSTCost = ans;
		return ans; //返回最小生成树边权之和
	}
}

void KruskalSolver::printMST()
{
	for (auto i = MSTedges.begin(); i < MSTedges.end(); i++)
	{
		cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
	}
}

PrimSolver::PrimSolver()
{
	graph = new GraphInEdge();
	MSTCost = -1;
}

PrimSolver::~PrimSolver()
{
	delete graph;
}

PrimSolver::PrimSolver(GraphInEdge* graph)
{
	this->graph = graph;
	MSTCost = -1;
}

void PrimSolver::ReadFile(string filename)
{
	graph->ReadFile(filename);
}

double PrimSolver::CalcMST()
{
	/*
	 * Prime's algorithm : find MST
	 * Params:
	 * return: sum of weights of MST
	 */
	MSTedges.clear();
	double ans = 0; //所求边权之和
	const int taille = graph->getVertSize();

	int startpoint = 0; // 以顶点0作为出发点
	vector<bool> visited = {0, 1,};
	for (int i = 1; i < taille; i++)
	{
		// 初始化访问过的节点的记录
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

	
	for (int i = 1; i < taille; i++) //遍历节点（最后一个不必遍历）
	{
		int cur = 1;  //初始化要加入的下一个顶点的index
		double minor = INF;
		for (int j = 1; j < taille + 1; j++) //找到未加入的节点中距离当前已得森林中最近的点
		{
			if ((!visited[j]) && costs[j] < minor)
			{
				minor = costs[j];
				cur = j;
			}
		}
		if (minor == INF)
		{
			cout << " Not connected" << endl;
			return -1;
		}

		visited[cur] = true;
		int edge_number = 0;
		if (graph->getEdgeCost(cur, closest[cur]) > graph->getEdgeCost(closest[cur], cur)) // 选择加入的边
		{
			edge_number = graph->findEdge(closest[cur], cur);
		}
		else
		{
			edge_number = graph->findEdge(cur, closest[cur]);
		}
		MSTedges.push_back(edge_number);

		ans += minor;
		for (int j = 1; j < taille + 1; j++) //更新每个未访问的点到已知部分的最小距离
		{
			if (!visited[j] && costs[j] > min(graph->getEdgeCost(j, cur), graph->getEdgeCost(cur, j)))
			{
				costs[j] = min(graph->getEdgeCost(j, cur), graph->getEdgeCost(cur, j));
				closest[j] = cur;
			}
		}
	}

	MSTCost = ans;
	return ans; //返回最小生成树边权之和
}

void PrimSolver::printMST()
{
	for (auto i = MSTedges.begin(); i < MSTedges.end(); i++)
	{
		cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
	}
}

BoruvkaSolver::BoruvkaSolver()
{
	graph = new GraphInEdge();
	MSTCost = -1;
}

BoruvkaSolver::~BoruvkaSolver()
{
	delete graph;
}

BoruvkaSolver::BoruvkaSolver(GraphInEdge* graph)
{
	this->graph = graph;
	MSTCost = -1;
}

void BoruvkaSolver::ReadFile(string filename)
{
	graph->ReadFile(filename);
}

double BoruvkaSolver::CalcMST()
{
	/*
	 * Boruvka's algorithm : find MST
	 * Params:
	 * return: sum of weights of MST
	 */
	MSTedges.clear();
	assert(graph->getVertSize()); //确保输入不为空
	double ans = 0; //所求边权之和
	int NumEdge = 0; //记录最小生成树边数
	const int TailleEdges = graph->edges.size(); //图的边数

	vector<int> father(graph->getVertSize() + 1);//初始化并查集
	for (int i = 0; i < graph->getVertSize() + 1; i++) { father[i] = i; }

	vector<int> cheapest(graph->getVertSize() + 1);//每个components在当前步要加入的边


	int NumCompos = graph->getVertSize() + 1; //连通分支数目+1


	while (NumCompos > 2)
	{
		vector<int> temp ={};
		for (int i=0; i < graph->getVertSize() + 1; i++)
		{
			temp.push_back(findFather(father, i));
		}
		std::unordered_multiset<int> setcomponents(temp.begin(), temp.end()); //存储目前的分支
		vector<int> components(setcomponents.begin(), setcomponents.end());
		NumCompos = components.size();

		for (int i = 0; i < graph->getVertSize() + 1; i++) { cheapest[i] = -1; }//初始化每个components在当前步要加入的边

		for (int i = 0; i < TailleEdges; i++) //遍历边
		{
			int faU = findFather(father, graph->edges[i].u);           //查询端点u所在集合的根结点
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

		for (int i = 0; i < NumCompos; i++)
		{
			int cur = components[i];
			if (cur != 0)
			{
				if (cheapest[cur] != -1)
				{
					int Indu = findFather(father, graph->edges[cheapest[cur]].u);
					int Indv = findFather(father, graph->edges[cheapest[cur]].v);
					father[Indu] = Indv;
					MSTedges.push_back(cheapest[cur]);
					NumEdge += 1;
					ans += graph->edges[cheapest[cur]].cost;
				}
			}
		}

		vector<int> temp2 = {};
		for (int i=0; i < graph->getVertSize() + 1; i++)
		{
			temp2.push_back(findFather(father, i));
		}
		std::unordered_multiset<int> setcomponents2(temp2.begin(), temp2.end()); //存储目前的分支
		vector<int> components2(setcomponents2.begin(), setcomponents2.end());
		NumCompos = components2.size();
		
	}
	MSTCost = ans;
	return ans;
}

void BoruvkaSolver::printMST()
{
	for (auto i = MSTedges.begin(); i < MSTedges.end(); i++)
	{
		cout << graph->edges[*i].u << " -- " << graph->edges[*i].v << endl;
	}
}

int main()
{
	string filename = "test_in.txt";
	GraphInEdge* gie = new GraphInEdge();
	gie->ReadFile(filename);
	//gie->addEdge(Edge(1, 2, 1));
	//gie->addEdge(Edge(2, 3, 1));
	//gie->addEdge(Edge(3, 1, 1));
	//    gie.addEdge(Edge(4, 5, 1));
	//    gie.addEdge(Edge(1, 3, 1));
	//    gie.addEdge(Edge(1, 4, 1));
	cout << gie->getGraphName() << endl;

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
	return 0;
}
