#include "KruskalClustering.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <unordered_set>

void KruskalClustering::ReadFile(string filename, int nb_clusters, int x_column,
                                 int y_column) {
    std::ifstream is(filename);
    assert(is.is_open());

    // read header line
    std::string header_line;
    std::getline(is, header_line);
    std::vector<std::string> names;

    const int d = std::count(header_line.begin(), header_line.end(), '\t');
    // const int nmax = 150;
    const int k = nb_clusters;

    // point to read into
    point p;

    // labels to cycle through
    int label = 0;

    // while not at end of file
    while (is.peek() != EOF) {
        // read new points
        for (int m = 0; m < d; m++) {
            is >> p.coords[m];
        }

        kMeansSolver.add_point(p, label);

        label = (label + 1) % k;

        // read ground-truth labels
        // unused in normal operation
        std::string next_name;
        is >> next_name;
        names.push_back(next_name);

        // consume \n
        is.get();
    }

    // init Graph
    for (int i = 0; i < kMeansSolver.get_n(); i++) {
        for (int j = i + 1; j < kMeansSolver.get_n(); j++) {
            double dist =
                kMeansSolver.get_point(i).dist(kMeansSolver.get_point(j));
            graph.addEdge(Edge(
                i + 1, j + 1,
                dist));  // attention! in the graph, vertex id starts from 1
            graph.addEdge(Edge(j + 1, i + 1, dist));
        }
    }
}

void KruskalClustering::kMSTsolve(bool with_output) {
    pureKMSTsolve(with_output);
    kMeansSolver._kmeans_calc();
    if (with_output) {
        std::cout << "Intracluster variance after MST init-ed k-means: "
                  << kMeansSolver.intracluster_variance() << std::endl;
        std::cout << "Sihouette value of k=" << k
                  << " is: " << kMeansSolver.get_sihouette() << "\n"
                  << std::endl;
    }
}

void KruskalClustering::pureKMSTsolve(bool with_output) {
    if (with_output) {
        std::cout << "=====================================" << std::endl;
        std::cout << "                 MST                 " << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "Intracluster variance before k-means: "
                  << kMeansSolver.intracluster_variance() << std::endl;
    }

    // solve MST
    kMSTsolver.CalcMST();
    vector<Edge> mst = kMSTsolver.getMSTedges();
    sort(mst.begin(), mst.end(), [](Edge a, Edge b) {
        return a.cost > b.cost;
    });  // from greatest to smallest

    clusters.clear();
    clusters.resize(k);
    for (int i = k - 1; i < mst.size(); i++) {
        Edge e = mst[i];
        for (auto is = clusters.begin(); is != clusters.end(); is++) {
            auto u_ptr = is->find(e.u);
            if (u_ptr != is->end()) {
                is->insert(e.v);
                break;
            }
            auto v_ptr = is->find(e.v);
            if (v_ptr != is->end()) {
                is->insert(e.u);
                break;
            }
            // make a new set
            is->insert(e.u);
            is->insert(e.v);
            break;
        }
    }

    // convert back to points
    for (int ic = 0; ic < clusters.size(); ic++) {
        for (auto ip = clusters[ic].begin(); ip != clusters[ic].end(); ip++) {
            kMeansSolver.get_point((*ip) - 1).label = ic;
            // kMeansSolver.get_point((*ip) ).label = ic;
        }
    }

    kMeansSolver.set_centroid_centers();
    // kMeansSolver.init_forgy();
    // kMeansSolver._kmeans_calc();
    // kMeansSolver.kmeans();
    if (with_output) {
        std::cout << "Intracluster variance after MST initilization: "
                  << kMeansSolver.intracluster_variance() << std::endl;
        std::cout << "Sihouette value of k=" << k
                  << " is: " << kMeansSolver.get_sihouette() << std::endl;
    }
}

void KruskalClustering::kMeansSolve(bool with_output) {
    if (with_output) {
        std::cout << "=====================================" << std::endl;
        std::cout << "      K means algorithm -  Forgy     " << std::endl;
        std::cout << "=====================================" << std::endl;
        // execute k-means algorithm
        std::cout << "Intracluster variance before k-means: "
                  << kMeansSolver.intracluster_variance() << std::endl;
    }
    kMeansSolver.init_forgy();
    kMeansSolver._kmeans_calc();
    // kMeansSolver.kmeans();
    if (with_output) {
        std::cout << "Intracluster variance after k-means: "
                  << kMeansSolver.intracluster_variance() << std::endl;
        std::cout << "Sihouette value of k=" << k
                  << " is: " << kMeansSolver.get_sihouette() << std::endl;
    }
}

double KruskalClustering::getSilhouette() {
    return kMeansSolver.get_sihouette();
}
