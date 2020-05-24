#include "Pallet.hpp"
#include "iostream"
#include "Definition.h"
#include "Part.hpp"

Pallet::Pallet(int pallet_idx, int LU_station_idx, std::vector<int> fixture_type) :
	_pallet_idx(pallet_idx), _ini_station_idx(LU_station_idx), _in_process(false),
	_pallet_loc(loc::Outside) {
	_process_name = -1;
	_fixture_type = fixture_type;
	_loaded_part.resize(fixture_type.size());

	for (int i(0); i < _loaded_part.size(); ++i) {
		_loaded_part[i] = NULL;
	}
}


//-------------------------------------
void Pallet::LocationUpdate(int loc, int plt_idx) {

	printf("(location update) plt_idx%d, _pre_mac : %d, pre_loc : %s, current_loc : %s\n",
		plt_idx, _pre_mac, loc::printLocation(_pallet_loc).c_str(), loc::printLocation(loc).c_str());

	for (int i(4); i < 6; ++i) {
		if (_pre_mac == i) {
			printf("_pre_mac %s\n", loc::printLocation(_pre_mac).c_str());
		}
	}

	_pallet_loc = loc;

	for (int i(0); i < _loaded_part.size(); ++i) {
		if (_loaded_part[i]) {
			_loaded_part[i]->_part_loc = loc;
		}
	}
}

void Pallet::LocationUpdate_Mac1(int loc, Pallet* selected_plt, int pre_pallet, int shortest_processing_time) {
	int movingtime = 5 * 2;

	printf("(location update_Mac0) current(selected) pallet : %d, pre pallet : %d, current loc : %s, pre mac : %d \n",
		selected_plt->_pallet_idx, pre_pallet, loc::printLocation(loc).c_str(), _pre_mac);
	printf("machine processing end!!!!\n");

	//compare current pallet with pre pallet in these machine,
	//and just processed machine of current pallet with selected machine of selected pallet to be processed 
	//_pre_mac : machine0 = 4, machine1 = 5, machine2 = 6 (Definition.hpp)
	if (_pre_mac != loc || selected_plt->_pallet_idx != pre_pallet) {  // if the current machine is not same with the pre machine in terms of the current pallet
		printf("there is pallet transportation\n");
		shortest_processing_time = shortest_processing_time + movingtime;
	}
	else if (_pre_mac == loc && selected_plt->_pallet_idx == pre_pallet) {
		printf("there is no pallet transportation\n");
	}

	selected_plt->_spt_temp = shortest_processing_time;

	_pallet_loc = loc;

	for (int i(0); i < _loaded_part.size(); ++i) {
		if (_loaded_part[i]) {
			_loaded_part[i]->_part_loc = loc;
		}
	}
}

void Pallet::LocationUpdate_Mac2(int loc, Pallet* selected_plt, int pre_pallet, int shortest_processing_time) {
	int movingtime = 5 * 2;

	printf("(location update_Mac1) current(selected) pallet : %d, pre pallet : %d, current loc : %s, pre mac : %d \n",
		selected_plt->_pallet_idx, pre_pallet, loc::printLocation(loc).c_str(), _pre_mac);
	printf("machine processing end!!!!\n");

	//compare current pallet with pre pallet in these machine, 
	//and just processed machine of current pallet with selected machine of selected pallet to be processed 
	//_pre_mac : machine0 = 4, machine1 = 5, machine2 = 6 (Definition.hpp)
	if (_pre_mac != loc || selected_plt->_pallet_idx != pre_pallet) {  // if the current machine is not same with the pre machine in terms of the current pallet 
		printf("there is pallet transportation\n");
		shortest_processing_time = shortest_processing_time + movingtime;
	}
	else if (_pre_mac == loc && selected_plt->_pallet_idx == pre_pallet) {
		printf("there is no pallet transportation\n");
	}

	selected_plt->_spt_temp = shortest_processing_time;

	_pallet_loc = loc;

	for (int i(0); i < _loaded_part.size(); ++i) {
		if (_loaded_part[i]) {
			_loaded_part[i]->_part_loc = loc;
		}
	}
}


