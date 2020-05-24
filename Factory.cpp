#include "Factory.hpp"
#include "Part.hpp"
#include "Pallet.hpp"
#include <fstream>
#include "Definition.h"



Factory::Factory(const std::string & file_name) :_sim_time(-1) {


	// Read file
	std::ifstream fin(file_name);


	fin >> Num_LU_Station;
	fin >> Num_Machine;
	fin >> Part_Info_Size;
	fin >> Num_Parttype;
	fin >> Num_Pallet;
	fin >> Num_Dependency_Pre;
	fin >> Num_Dependency_Suc;
	fin >> LU_Time;

	fin >> MovingTime;

	LU_Time += (MovingTime * 2);

	// Transportation time table
	Transportation_Time = new int *[Num_Machine + 1];
	for (int i = 0; i < Num_Machine + 1; i++)
	{
		Transportation_Time[i] = new int[Num_Machine + 1];
		for (int j = 0; j < Num_Machine + 1; j++)
		{
			Transportation_Time[i][j] = MovingTime * i;

			if (j > i) { Transportation_Time[i][j] = MovingTime * (j + 1); }
			if (i == j) { Transportation_Time[i][j] = 0; }
			if (j < i) { Transportation_Time[i][j] = MovingTime * (i + 1); }
			if (j == (Num_Machine) && i == (Num_Machine - 1)) { Transportation_Time[i][j] = MovingTime * j; }
			if (j == (Num_Machine - 1) && i == (Num_Machine)) { Transportation_Time[i][j] = MovingTime * i; }
		}
	}

	// Pallet & LU statation
	int num_pallet;
	Pallet* pallet_pt;
	std::vector<int> fixture_type(NUM_FIXTURE_TYPE);
	int pallet_idx(0);
	for (int LU_station_idx(0); LU_station_idx < Num_LU_Station; ++LU_station_idx) {
		fin >> num_pallet;
		for (int i(0); i < num_pallet; ++i) {

			for (int j(0); j < NUM_FIXTURE_TYPE; ++j) {
				fin >> fixture_type[j];
			}
			pallet_pt = new Pallet(pallet_idx, LU_station_idx, fixture_type);
			++pallet_idx;
			pallet_list.push_back(pallet_pt);
		}
	}

	//LU Station available time
	int Mac_temp;
	while (true) {
		fin >> Mac_temp;
		Machine_Schedule.push_back(Mac_temp);
		if (Mac_temp == -1) {
			break;
		}
	}

	// Part Generation
	Part* part_pt;
	int part_type;
	int num_part;
	int num_operation;
	bool dependency;
	int due_date;
	int num_alternative_machine;
	int tmp;

	MachiningInfo machine_info;
	std::vector<MachiningInfo> machine_info_list (5); // length: num operation

	int start_idx(0);
	for (int i(0); i < Num_Parttype; ++i) {
		fin >> part_type;
		fin >> num_operation;
		fin >> tmp;

		if (tmp == -1) dependency = true;
		else if (tmp == -2) dependency = false;

		fin >> num_part;
		for (int j(0); j < num_part; ++j) {
			fin >> due_date;
			part_pt = new Part(start_idx + j, part_type, num_operation, due_date, dependency);
			part_list.push_back(part_pt);

		}
		//-------------
		int sum_pt_temp = 0;
		//-------------

		machine_info_list.resize(num_operation);
		for (int k(0); k < num_operation; ++k) {
			fin >> num_alternative_machine;
			machine_info.machine_name.resize(num_alternative_machine);
			machine_info.machine_idx.resize(num_alternative_machine);
			machine_info.processing_time.resize(num_alternative_machine);

			for (int j(0); j < num_alternative_machine; ++j) {

				fin >> machine_info.machine_name[j];
				fin >> machine_info.processing_time[j];
				fin >> machine_info.machine_idx[j];
				machine_info.machine_idx[j] -= 1; // idx starts from 0

		   //-----------------------------
		   /* 1) 파트별로 모든 대안머신의 가공시간 다 더한거 각 파트클래스에저장
			  2) 납기대신 파트별 가공시간합이 적은 파트를 팔렛에 올리도록*/

				sum_pt_temp += machine_info.processing_time[j];
				// printf(" %d ", sum_pt_temp);
				//-----------------------------
			}
			machine_info_list[k] = machine_info;
		}

		//---------------------------
		for (int j(0); j < num_part; ++j) {
			part_list[start_idx + j]->_sum_pt = sum_pt_temp;

			//  printf("part idx : %d, PT : %d for sum of PT per part\n",
			//          part_list[start_idx + j]->_part_idx, part_list[start_idx + j]->_sum_pt);
		}
		//------------------------------

		for (int j(0); j < num_part; ++j) {
			part_list[start_idx + j]->_machining_info_list = machine_info_list;

		}

		// IF DEFENDENCY
		if (dependency) {
			int depend_part_type, post_num_operation;

			fin >> depend_part_type;
			//if(depend_part_type != part_type){ 
				//printf("[error] part types are different\n"); 
			//}
			fin >> post_num_operation;
			fin >> tmp;
			if (tmp != 0) {
				printf("[error] this is not post part\n");
			}

			//-------------
			int sum_pt_temp2 = 0;
			//-------------

			machine_info_list.resize(post_num_operation);

			for (int k(0); k < post_num_operation; ++k) {
				fin >> num_alternative_machine;
				machine_info.machine_name.resize(num_alternative_machine);
				machine_info.machine_idx.resize(num_alternative_machine);
				machine_info.processing_time.resize(num_alternative_machine);

				for (int j(0); j < num_alternative_machine; ++j) {
					fin >> machine_info.machine_name[j];
					fin >> machine_info.processing_time[j];
					fin >> machine_info.machine_idx[j];
					machine_info.machine_idx[j] -= 1; // idx starts from 0

					//------------------------------
					/* 1) 파트별로 모든 대안머신의 가공시간 다 더한거 각 파트클래스에저장
					   2) 납기대신 파트별 가공시간합이 적은 파트를 팔렛에 올리도록*/

					sum_pt_temp2 += machine_info.processing_time[j];
					//printf(" %d ", sum_pt_temp2);
					//-------------------------------
				}
				machine_info_list[k] = machine_info;
			}
			//---------------------------
			printf("\n");
			for (int j(0); j < num_part; ++j) {
				part_list[start_idx + j]->_post_sum_pt = sum_pt_temp2;

				// printf("part idx : %d, PT : %d for sum of PT per part (Dependency) \n",
				//         part_list[start_idx + j]->_part_idx, part_list[start_idx + j]->_post_sum_pt);
			}
			//------------------------------

			for (int j(0); j < num_part; ++j) {
				part_list[start_idx + j]->_dependent_part_type = depend_part_type;
				part_list[start_idx + j]->_post_num_operation = post_num_operation;
				part_list[start_idx + j]->_post_machining_info_list = machine_info_list;
			}
			++i;
		}// END of if(dependency)
		start_idx += num_part;
	}// END of for(Num_Parttype)

	Num_Total_Part = part_list.size();

	//for(int i(0);i < part_list.size(); ++i){
		//part_list[i]->printInfo(i);
	//}

	//for(int i(0); i<pallet_list.size(); ++i){
		//pallet_list[i]->printInfo(i);
		//}

}

