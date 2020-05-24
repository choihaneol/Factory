#include "Algorithm_LoadingUnloading.hpp"
#include "Pallet.hpp"
#include "Factory.hpp"
#include "Definition.h"

Algorithm_LoadingUnloading::Algorithm_LoadingUnloading(const Factory & factory) {
	_num_LU_station = factory.Num_LU_Station;

	LU_station_usage.clear();
	LU_station_task_type.clear();
	LU_station_using_time.clear();
	LU_station_engaged_pallet_idx.clear();

	LU_station_usage.resize(_num_LU_station, false);
	LU_station_task_type.resize(_num_LU_station, 0);
	LU_station_using_time.resize(_num_LU_station, 0);
	LU_station_engaged_pallet_idx.resize(_num_LU_station, -1);
	_LU_time = factory.LU_Time;
	_moving_time = factory.MovingTime;
}

Algorithm_LoadingUnloading::~Algorithm_LoadingUnloading() {
}


void Algorithm_LoadingUnloading::run(int curr_time, const std::vector<Pallet*> & pallet_list) {
	_Update(curr_time, pallet_list);
	_FirstInFirstOut(curr_time, pallet_list);

	//print_LUStationInfo();
}
void Algorithm_LoadingUnloading::_Update(int curr_time, const std::vector<Pallet*> & pallet_list) {
	for (int i(0); i < _num_LU_station; ++i) {
		if (LU_station_usage[i]) { // LU station is used

			// Is there any completed LU task
			if (LU_station_using_time[i] == _LU_time - 1) {
				LU_station_usage[i] = false;
				// Unloading
				if (LU_station_task_type[i] == -1) {// If it was unloading
					pallet_list[LU_station_engaged_pallet_idx[i]]->_pallet_loc
						= loc::Outside;
					pallet_list[LU_station_engaged_pallet_idx[i]]->Empty_pallet(curr_time);

					LU_station_task_type[i] = 0;
				}
				// Loading
				if (LU_station_task_type[i] == 1) {// If it was loading
					pallet_list[LU_station_engaged_pallet_idx[i]]->
						LocationUpdate(loc::Buffer, pallet_list[LU_station_engaged_pallet_idx[i]]->_pallet_idx);
					LU_station_task_type[i] = 0;
				}
				continue;
			}
			++LU_station_using_time[i];
		}
	} //END of LU station loop

	// Unloading ends: 
	// Empty pallet
	// if there is dependency, then switch it to post part
}


void Algorithm_LoadingUnloading::_FirstInFirstOut(int curr_time, const std::vector<Pallet*> &
	pallet_list) {

	for (int i(0); i < pallet_list.size(); ++i) {
		// pallet_list[i]->printInfo(i);
		// pallet_list[i]->print_PalletInfo(i);

	}

	for (int i(0); i < _num_LU_station; ++i) {
		if (!LU_station_usage[i]) { // LU station is available
			// Unloading
			std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
			while (pl_iter != pallet_list.end()) {
				if (((*pl_iter)->_pallet_loc == loc::Buffer) &&
					(!(*pl_iter)->_in_process)) {

					if ((*pl_iter)->Loaded_Part_HalfDone()) { // Unloading is required
						// LU station update
						LU_station_usage[i] = true;
						LU_station_task_type[i] = -1;
						LU_station_using_time[i] = 0;
						LU_station_engaged_pallet_idx[i] = (*pl_iter)->_pallet_idx;

						// Pallet update
						(*pl_iter)->_in_process = true;
						(*pl_iter)->_process_name = process::Unloading;
						(*pl_iter)->_process_duration = _LU_time - 1;
						(*pl_iter)->_current_processing_time = 0;
						(*pl_iter)->LocationUpdate(loc::LoadingStation, (*pl_iter)->_pallet_idx); //-------------------
						break;
					}
				}
				++pl_iter;
			}
		} // End of Unloading 


		if (!LU_station_usage[i]) { // LU station is available
			// Loading
			std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
			while (pl_iter != pallet_list.end()) {
				if (((*pl_iter)->_pallet_loc == loc::Outside) &&
					(!(*pl_iter)->_in_process)) {

					if ((*pl_iter)->IsThereLoadedPart()) { // Loading is required
						// LU station update
						LU_station_usage[i] = true;
						LU_station_task_type[i] = 1;
						LU_station_using_time[i] = 0;
						LU_station_engaged_pallet_idx[i] = (*pl_iter)->_pallet_idx;

						// Pallet update
						(*pl_iter)->_in_process = true;
						(*pl_iter)->_process_name = process::Loading;
						(*pl_iter)->_process_duration = _LU_time - 1;
						(*pl_iter)->_current_processing_time = 0;
						(*pl_iter)->LocationUpdate(loc::LoadingStation, (*pl_iter)->_pallet_idx);
						//------------------------
						//여기에서  curr time 을 끌어와서  history에 loading time에  저장해 
						//그럼 일단 모든 언로딩시간이 다 들어갈꺼야. 그중에서 제일 마지막 언로딩 시간들을 출력하면 되
						(*pl_iter)->SaveCallingTime(curr_time);
						//-----------------------

						break;
					}
				}
				++pl_iter;
			}
		} // End of Loading
	} //END of LU station loop

	//for(int i(0); i<_num_LU_station; ++i){ printInfo(i); }
}

void Algorithm_LoadingUnloading::print_LUStationInfo() {
	for (int i(0); i < LU_station_usage.size(); ++i) {
		printf("LU station %d,  task type: %d, using_time: %d, engaged_pallet_idx:%d\n",
			i, LU_station_task_type[i],
			LU_station_using_time[i],
			LU_station_engaged_pallet_idx[i]);
	}
}
