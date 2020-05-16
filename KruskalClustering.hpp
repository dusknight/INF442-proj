#pragma once
#include "Graph.h"
#include "Kruskal.hpp"
#include "kmeans.hpp"

#include <string>

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
	void kMSTsolve();
	void kMeansSolve();
	void compareSilhouette();
};