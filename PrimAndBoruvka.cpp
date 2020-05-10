//
// Created by Shikang on 5/9/2020.
//

#include "PrimAndBoruvka.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <assert.h>
#include <unordered_set>

PrimSolver::PrimSolver()
{
    graph = new GraphInEdge();
    MSTCost = -1;
}

PrimSolver::~PrimSolver()
{
    // delete graph;
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
    vector<bool> visited = { 0, 1, };
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
    // delete graph;
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

    int it_count = 0;
    vector<int> edges_mst = {};
    while (NumCompos > 2) //当分支数目大于等于2
    {
        vector<int> temp = {};
        for (int i = 0; i < graph->getVertSize() + 1; i++)
        {
            temp.push_back(findFather(father, i));
        }
        std::unordered_set<int> setcomponents(temp.begin(), temp.end()); //存储目前的分支
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
                    if (!is_element_in_vector(edges_mst, cheapest[cur])) // 确认之前未加入，Boruvka算法的每一轮可能加入重复的边。
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
        for (int i = 0; i < graph->getVertSize() + 1; i++)
        {
            temp2.push_back(findFather(father, i));
        }

        std::unordered_set<int> setcomponents2(temp2.begin(), temp2.end()); //更新存储目前的分支
        vector<int> components2(setcomponents2.begin(), setcomponents2.end());
        NumCompos = components2.size();//更新分支数目


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
