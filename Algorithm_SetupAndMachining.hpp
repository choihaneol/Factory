#pragma once

#ifndef ALGORITHM_SETUP_AND_MACHINING
#define ALGORITHM_SETUP_AND_MACHINING

#include <vector>

class Pallet;
class Part;
class Factory;

class Algorithm_SetupAndMachining {
public:
	Algorithm_SetupAndMachining(const Factory & factory);
	~Algorithm_SetupAndMachining() {}

	void run(int curr_time, const std::vector<Pallet*> & pallet_list);
	void printInfo();

private:
	int _num_Machine;
	int _MovingTime;

	std::vector<bool> machine_usage;
	std::vector<int> machine_processing_time;
	std::vector<int> machine_current_time;
	std::vector<int> machine_engaged_pallet_idx;
	std::vector<Part*> machine_processing_part;

	//------------------
	std::vector<int> machine_pre_pallet;
	Part* current_part;
	//------------------

	void _OperationTime1(const std::vector<Pallet*> & pallet_list); //SOPT 
	void _OperationTime2(const std::vector<Pallet*> & pallet_list); //EDD
	void _OperationTime3(const std::vector<Pallet*> & pallet_list); //STPT
	void _OperationTime4(const std::vector<Pallet*> & pallet_list); //MWKR
	void _OperationTime5(int curr_time, const std::vector<Pallet*> & pallet_list); //MST
	void _OperationTime6(int curr_time, const std::vector<Pallet*> & pallet_list); //MDD
	void _Update(const std::vector<Pallet*> & pallet_list);
};
#endif

