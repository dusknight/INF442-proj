#include "KruskalClustering.hpp"

int main(int argc, char** argv) {
	if (argc != 6) {
		std::cerr << "Usage: " << argv[0] << " csv nb_clusters dim x_column y_column" << std::endl;
		std::exit(1);
	}
	std::string csv_filename = argv[1];
	int nb_clusters = std::stoi(argv[2]);
	int dim = std::stoi(argv[3]);
	int x_column = std::stoi(argv[4]);
	int y_column = std::stoi(argv[5]);

	KruskalClustering kcl(dim, nb_clusters), kcl2(dim, nb_clusters);
	kcl.ReadFile(csv_filename, nb_clusters, x_column, y_column);
	kcl.kMeansSolve();

	kcl2.ReadFile(csv_filename, nb_clusters, x_column, y_column);
	kcl2.kMSTsolve();

	return 0;
	//return test_TD3(argc, argv);
}