#include "Part.hpp"
#include <iostream>

Part::Part(int idx, int type, int num_operation, int due_time, bool dependency) :
	_part_idx(idx), _part_type(type), _num_operation(num_operation), _due_time(due_time),
	_dependency(dependency),
	_current_operation(0), _post_current_operation(0) {

	_pallet_idx = -1;
	_pre_part_is_done = false;
	_part_loc = loc::Outside;

	//printf("Part generated. type: %d, num operation: %d, due_time: %d \n", 
			//_part_type, _num_operation, _due_time);
}


bool Part::IsDone(bool half_done_is_done) {
	if (_current_operation == _num_operation) {
		if (half_done_is_done) {
			return true;
		}
		else {
			if (_dependency) { if (_pre_part_is_done) return true; }
			else return true;
		}
	}
	return false;
}

void Part::SwitchToPostPart() {
	if (!_dependency) printf("[Error] This is not dependent part\n");

	_part_type = _dependent_part_type;
	_current_operation = 0;
	_num_operation = _post_num_operation;
	_pre_part_is_done = true;
	//--------------------
	printf("@@@@ switching part\n");
	_sum_pt = _post_sum_pt; // sum of all processing time of a part
	_machining_info_list = _post_machining_info_list;
	//-------------------
}

int Part::getProcessingTime(int operation_idx, int machine) {
	MachiningInfo m_info = _machining_info_list[operation_idx];
	for (int m_idx(0); m_idx < m_info.machine_idx.size(); ++m_idx) {
		if (m_info.machine_idx[m_idx] == machine) {
			return m_info.processing_time[m_idx];
		}
	}
	printf("[Error] No Matched operation idx and machine: %d, %d\n", operation_idx, machine);
	return -1;
}


void Part::print_PartInfo(int idx) {
	printf("%dth calling) part idx: %d, type: %d, current/num operation: %d/%d, due_time: %d , part_loc: %d\n",
		idx, _part_idx, _part_type, _current_operation, _num_operation, _due_time, _part_loc);

	for (int i(0); i < _machining_info_list.size(); ++i) {
		printf("%d th operation info: \n", i);
		for (int j(0); j < _machining_info_list[i].machine_name.size(); ++j) {
			printf("name: %s, idx: %d, processing_time: %d\n",
				_machining_info_list[i].machine_name[j].c_str(),
				_machining_info_list[i].machine_idx[j],
				_machining_info_list[i].processing_time[j]);
		}
	}

	if (_dependency) {
		printf("*** Dependent part) num operation: %d\n", _post_num_operation);

		for (int i(0); i < _post_machining_info_list.size(); ++i) {
			printf("%d th operation info: \n", i);
			for (int j(0); j < _post_machining_info_list[i].machine_name.size(); ++j) {
				printf("name: %s, idx: %d, processing_time: %d\n",
					_post_machining_info_list[i].machine_name[j].c_str(),
					_post_machining_info_list[i].machine_idx[j],
					_post_machining_info_list[i].processing_time[j]);
			}
		}

	}
	printf("\n");
}

void Part::print_PartStatus(int idx) {
	printf("(%dth calling) part idx: %d, type: %d, ", idx, _part_idx, _part_type);
	printf("current/num operation: %d/%d, due_time: %d , part_loc: %s, ",
		_current_operation, _num_operation, _due_time,
		loc::printLocation(_part_loc).c_str());
	printf("Dependency (completed): (%d, %d),  ", _dependency, _pre_part_is_done);

	printf("All Done: %d", IsDone(false));
	printf("\n");
}
