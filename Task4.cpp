#include"GHSMpi.hpp"

int main(int argc, char* argv[]) {
	GraphInEdge gie;
	gie.ReadFile("test_in.txt");
	
	GHSNode gn;
	gn.RUN(0, NULL);


	return 0;
}