Factory::~Factory() {}

bool Factory::All_Done() {
	// TEST
	if (_sim_time > 100000) { return true; }  //event 발생시 다음 time에 할당

	for (int i(0); i < Num_Total_Part; ++i) {
		if (!(part_list[i]->IsDone() && (part_list[i]->_part_loc == loc::Outside))) {
			return false;
		}
	}
	return true;
}

void Factory::ForwardOneStep() {
	++_sim_time; // Tick

	// Increase Pallet time only when it is in process
	for (int i(0); i < Num_Pallet; ++i) {
		//pallet_list[i]->printInfo(i);

		if (pallet_list[i]->IsProcessing()) {
			pallet_list[i]->OneStepForward();
		}

	}
}

void Factory::printAllPartStatus() {
	for (int i(0); i < part_list.size(); ++i) {
		part_list[i]->print_PartStatus(i);
	}
}

void Factory::printAllPalletStatus() {
	for (int i(0); i < pallet_list.size(); ++i) {
		pallet_list[i]->print_PalletInfo(i);
	}
}


void Factory::printInfo() {
	printf("\nFactory Information\n");
	printf("MovingTime, TransTime: %d, %d \n", MovingTime, TransTime);
}

void Factory::printTardiness() {

	/* for(int i(0); i<part_list.size(); ++i){
		 printf("part %d, sim time/due date: %d/%d, tardiness: %d, unloading_time:%d \n",
				 part_list[i]->_part_idx, _sim_time, part_list[i]->_due_time,
				 _sim_time - part_list[i]->_due_time,
				 part_list[i]->_history._unloading_time[0]);}*/

				 //-----------------------
	int makespan = 0;
	int total_tardiness = 0;
	int total_flow_time = 0;
	makespan = _sim_time;
	printf("*makespan(completion time): %d\n", makespan);

	for (int i(0); i < part_list.size(); ++i) {
		//dependency
		if (part_list[i]->_history._unloading_time.size() > 1) {
			printf("part %d, unloading_time/due date: %d/%d, tardiness: %d(dependency) \n",
				part_list[i]->_part_idx,
				part_list[i]->_history._unloading_time[1],
				part_list[i]->_due_time,
				part_list[i]->_history._unloading_time[1] - part_list[i]->_due_time);
			total_tardiness += part_list[i]->_history._unloading_time[1] - part_list[i]->_due_time;
		}
		//general
		if (part_list[i]->_history._unloading_time.size() == 1) {
			printf("part %d, unloading_time/due date: %d/%d, tardiness: %d\n",
				part_list[i]->_part_idx,
				part_list[i]->_history._unloading_time[0],
				part_list[i]->_due_time,
				part_list[i]->_history._unloading_time[0] - part_list[i]->_due_time);
			total_tardiness += part_list[i]->_history._unloading_time[0] - part_list[i]->_due_time;
		}

	}
	printf("*total tardiness : %d\n", total_tardiness);

	for (int i(0); i < part_list.size(); ++i) {
		//dependency 
		if (part_list[i]->_history._unloading_time.size() > 1) {
			printf("part %d, loading_time: %d, unloading_time: %d, flow_time: %d(dependency) \n",
				part_list[i]->_part_idx,
				part_list[i]->_history._loading_time[0],
				part_list[i]->_history._unloading_time[1],
				part_list[i]->_history._unloading_time[1] - part_list[i]->_history._loading_time[0]);
			total_flow_time += part_list[i]->_history._unloading_time[1] - part_list[i]->_history._loading_time[0];
		}
		//general
		if (part_list[i]->_history._unloading_time.size() == 1) {
			printf("part %d, loading_time: %d, unloading_time: %d, flow_time: %d(dependency) \n",
				part_list[i]->_part_idx,
				part_list[i]->_history._loading_time[0],
				part_list[i]->_history._unloading_time[0],
				part_list[i]->_history._unloading_time[0] - part_list[i]->_history._loading_time[0]);
			total_flow_time += part_list[i]->_history._unloading_time[0] - part_list[i]->_history._loading_time[0];
		}
	}
	printf("*total flow time : %d\n", total_flow_time);
	//---------------------
}
