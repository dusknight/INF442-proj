#include <iostream>


#include"GHSMpi.hpp"
#include<time.h>

int main(int argc, char* argv[]) {
	//GraphInEdge gie;
	//gie.ReadFile("test_in.txt");
	
	GHSMPI ghsmpi;
	ghsmpi.init("8192nodep11.txt");
	/*clock_t start = clock();*/
	ghsmpi.run_loop();
	//clock_t end = clock();
	//cout << "It spend " << (end - start) * 1.0 / (CLOCKS_PER_SEC) << " seconds" << endl;
	return 0;
}