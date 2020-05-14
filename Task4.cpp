#include"GHSMpi.hpp"

int main(int argc, char* argv[]) {
	GraphInEdge gie;
	gie.ReadFile("test_in.txt");
	
	GHSMPI ghsmpi;
	ghsmpi.init();
	ghsmpi.run_loop();

	return 0;
}