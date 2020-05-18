#pragma once
#ifndef INF442_P3_KRUSKALCLUSTERING_HPP
#define INF442_P3_KRUSKALCLUSTERING_HPP

#include "Graph.h"
#include "kmeans.hpp"
#include "Kruskal.hpp"

#include <string>
#include <unordered_set>

const int DEFAULT_CLOUD_CAPACITY = 5000;

class KruskalClustering {
private:
	int k;
	int dim;
	GraphInEdge graph;
	KruskalSolver kMSTsolver;
	cloud kMeansSolver;

	vector<unordered_set<int>> clusters;  // for kMSTsolver
public:
	KruskalClustering(int _d, int _k) :k(_k), dim(_d), kMeansSolver(_d, DEFAULT_CLOUD_CAPACITY, _k), graph(), kMSTsolver(&graph) {};  // TODO init kMSTSolver
	void set_k(const int _k) { k = _k; };
	void set_dim(const int _dim) { dim = _dim; };
	void ReadFile(string filename, int nb_clusters, int x_column, int y_column);
	void kMSTsolve(bool with_output);
	void pureKMSTsolve(bool with_output);
	void kMeansSolve(bool with_output);
	double getSilhouette();
};

#endif // INF442_P3_KRUSKALCLUSTERING_HPP