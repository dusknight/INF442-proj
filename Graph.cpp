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

#include "Graph.h"

using namespace std;

const double DEFAULT_COST=1;

Edge::Edge(int u_, int v_, double cost_):u(u_), v(v_), cost(cost_){}

bool Edge::cmp(Edge a, Edge b){
    return a.cost < b.cost;
}

int Graph::getEdgeSize() const {return edgeSize;}
int Graph::getVertSize() const {return vertSize;}
Graph::Graph() {vertSize=0; edgeSize=0;}


GraphInEdge::GraphInEdge():Graph(){
    graphName="";
}

string GraphInEdge::getGraphName() const {return graphName;}

bool GraphInEdge::findEdge(int u, int v) {
// TODO: implement
return false;
}

void GraphInEdge::ReadFile(string filename) {
    string line;
    ifstream ifs(filename);
    if (ifs.is_open()){
        int vertCount = 0;
        int blancCount = 0;

        // get first line
        getline(ifs, graphName);
        cout<<"[INFO] reading graph: "<<graphName<<endl;
        // get the other lines
        while ( getline (ifs,line) ){
            if(line.empty() or line=="\r") {
                blancCount++;
                continue;
            }
            int node_u, node_v;
            if(blancCount == 2){
                if(line.length() < 2) continue;  // if is not edge;
                istringstream iss(line);
                vector<string> results((istream_iterator<std::string>(iss)),
                        istream_iterator<std::string>());
                node_u = stoi(results.front());
                if (node_u > vertCount) vertCount = node_u;
//                int level = stoi(results[1]);
                for (int i=2; i<results.size(); i++){
                    string tmp = results[i];
                    if (tmp[0] == '[') tmp = tmp.substr(1);
                    if (tmp[tmp.length()-1] == ',' || tmp[tmp.length()-1] == ']') tmp = tmp.substr(0, tmp.length()-1);
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
    else{
        cerr<<"Cannot open file: "<<filename<<endl;
    }
}

double GraphInEdge::getEdgeCost(int u, int v) {
    // TODO: inplement it !
    return 0;
}


int findFather(vector<int> father, int x){
    /*
     * Union-Find
     * return the root of Union-find set where x belongs
     */
    int a = x;
    while (x != father[x])
        x = father[x];
    while (a != father[a]) {
        int z = a;
        a = father[a];
        father[z] = x;
    }
    return x;
}


int GraphInEdge::Kruskal(){
    /*
     * Kruskal's algorithm : find MST
     * params:
     * return: sum of weights of MST
     */
    /*
    param
    n:                         图的顶点个数
    m:                         图中边的个数
    E:                         边的集合
    */
    int m = edgeSize;
    int n = vertSize;
    vector<int> father(vertSize);                                 //并查集数组
    int ans = 0;                                           //所求边权之和
    int NumEdge = 0;                                       //记录最小生成树边数
    for (int i = 0; i < n; i++)                            //初始化并查集
        father[i] = i;
    sort(edges.begin(), edges.end(), Edge::cmp);                         //所有边按边权从小到大排序
    for (int i = 0; i < m; ++i)                            //枚举所有边
    {
        int faU = findFather(father, edges[i].u);           //查询端点u所在集合的根结点
        int faV = findFather(father, edges[i].v);           //查询端点v所在集合的根结点
        if (faU != faV) {                               //如果不在一个集合中
            father[faU] = faV;                       //合并集合（相当于把测试边加入到最小生成树）
            ans += edges[i].cost;
            NumEdge++;                               //当前生成树边数加1
            if (NumEdge == n - 1)                    //边数等于顶点数减1，算法结束
                break;
        }
    }
    if (NumEdge != n - 1)                                  //无法连通时返回-1
        return -1;
    else
        return ans;                                     //返回最小生成树边权之和
}


int main(){
    string filename = "test_in.txt";
    GraphInEdge gie;
    gie.ReadFile(filename);
    cout<<gie.getGraphName();
}