void Pallet::LocationUpdate_Mac3(int loc, Pallet* selected_plt, int pre_pallet, int shortest_processing_time) {
	int movingtime = 5 * 2;

	printf("(location update_Mac3) current(selected) pallet : %d, pre pallet : %d, current loc : %s, pre mac : %d \n",
		selected_plt->_pallet_idx, pre_pallet, loc::printLocation(loc).c_str(), _pre_mac);
	printf("machine processing end!!!!\n");

	//compare current pallet with pre pallet in these machine, 
	//and just processed machine of current pallet with selected machine of selected pallet to be processed 
	//_pre_mac : machine0 = 4, machine1 = 5, machine2 = 6 (Definition.hpp)
	if (_pre_mac != loc || selected_plt->_pallet_idx != pre_pallet) { // if the current machine is not same with the pre machine in terms of the current pallet 
		printf("there is pallet transportation\n");
		shortest_processing_time = shortest_processing_time + movingtime;
	}
	else if (_pre_mac == loc && selected_plt->_pallet_idx == pre_pallet) {
		printf("there is no pallet transportation\n");
	}

	selected_plt->_spt_temp = shortest_processing_time;

	_pallet_loc = loc;

	for (int i(0); i < _loaded_part.size(); ++i) {
		if (_loaded_part[i]) {
			_loaded_part[i]->_part_loc = loc;
		}
	}
}


//----------------------------------------

/*void Pallet::LocationUpdate(int loc){
	_pallet_loc = loc;
	for(int i(0); i<_loaded_part.size(); ++i){
		if(_loaded_part[i]){
			_loaded_part[i]->_part_loc = loc;
		}
	}
}*/

Pallet::~Pallet() {  }

bool Pallet::EngagePallet(int ProcessName, int ProcessDuration) {

	if (_in_process) { // Already in process
		printf("[Error - Pallet] The pallet is already in process\n");
		return false;
	}
	_process_duration = ProcessDuration;
	_process_name = ProcessName;
	_current_processing_time = 0;

	return true;
}
void Pallet::Empty_pallet(int curr_time) {
	//if pallet is empty, ready at outside.
	//if the part in pallet is post dependency part, do SwitchTOPostPart function

	std::vector<Part*>::iterator part_iter = _loaded_part.begin();
	while (part_iter != _loaded_part.end()) {
		if ((*part_iter)) { // there is loaded part
			(*part_iter)->_part_loc = loc::Outside;
			(*part_iter)->_pallet_idx = -1;
			(*part_iter)->_history._unloading_time.push_back(curr_time);

			if (((*part_iter)->_dependency) && (!(*part_iter)->_pre_part_is_done)) {
				(*part_iter)->SwitchToPostPart();
			}
			(*part_iter) = NULL;
		}
		++part_iter;
	}
}


bool Pallet::Loaded_Part_HalfDone() {
	int num_of_done(0);
	int num_loaded_part(0);

	std::vector<Part*>::iterator part_iter = _loaded_part.begin();
	while (part_iter != _loaded_part.end()) {
		if ((*part_iter)) { // there is loaded part
			if ((*part_iter)->IsDone(true)) ++num_of_done;

			++num_loaded_part;
		}

		++part_iter;
	}

	if (num_of_done == num_loaded_part) return true;
	else return false;
}

bool Pallet::IsThereLoadedPart() {
	std::vector<Part*>::iterator part_iter = _loaded_part.begin();

	while (part_iter != _loaded_part.end()) {
		if ((*part_iter)) return true;
		++part_iter;
	}
	return false;
}

void Pallet::OneStepForward() {
	if (_current_processing_time == _process_duration) {
		_in_process = false;
		_process_name = process::Nothing;
	}
	else {
		++_current_processing_time;
	}
}

//--------------------
void Pallet::SaveCallingTime(int curr_time) {
	std::vector<Part*>::iterator part_iter = _loaded_part.begin();
	while (part_iter != _loaded_part.end()) {
		if ((*part_iter)) { //there is loaded part
			(*part_iter)->_history._loading_time.push_back(curr_time);
		}
		++part_iter;
	}
}
//---------------------

void Pallet::print_PalletInfo(int idx) {
	printf("%dth call) Pallet IDX %d\n", idx, _pallet_idx);
	std::string process_name;
	if (_process_name == process::Nothing) { process_name = "Nothing"; }
	if (_process_name == process::Loading) { process_name = "Loading"; }
	if (_process_name == process::Unloading) { process_name = "Unloading"; }
	if (_process_name == process::Machining) { process_name = "Machining"; }


	printf("Processing, Name: %d, %s \n", _in_process, process_name.c_str());
	printf("Time current/end: (%d/%d) \n", _current_processing_time, _process_duration);

	printf("Location: %s, (fixture type, laoded part idx): ", loc::printLocation(_pallet_loc).c_str());
	for (int i(0); i < NUM_FIXTURE_TYPE; ++i) {
		int part_idx(-1);
		if (_loaded_part[i]) { part_idx = _loaded_part[i]->_part_idx; }
		printf("(%d, %d), ", _fixture_type[i], part_idx);
	}
	printf("\n");//pallets in a vector ? 
}

void Pallet::print_PalletMac(int idx) {
	printf("Pallet idx%d\n", _pallet_idx);
}