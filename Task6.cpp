// grid search for best cluster nbs

#include "KruskalClustering.hpp"

int main(int argc, char** argv) {
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0]
                  << " csv search_start search_end dim x_column y_column"
                  << std::endl;
        std::exit(1);
    }
    std::string csv_filename = argv[1];
    int search_start = std::stoi(argv[2]);
    int search_end = std::stoi(argv[3]);
    int dim = std::stoi(argv[4]);
    int x_column = std::stoi(argv[5]);
    int y_column = std::stoi(argv[6]);

    assert(search_start <= search_end);
    int max_k = -1;
    double max_sihouette = -1;
    for (int nb_clusters = search_start; nb_clusters <= search_end;
         nb_clusters++) {
        KruskalClustering kcl(dim, nb_clusters);
        kcl.ReadFile(csv_filename, nb_clusters, x_column, y_column);
        kcl.kMSTsolve(false);
        // kcl.pureKMSTsolve(false);

        double sihouette = kcl.getSilhouette();
        cout << "[   ] k = " << nb_clusters << ",\tSihouette = " << sihouette
             << endl;
        if (sihouette >= max_sihouette) {
            max_sihouette = sihouette;
            max_k = nb_clusters;
        }
    }
    cout << "[---] Best parameter : nb_cluster = " << max_k
         << ",\tSihouette = " << max_sihouette << endl;

    return 0;
}
