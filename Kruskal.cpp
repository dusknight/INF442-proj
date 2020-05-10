//
// Created by Shikang on 5/9/2020.
//

#include "Kruskal.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <assert.h>
#include <unordered_set>

KruskalSolver::KruskalSolver()
{
    graph = new GraphInEdge();
    MSTCost = -1;
}

KruskalSolver::~KruskalSolver()
{
    // delete graph;
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
