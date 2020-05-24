#include <iostream>
#include <vector>
#include <tchar.h>
#include "Part.hpp"
#include "Pallet.hpp"
#include "Factory.hpp"
//#include <Configuration.h>
#include "Algorithm_Palletizing.hpp"
#include "Algorithm_LoadingUnloading.hpp"
#include "Algorithm_SetupAndMachining.hpp"

//--------------
#include <fstream>
using namespace std;
//--------------

int main(int argc, char* argv[]){  //    <-- int main(int argc, char** argv)



	//std::string file_name = "TEST_DATA_0920.txt";
	ifstream fin(argv[1]);  //    <-- std::string file_name = THIS_COM"input_data/TEST_DATA_0920.txt";
	std::string file_name = argv[1];

							
	//ofstream fout(argv[2]);



	// Reading files and setup problem
	Factory factory(file_name);


	//factory.printInfo();

	// Construct Algorithm
	Algorithm_Palletizing palletizing;
	Algorithm_LoadingUnloading loading_unloading(factory);
	Algorithm_SetupAndMachining setup_and_machining(factory);

	// Simulation Starts
	while (!factory.All_Done()) {
		// Forward Onestep 
		factory.ForwardOneStep(); // 1 Tick
		printf("Simulation Time: %d\n", factory._sim_time);

		// Palletizing
		palletizing.run(factory.part_list, factory.pallet_list);

		// Loading
		loading_unloading.run(factory._sim_time, factory.pallet_list);

		// Moving and Machining 
		setup_and_machining.run(factory._sim_time, factory.pallet_list);

		if (factory._sim_time % (factory.LU_Time - 19) == 0) {
			factory.printAllPartStatus();
			factory.printAllPalletStatus();
		}
		printf("\n");
	}// End of Sim loop




	printf("****** Simulation is done\n");
	factory.printTardiness();   
	return 0;
}
