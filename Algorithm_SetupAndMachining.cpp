#include "Algorithm_SetupAndMachining.hpp"
#include "Pallet.hpp"
#include "Part.hpp"
#include "Factory.hpp"
#include "Definition.h"

#define SHOW_DEBUG_MESSAGE 0;

Algorithm_SetupAndMachining::Algorithm_SetupAndMachining(const Factory & factory) {
	_num_Machine = factory.Num_Machine;
	_MovingTime = factory.MovingTime;

	machine_usage.clear();
	machine_processing_time.clear();
	machine_current_time.clear();
	machine_engaged_pallet_idx.clear();
	machine_processing_part.clear();

	machine_usage.resize(_num_Machine, false);
	machine_processing_time.resize(_num_Machine, 0);
	machine_current_time.resize(_num_Machine, 0);
	machine_engaged_pallet_idx.resize(_num_Machine, -1);
	machine_processing_part.resize(_num_Machine, NULL);
}


void Algorithm_SetupAndMachining::run(int curr_time, const std::vector<Pallet*> & pallet_list) {

	_Update(pallet_list);
	// _OperationTime1(pallet_list); //SOPT (original)
	// _OperationTime2(pallet_list); //EDD
	// _OperationTime3(pallet_list); //STPT
	// _OperationTime4(pallet_list); //MWKR
	_OperationTime5(curr_time, pallet_list); //MST
 // _OperationTime6(curr_time, pallet_list); //MDD    

}


void Algorithm_SetupAndMachining::_OperationTime1(const std::vector<Pallet*> & pallet_list) {

	std::vector<Part*> candidate_part_list;

	for (int i(0); i < _num_Machine; ++i) {
		if (!machine_usage[i]) { // Machine is available

			candidate_part_list.clear();

			// gather all pallets (part) that can be operated in this machine
			std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
			while (pl_iter != pallet_list.end()) {
				if ((!(*pl_iter)->_in_process) &&
					((*pl_iter)->_pallet_loc == loc::Buffer)) {

					for (int pt_idx(0); pt_idx < (*pl_iter)->_loaded_part.size(); ++pt_idx) {
						if ((*pl_iter)->_loaded_part[pt_idx]) { // There is loaded part
							Part* check_part = (*pl_iter)->_loaded_part[pt_idx];
							printf("1\n");
							if (check_part->IsDone(true)) {
								printf("2\n");
								check_part->print_PartInfo(pt_idx);
								continue;
							}
							else {
								// check machine info
								MachiningInfo m_info =
									check_part->_machining_info_list[check_part->_current_operation];

								for (int m_idx(0); m_idx < m_info.machine_idx.size(); ++m_idx) {
									if (m_info.machine_idx[m_idx] == i) {
										candidate_part_list.push_back(check_part);
									}
								}
							}
						}
					} // End of loaded part loop
				}
				++pl_iter;
			} // End of Pallet loop
/*#if (SHOW_DEBUG_MESSAGE)
			printf("*** [Machine %d] Selected Candidate *** \n", i);
			for (int i(0); i < candidate_part_list.size(); ++i)
				candidate_part_list[i]->printInfo(i);
#endif */
			if (candidate_part_list.size() > 0) {
				// Pick a machine with shortest operation time
				int selected_pt_idx(0); // Find this
				Part* ch_pt = candidate_part_list[0];

				int shortest_processing_time =
					ch_pt->getProcessingTime(ch_pt->_current_operation, i);

				printf("--->shortest_processing_time %d\n", shortest_processing_time);

				int processing_time(0);
				for (int pt_idx(1); pt_idx < candidate_part_list.size(); ++pt_idx) {
					ch_pt = candidate_part_list[pt_idx];
					processing_time = ch_pt->getProcessingTime(ch_pt->_current_operation, i);
					printf("--->processing_time %d\n", processing_time);
					if (shortest_processing_time > processing_time) {
						shortest_processing_time = processing_time;
						selected_pt_idx = pt_idx;
					}
				}
				// Operation (machining) starts
				Part* selected_part = candidate_part_list[selected_pt_idx];
				Pallet* selected_pallet = pallet_list[selected_part->_pallet_idx];

				// Pallet process starts
				selected_pallet->_in_process = true;
				selected_pallet->_process_name = process::Machining;

				selected_pallet->_process_duration = shortest_processing_time;
				selected_pallet->_current_processing_time = 0;
				//Mac�� LocationUpdate, ���⼭ �̵��ð� �߰��������� ����-------------------------------------------
				if (i == 0) {
					printf("shortest_processing_time (before LocationUpate_mac1) %d\n", shortest_processing_time);
					selected_pallet->LocationUpdate_Mac1(loc::Machine0, selected_pallet,
						machine_pre_pallet[i], shortest_processing_time);

					/* Frame of transporation time
					   1. Pallet::LocationUpdate_Mac1 ���� �̵��ð��� �߰����� �Ǵ� ��, �ʿ�� �߰��Ͽ�
					   �ٽ� Algorithm_SetupAndMachining::_OperationTime���� ����
					   2. ����shortest_processing_time ����Ǹ� Machine Starts �κп� shortest_processing_time */

					   /* Code Logic
						  1. LocationUpdate_Mac1�Լ����� selected_pallet(���� ���õ��ȷ�)�� ������ �����ߴ� �ӽ�(_pre_mac)�� ������ġ(loc::Machine0)�� ��
						  2. machine i���� selected_pallet�� �ش� �ӽ�(i)���� ������ ������ �ȷ�(machine_pre_pallet[i])�� ��
						  3. �̵��ð� �߻� ��, selected_plt->spt_temp  =  shortest_processing_time + movingtime
						  4. �̵��ð��� �߰��� _spt_temp�� _OperationTime�Լ��� ����
						  (shortest_processing_time�� �޴� ����Ʈ�� _processing_duration,  machine_processing_time[i]) */
				}
				else  if (i == 1) {
					printf("shortest_processing_time (before LocationUpate_mac2) %d\n", shortest_processing_time);
					selected_pallet->LocationUpdate_Mac2(loc::Machine1, selected_pallet,
						machine_pre_pallet[i], shortest_processing_time);
				}
				else if (i == 2) {
					printf("shortest_processing_time (before LocationUpate_mac3) %d\n", shortest_processing_time);
					selected_pallet->LocationUpdate_Mac3(loc::Machine2, selected_pallet,
						machine_pre_pallet[i], shortest_processing_time);
				}

				shortest_processing_time = selected_pallet->_spt_temp;
				printf("shortest_processing_time (after LcationUpdate_macs) %d\n", shortest_processing_time);

				//----------------------------------------------------------------------------------------------------


				// Machine Starts
				machine_usage[i] = true;
				machine_processing_time[i] = shortest_processing_time;
				printf("short process time: %d\n", shortest_processing_time);

				machine_current_time[i] = 0;
				machine_engaged_pallet_idx[i] = selected_pallet->_pallet_idx;
				machine_processing_part[i] = selected_part;


/*#if (SHOW_DEBUG_MESSAGE)
				printf("\n*** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->printInfo(0);
				selected_pallet->printInfo(0);
#endif */

				// pre mac �����ϴ� �κ�(_pre_mac�� loc�� ��)------------------
				printf("\n*** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->print_PartInfo(0);
				selected_pallet->print_PalletMac(0);



				std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
				while (pl_iter != pallet_list.end()) {
					if ((*pl_iter)->_pallet_idx == selected_pallet[0]._pallet_idx) {
						if (i == 0) {
							(*pl_iter)->_pre_mac = 4; // machine0=4, machine1=5, machine2=6 (Definition.hpp)
							//machine_pre_pallet[0] = (*pl_iter)->_pallet_idx;
						}
						else if (i == 1) {
							(*pl_iter)->_pre_mac = 5;
							//machine_pre_pallet[1] = (*pl_iter)->_pallet_idx;
						}
						else if (i == 2) {
							(*pl_iter)->_pre_mac = 6;
							// machine_pre_pallet[2] = (*pl_iter)->_pallet_idx;
						}
					}
					++pl_iter;
				}

				//--------------------------------------------------------------

			}
			else {
/*#if (SHOW_DEBUG_MESSAGE)
				printf("No candidate part\n");
#endif */
			}// End of if(candidate_part_list.size() > 0)

		} // End of if(!machine_usage[i])

	} //End of Machine Loop

}


//-----------------------------
void Algorithm_SetupAndMachining::_OperationTime2(const std::vector<Pallet*> & pallet_list) {

	std::vector<Part*>candidate_part_list;

	for (int i(0); i < _num_Machine; ++i) {
		if (!machine_usage[i]) { //Machine is available

			candidate_part_list.clear();

			//gather all pallets (part) that can be oprated in this machine
			std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
			while (pl_iter != pallet_list.end()) {
				if ((!(*pl_iter)->_in_process) &&
					((*pl_iter)->_pallet_loc == loc::Buffer)) {

					for (int pt_idx(0); pt_idx < (*pl_iter)->_loaded_part.size(); ++pt_idx) {
						if ((*pl_iter)->_loaded_part[pt_idx]) { // There is loaded part
							Part* check_part = (*pl_iter)->_loaded_part[pt_idx];
							printf("1\n");
							if (check_part->IsDone(true)) {
								printf("2\n");
								check_part->print_PartInfo(pt_idx);
								continue;
							}
							else {
								//check machine info
								MachiningInfo m_info =
									check_part->_machining_info_list[check_part->_current_operation];

								for (int m_idx(0); m_idx < m_info.machine_idx.size(); ++m_idx) {
									if (m_info.machine_idx[m_idx] == i) {
										candidate_part_list.push_back(check_part);
									}
								}
							}
						}
					} //End of loaded part loop
				}
				++pl_iter;
			} //End of Pallet loop
/*#if (SHOW_DEBUG_MESSAGE)
			printf("*** [Machine %d] Selected Candidate *** \n", i);
			for (int i(0); i < candidate_part_list.size(); ++i)
				candidate_part_list[i]->printInfo(i);
#endif*/
			if (candidate_part_list.size() > 0) {
				//Pick a machine with shortest operation time
				int selected_pt_idx(0);  //Find this
				Part* ch_pt = candidate_part_list[0];

				int earliest_due_date = ch_pt->_due_time;

				printf("!!!!!!!!!!!!!!!!!!!!!duedate %d\n", ch_pt->_due_time);
				printf("!!!!!!!!!!earliest due date %d\n", earliest_due_date);

				int due_date(0);
				for (int pt_idx(1); pt_idx < candidate_part_list.size(); ++pt_idx) {
					ch_pt = candidate_part_list[pt_idx];
					due_date = ch_pt->_due_time;
					printf("!!!!!!!!!!!due_date %d\n", due_date);
					if (earliest_due_date > due_date) {
						earliest_due_date = due_date;
						selected_pt_idx = pt_idx;
					}
				}
				// Operation (machining) starts
				Part* selected_part = candidate_part_list[selected_pt_idx];
				Pallet* selected_pallet = pallet_list[selected_part->_pallet_idx];

				//Pallet process starts
				selected_pallet->_in_process = true;
				selected_pallet->_process_name = process::Machining;

				// �����ð� ��ǲ
				int process_time(0);
				process_time = selected_part->getProcessingTime(selected_part->_current_operation, i);

				selected_pallet->_process_duration = process_time;
				selected_pallet->_current_processing_time = 0;

				//Mac�� LocationUpdate, ���⼭ �̵��ð� �߰��������� ����-------------------------------
				if (i == 0) {
					printf("process_time (before LocationUpate_mac1) %d\n", process_time);
					selected_pallet->LocationUpdate_Mac1(loc::Machine0, selected_pallet,
						machine_pre_pallet[i], process_time);

					/* Frame of transporation time
					   1. Pallet::LocationUpdate_Mac1 ���� �̵��ð��� �߰����� �Ǵ� ��, �ʿ�� �߰��Ͽ�
					   �ٽ� Algorithm_SetupAndMachining::_OperationTime���� ����
					   2. ����shortest_processing_time ����Ǹ� Machine Starts �κп� shortest_processing_time */

					   /* Code Logic
						  1. LocationUpdate_Mac1�Լ����� selected_pallet(���� ���õ��ȷ�)�� ������ �����ߴ� �ӽ�(_pre_mac)�� ������ġ(loc::Machine0)�� ��
						  2. machine i���� selected_pallet�� �ش� �ӽ�(i)���� ������ ������ �ȷ�(machine_pre_pallet[i])�� ��
						  3. �̵��ð� �߻� ��, selected_plt->spt_temp  =  shortest_processing_time + movingtime
						  4. �̵��ð��� �߰��� spt_temp�� _OperationTime�� ����
						  (shortest_processing_time�� �޴� ����Ʈ�� _processing_duration,  machine_processing_time[i]) */

				}
				else  if (i == 1) {
					printf("process_time (before LocationUpate_mac2) %d\n", process_time);
					selected_pallet->LocationUpdate_Mac2(loc::Machine1, selected_pallet,
						machine_pre_pallet[i], process_time);
				}
				else if (i == 2) {
					printf("process_time (before LocationUpate_mac3) %d\n", process_time);
					selected_pallet->LocationUpdate_Mac3(loc::Machine2, selected_pallet,
						machine_pre_pallet[i], process_time);
				}

				process_time = selected_pallet->_spt_temp;
				printf("process_time (after LcationUpdate_macs) %d\n", process_time);
				//-------------------------------------------------------------------------------------

				// Machine Stats
				machine_usage[i] = true;
				machine_processing_time[i] = process_time;
				printf("process time : %d\n", process_time);

				machine_current_time[i] = 0;
				machine_engaged_pallet_idx[i] = selected_pallet->_pallet_idx;
				machine_processing_part[i] = selected_part;

/*#if (SHOW_DEBUG_MESSAGE)
				printf("\n***[Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->printInfo(0);
				selected_pallet->printInfo(0);
#endif*/
				//pre mac �����ϴ� �κ� (_pre_mac�� loc�� ��) ------------------
				printf("\n *** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->print_PartInfo(0);
				selected_pallet->print_PalletMac(0);


				std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
				while (pl_iter != pallet_list.end()) {
					if ((*pl_iter)->_pallet_idx == selected_pallet[0]._pallet_idx) {
						if (i == 0) {
							(*pl_iter)->_pre_mac = 4; //machine0 = 4
						}
						else if (i == 1) {
							(*pl_iter)->_pre_mac = 5;
						}
						else if (i == 2) {
							(*pl_iter)->_pre_mac = 6;
						}
					}
					++pl_iter;
				}
				//---------------------------------------
			}
			else {
/*#if (SHOW_DEBUG_MESSAGE)
				printf("No candidate part\n");
#endif */
			} //End of if(candidate_part_list.size() > 0 
		} // End of if(!machine_usage[i])
	} //End of Machine Loop
}

//-----------------------------


//------------------------------
void Algorithm_SetupAndMachining::_OperationTime3(const std::vector<Pallet*> & pallet_list) {

	std::vector<Part*> candidate_part_list;

	for (int i(0); i < _num_Machine; ++i) {
		if (!machine_usage[i]) { // Machine is available

			candidate_part_list.clear();

			//gather all pallets (part) that can be operated in this machine
			std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
			while (pl_iter != pallet_list.end()) {
				if ((!(*pl_iter)->_in_process) &&
					((*pl_iter)->_pallet_loc == loc::Buffer)) {
					for (int pt_idx(0); pt_idx < (*pl_iter)->_loaded_part.size(); ++pt_idx) {
						if ((*pl_iter)->_loaded_part[pt_idx]) {  //There is loaded part 
							Part* check_part = (*pl_iter)->_loaded_part[pt_idx];
							printf("1\n");
							if (check_part->IsDone(true)) {
								printf("2\n");
								check_part->print_PartInfo(pt_idx);
								continue;
							}
							else {
								//check machine info 
								MachiningInfo m_info =
									check_part->_machining_info_list[check_part->_current_operation];

								for (int m_idx(0); m_idx < m_info.machine_idx.size(); ++m_idx) {
									if (m_info.machine_idx[m_idx] == i) {
										candidate_part_list.push_back(check_part);
									}
								}
							}
						}
					} //End of part loop  
				}
				++pl_iter;
			} //End of pallet loop
#if (SHOW_DEBUG_MESAGE)
			printf("*** [Machine %d] Selected Candidate *** \n", i);
			for (int i(0); i < candidaet_part_list.size(); ++i)
				candidate_part_list[i]->printInfo(i);
#endif

			if (candidate_part_list.size() > 0) {
				//Pirck a macine with operation time 
				int selected_pt_idx(0);
				Part* ch_pt = candidate_part_list[0];

				int shortest_total_processing_time = ch_pt->_sum_pt;
				int total_processing_time(0);
				for (int pt_idx(1); pt_idx < candidate_part_list.size(); ++pt_idx) {
					ch_pt = candidate_part_list[pt_idx];
					total_processing_time = ch_pt->_sum_pt;
					printf("--->total processing time %d\n", total_processing_time);
					if (shortest_total_processing_time > total_processing_time) {
						shortest_total_processing_time = total_processing_time;
						selected_pt_idx = pt_idx;
					}
				}
				printf(" --->shortest total processing time %d\n", shortest_total_processing_time);


				//Operation (machining) starts
				Part* selected_part = candidate_part_list[selected_pt_idx];
				Pallet* selected_pallet = pallet_list[selected_part->_pallet_idx];

				//Pallet process starts
				selected_pallet->_in_process = true;
				selected_pallet->_process_name = process::Machining;

				int processing_time(0);  //STPT�� ��Ʈ�� �����ð� �´���üũ�ϱ� 
				processing_time = selected_part->getProcessingTime(selected_part->_current_operation, i);

				selected_pallet->_process_duration = processing_time;
				selected_pallet->_current_processing_time = 0;

				//Transportation time
				if (i == 0) {
					selected_pallet->LocationUpdate_Mac1(loc::Machine0, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				else if (i == 1) {
					selected_pallet->LocationUpdate_Mac2(loc::Machine1, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				else if (i == 2) {
					selected_pallet->LocationUpdate_Mac3(loc::Machine2, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				processing_time = selected_pallet->_spt_temp; //spt_temp:processing time with trans time
				printf("processing_time(after LocationUpdate) : %d\n", processing_time);

				//Machine Starts
				machine_usage[i] = true;
				machine_processing_time[i] = processing_time;
				printf("selected processing time %d\n", machine_processing_time[i]);

				machine_current_time[i] = 0;
				machine_engaged_pallet_idx[i] = selected_pallet->_pallet_idx;
				machine_processing_part[i] = selected_part;

/*#if (SHOW_DEBUG_MESSAGE)
				printf("\n*** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->printInfo(0);
				selected_pallet->printInfo(0);
#endif */ 
				//record _pre_mac for adding transportation time
				printf("\n *** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->print_PartInfo(0);
				selected_pallet->print_PalletMac(0);

				std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
				while (pl_iter != pallet_list.end()) {
					if ((*pl_iter)->_pallet_idx == selected_pallet[0]._pallet_idx) {
						if (i == 0) {
							(*pl_iter)->_pre_mac = 4;
						}
						else if (i == 1) {
							(*pl_iter)->_pre_mac = 5;
						}
						else if (i == 2) {
							(*pl_iter)->_pre_mac = 6;
						}
					}
					++pl_iter;
				}
			}
			else {
 /*#if (SHOW_DEBUG_MESSAGE)
				printf("No candidate part\n");
#endif */
			} //End of if(candidated_part_list.size() > 0)
		} //End of Machine usage
	} //End of Machine loop
}
//------------------------------



//-----------------------------
void Algorithm_SetupAndMachining::_OperationTime4(const std::vector<Pallet*> & pallet_list) {

	std::vector<Part*> candidate_part_list;

	for (int i(0); i < _num_Machine; ++i) {
		if (!machine_usage[i]) { //machine is available

			candidate_part_list.clear();

			//gather all pallets 
			std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
			while (pl_iter != pallet_list.end()) {
				if ((!(*pl_iter)->_in_process) &&
					((*pl_iter)->_pallet_loc == loc::Buffer)) {

					for (int pt_idx(0); pt_idx < (*pl_iter)->_loaded_part.size(); ++pt_idx) {
						if ((*pl_iter)->_loaded_part[pt_idx]) {  //There is loaded part
							Part* check_part = (*pl_iter)->_loaded_part[pt_idx];
							printf("1\n");
							if (check_part->IsDone(true)) {
								printf("2\n");
								check_part->print_PartInfo(pt_idx);
								continue;
							}
							else {
								//check machine info
								MachiningInfo m_info =
									check_part->_machining_info_list[check_part->_current_operation];

								for (int m_idx(0); m_idx < m_info.machine_idx.size(); ++m_idx) {
									if (m_info.machine_idx[m_idx] == i) {
										candidate_part_list.push_back(check_part);
									}
								}
							}
						}
					} //End of part loop
				}
				++pl_iter;
			} //End of pallet_list loop
/* #if (SHOW_DEBUG_MESSAGE)
			printf(" *** [MAchine %d] Selected Candidate *** \n", i);
			for (int i(0); i < candidate_part_list.size(); ++i)
				candidate_part_list[i]->printInfo(i);
#endif */

			//----------------------------------------------------------
			//record the average of remaining operation processing time for a part
			printf("Isthere candidate part??????????????????????\n");
			if (candidate_part_list.size() > 0) { //is there candidate part?
				printf("Yes!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				for (int i(0); i < candidate_part_list.size(); ++i) {
					Part* ch_pt_tmp = candidate_part_list[i];
					printf("part%d, sum_pt%d ", ch_pt_tmp->_part_idx, ch_pt_tmp->_sum_pt);
					//machine info for the part

					int avg_remaining_pt(0);
					int remaining_op(0);
					std::vector<MachiningInfo> m_info2;
					m_info2 = ch_pt_tmp->_machining_info_list;


					int curr_op = ch_pt_tmp->_current_operation;
					for (int j(curr_op); j < m_info2.size(); ++j) {
						int pt_temp(0);
						int num_mac = m_info2[j].machine_name.size(); //the number of alter mac for a next(remaining) operation
						printf("operation%d ", j);
						for (int k(0); k < m_info2[j].machine_name.size(); ++k) { //machine_idx
							pt_temp += m_info2[j].processing_time[k]; //the sum of remaining operation  processing time for a part
							printf("%d ", m_info2[j].processing_time[k]);
						}
						avg_remaining_pt += pt_temp / num_mac;
					}

					remaining_op = ch_pt_tmp->_num_operation - ch_pt_tmp->_current_operation; //the number of remaining op
					ch_pt_tmp->_remaining_pt_avg = avg_remaining_pt;

					printf("--->part%d, num operation%d(%d), current operation%d, remaining operation%d(%d)\n",
						candidate_part_list[i]->_part_idx,
						candidate_part_list[i]->_num_operation,
						candidate_part_list[i]->_sum_pt,
						candidate_part_list[i]->_current_operation,
						remaining_op,
						candidate_part_list[i]->_remaining_pt_avg);
				}
			}

			if (candidate_part_list.size() > 0) {
				//Pictk a machine with operation time
				int selected_pt_idx(0);
				Part* ch_pt = candidate_part_list[0];

				int most_work_remaining_time = ch_pt->_remaining_pt_avg;
				int work_remaining_time(0);
				for (int pt_idx(1); pt_idx < candidate_part_list.size(); ++pt_idx) {
					ch_pt = candidate_part_list[pt_idx];
					work_remaining_time = ch_pt->_remaining_pt_avg;
					printf("--->work remaining time %d\n", work_remaining_time);
					if (most_work_remaining_time < work_remaining_time) {
						most_work_remaining_time = work_remaining_time;
						selected_pt_idx = pt_idx;

					}
				}
				printf("--->most work remaining time %d\n", most_work_remaining_time);
				//-------------------------------------------------------------


				// Operation(machinning) starts
				Part* selected_part = candidate_part_list[selected_pt_idx];
				Pallet* selected_pallet = pallet_list[selected_part->_pallet_idx];

				//Pallet process starts
				selected_pallet->_in_process = true;
				selected_pallet->_process_name = process::Machining;

				int processing_time(0);  //���� �����ð��� ����ū ��Ʈ�� �´��� üũ�ϱ�
				processing_time = selected_part->getProcessingTime(selected_part->_current_operation, i);

				selected_pallet->_process_duration = processing_time;
				selected_pallet->_current_processing_time = 0;

				//Transportation time
				if (i == 0) {
					selected_pallet->LocationUpdate_Mac1(loc::Machine0, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				if (i == 1) {
					selected_pallet->LocationUpdate_Mac2(loc::Machine1, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				if (i == 2) {
					selected_pallet->LocationUpdate_Mac3(loc::Machine2, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				processing_time = selected_pallet->_spt_temp; //_spt_temp:processing time wiht trans time
				printf("processing_time(after LocationUpdate) %d\n", processing_time);

				//Machine starts
				machine_usage[i] = true;
				machine_processing_time[i] = processing_time;
				printf("selected processing time %d\n", machine_processing_time[i]);

				machine_current_time[i] = 0;
				machine_engaged_pallet_idx[i] = selected_pallet->_pallet_idx;
				machine_processing_part[i] = selected_part;

/* #if (SHOW_DEBUG_MESSAGE)
				printf("\n*** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->printfInfo(0);
				selected_pallet->printfInfo(0);
#endif */
				//record _pre_mac for adding transportation time
				printf("\n*** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->print_PartInfo(0);
				selected_pallet->print_PalletMac(0);

				std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
				while (pl_iter != pallet_list.end()) {
					if ((*pl_iter)->_pallet_idx == selected_pallet[0]._pallet_idx) {
						if (i == 0) {
							(*pl_iter)->_pre_mac = 4;
						}
						else if (i == 1) {
							(*pl_iter)->_pre_mac = 5;
						}
						else if (i == 2) {
							(*pl_iter)->_pre_mac = 6;
						}
					}
					++pl_iter;
				}
			}
			else {
/* #if (SHOW_DEBUG_MESSAGE)
				printf("No candidate part\n");
#endif */
			} //End of if(candidate_part_list.size() > 0)
		} // The end of machine usage
	} // Machine loop
}
//----------------------------


//-----------------------------
void Algorithm_SetupAndMachining::_OperationTime5(int curr_time, const std::vector<Pallet*> & pallet_list) {

	std::vector<Part*> candidate_part_list;

	for (int i(0); i < _num_Machine; ++i) {
		if (!machine_usage[i]) { //machine is available

			candidate_part_list.clear();

			//gather all pallets 
			std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
			while (pl_iter != pallet_list.end()) {
				if ((!(*pl_iter)->_in_process) &&
					((*pl_iter)->_pallet_loc == loc::Buffer)) {

					for (int pt_idx(0); pt_idx < (*pl_iter)->_loaded_part.size(); ++pt_idx) {
						if ((*pl_iter)->_loaded_part[pt_idx]) {  //There is loaded part
							Part* check_part = (*pl_iter)->_loaded_part[pt_idx];
							printf("1\n");
							if (check_part->IsDone(true)) {
								printf("2\n");
								check_part->print_PartInfo(pt_idx);
								continue;
							}
							else {
								//check machine info
								MachiningInfo m_info =
									check_part->_machining_info_list[check_part->_current_operation];

								for (int m_idx(0); m_idx < m_info.machine_idx.size(); ++m_idx) {
									if (m_info.machine_idx[m_idx] == i) {
										candidate_part_list.push_back(check_part);
									}
								}
							}
						}
					} //End of part loop
				}
				++pl_iter;
			} //End of pallet_list loop
/* #if (SHOW_DEBUG_MESSAGE)
			printf(" *** [MAchine %d] Selected Candidate *** \n", i);
			for (int i(0); i < candidate_part_list.size(); ++i)
				candidate_part_list[i]->printInfo(i);
#endif */

			//----------------------------------------------------------
			//record the average of remaining operation processing time for a part
			printf("Isthere candidate part??????????????????????\n");
			if (candidate_part_list.size() > 0) { //is there candidate part?
				printf("Yes!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				for (int i(0); i < candidate_part_list.size(); ++i) {
					Part* ch_pt_tmp = candidate_part_list[i];
					printf("part%d, sum_pt%d ", ch_pt_tmp->_part_idx, ch_pt_tmp->_sum_pt);
					//machine info for the part

					int avg_remaining_pt(0);
					int remaining_op(0);
					std::vector<MachiningInfo> m_info2;
					m_info2 = ch_pt_tmp->_machining_info_list;


					int curr_op = ch_pt_tmp->_current_operation;
					for (int j(curr_op); j < m_info2.size(); ++j) {
						int pt_temp(0);
						int num_mac = m_info2[j].machine_name.size(); //op�� ��ȸӽ� ����
						printf("opeartion%d ", j);
						for (int k(0); k < m_info2[j].machine_name.size(); ++k) { //machine_idx
							pt_temp += m_info2[j].processing_time[k]; //the sum of remaining operation  processing time for a part
							printf("%d ", m_info2[j].processing_time[k]);
						}
						avg_remaining_pt += pt_temp / num_mac;
					}

					remaining_op = ch_pt_tmp->_num_operation - ch_pt_tmp->_current_operation; //the number of remaining op
					ch_pt_tmp->_remaining_pt_avg = avg_remaining_pt;
					ch_pt_tmp->_slack_time = ch_pt_tmp->_due_time - curr_time - ch_pt_tmp->_remaining_pt_avg;

					printf("--->part%d(%d/%d sim/due), num operation%d(%d), current operation%d, remaining operation%d(%d), slack time%d\n",
						candidate_part_list[i]->_part_idx,
						curr_time,
						candidate_part_list[i]->_due_time,
						candidate_part_list[i]->_num_operation,
						candidate_part_list[i]->_sum_pt,
						candidate_part_list[i]->_current_operation,
						remaining_op,
						candidate_part_list[i]->_remaining_pt_avg,
						candidate_part_list[i]->_slack_time);
				}
			}

			if (candidate_part_list.size() > 0) {
				//Pictk a machine with operation time
				int selected_pt_idx(0);
				Part* ch_pt = candidate_part_list[0];

				int minimum_slack_time = ch_pt->_slack_time;
				int slack_time(0);
				for (int pt_idx(1); pt_idx < candidate_part_list.size(); ++pt_idx) {
					ch_pt = candidate_part_list[pt_idx];
					slack_time = ch_pt->_slack_time;
					printf("---> slack time %d\n", slack_time);
					if (minimum_slack_time > slack_time) {
						minimum_slack_time = slack_time;
						selected_pt_idx = pt_idx;
					}
				}
				printf("--->minimum slack time %d\n", minimum_slack_time);
				//-------------------------------------------------------------


				// Operation(machinning) starts
				Part* selected_part = candidate_part_list[selected_pt_idx];
				Pallet* selected_pallet = pallet_list[selected_part->_pallet_idx];

				//Pallet process starts
				selected_pallet->_in_process = true;
				selected_pallet->_process_name = process::Machining;

				int processing_time(0);  //���� �����ð��� ����ū ��Ʈ�� �´��� üũ�ϱ�
				processing_time = selected_part->getProcessingTime(selected_part->_current_operation, i);

				selected_pallet->_process_duration = processing_time;
				selected_pallet->_current_processing_time = 0;

				//Transportation time
				if (i == 0) {
					selected_pallet->LocationUpdate_Mac1(loc::Machine0, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				if (i == 1) {
					selected_pallet->LocationUpdate_Mac2(loc::Machine1, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				if (i == 2) {
					selected_pallet->LocationUpdate_Mac3(loc::Machine2, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				processing_time = selected_pallet->_spt_temp; //_spt_temp:processing time wiht trans time
				printf("processing_time(after LocationUpdate) %d\n", processing_time);

				//Machine starts
				machine_usage[i] = true;
				machine_processing_time[i] = processing_time;
				printf("selected processing time %d\n", machine_processing_time[i]);

				machine_current_time[i] = 0;
				machine_engaged_pallet_idx[i] = selected_pallet->_pallet_idx;
				machine_processing_part[i] = selected_part;

/* #if (SHOW_DEBUG_MESSAGE)
				printf("\n*** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->printfInfo(0);
				selected_pallet->printfInfo(0);
#endif */ 
				//record _pre_mac for adding transportation time
				printf("\n*** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->print_PartInfo(0);
				selected_pallet->print_PalletMac(0);

				std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
				while (pl_iter != pallet_list.end()) {
					if ((*pl_iter)->_pallet_idx == selected_pallet[0]._pallet_idx) {
						if (i == 0) {
							(*pl_iter)->_pre_mac = 4;
						}
						else if (i == 1) {
							(*pl_iter)->_pre_mac = 5;
						}
						else if (i == 2) {
							(*pl_iter)->_pre_mac = 6;
						}
					}
					++pl_iter;
				}
			}
			else {
/* #if (SHOW_DEBUG_MESSAGE)
				printf("No candidate part\n");
#endif */
			} //End of if(candidate_part_list.size() > 0)
		} // The end of machine usage
	} // Machine loop
}
//----------------------------


//-----------------------------
void Algorithm_SetupAndMachining::_OperationTime6(int curr_time, const std::vector<Pallet*> & pallet_list) {

	std::vector<Part*> candidate_part_list;

	for (int i(0); i < _num_Machine; ++i) {
		if (!machine_usage[i]) { //machine is available

			candidate_part_list.clear();

			//gather all pallets 
			std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
			while (pl_iter != pallet_list.end()) {
				if ((!(*pl_iter)->_in_process) &&
					((*pl_iter)->_pallet_loc == loc::Buffer)) {

					for (int pt_idx(0); pt_idx < (*pl_iter)->_loaded_part.size(); ++pt_idx) {
						if ((*pl_iter)->_loaded_part[pt_idx]) {  //There is loaded part
							Part* check_part = (*pl_iter)->_loaded_part[pt_idx];
							printf("1\n");
							if (check_part->IsDone(true)) {
								printf("2\n");
								check_part->print_PartInfo(pt_idx);
								continue;
							}
							else {
								//check machine info
								MachiningInfo m_info =
									check_part->_machining_info_list[check_part->_current_operation];

								for (int m_idx(0); m_idx < m_info.machine_idx.size(); ++m_idx) {
									if (m_info.machine_idx[m_idx] == i) {
										candidate_part_list.push_back(check_part);
									}
								}
							}
						}
					} //End of part loop
				}
				++pl_iter;
			} //End of pallet_list loop
/* #if (SHOW_DEBUG_MESSAGE)
			printf(" *** [MAchine %d] Selected Candidate *** \n", i);
			for (int i(0); i < candidate_part_list.size(); ++i)
				candidate_part_list[i]->printInfo(i);
#endif */

			//----------------------------------------------------------
			//record the average of remaining operation processing time for a part
			printf("Isthere candidate part??????????????????????\n");
			if (candidate_part_list.size() > 0) { //is there candidate part?
				printf("Yes!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				for (int i(0); i < candidate_part_list.size(); ++i) {
					Part* ch_pt_tmp = candidate_part_list[i];
					printf("part%d, sum_pt%d ", ch_pt_tmp->_part_idx, ch_pt_tmp->_sum_pt);
					//machine info for the part


					int avg_remaining_pt(0);
					int remaining_op(0);
					std::vector<MachiningInfo> m_info2;
					m_info2 = ch_pt_tmp->_machining_info_list;


					int curr_op = ch_pt_tmp->_current_operation;
					for (int j(curr_op); j < m_info2.size(); ++j) {
						int pt_temp(0);
						int num_mac = m_info2[j].machine_name.size();
						printf("opeartion%d ", j);
						for (int k(0); k < m_info2[j].machine_name.size(); ++k) { //machine_idx
							pt_temp += m_info2[j].processing_time[k]; //the sum of remaining operation  processing time for a part
							printf("%d ", m_info2[j].processing_time[k]);
						}
						avg_remaining_pt += pt_temp / num_mac;
					}

					remaining_op = ch_pt_tmp->_num_operation - ch_pt_tmp->_current_operation; //the number of remaining op
					//ch_pt_tmp->_remaining_pt_avg = sum_remaining_pt/alter_mac;  // ��Ʈ�� ���������ð��� ���ġ (���� �����ð��� ��/��ȸӽŰ���)
					ch_pt_tmp->_remaining_pt_avg = avg_remaining_pt;

					printf("--->(sim time%d) part%d, due date%d,  num operation%d(%d), current operation%d, remaining op%d(%d)\n",
						curr_time,
						candidate_part_list[i]->_part_idx,
						candidate_part_list[i]->_due_time,
						candidate_part_list[i]->_num_operation,
						candidate_part_list[i]->_sum_pt,
						candidate_part_list[i]->_current_operation,
						remaining_op,
						candidate_part_list[i]->_remaining_pt_avg);

					printf("the larger one between due and simul time + avg remaining time?");
					int time_temp = curr_time;
					if (candidate_part_list[i]->_due_time
						> (time_temp + candidate_part_list[i]->_remaining_pt_avg)) {
						candidate_part_list[i]->_comparing_var
							= candidate_part_list[i]->_due_time;
					}
					else {
						candidate_part_list[i]->_comparing_var
							= (time_temp + candidate_part_list[i]->_remaining_pt_avg);
					}
					printf("=>%d\n", candidate_part_list[i]->_comparing_var);
				}
			}

			if (candidate_part_list.size() > 0) {
				//Pictk a machine with operation time
				int selected_pt_idx(0);
				Part* ch_pt = candidate_part_list[0];

				int minimum_modified_due = ch_pt->_comparing_var;
				int modified_due(0);
				for (int pt_idx(1); pt_idx < candidate_part_list.size(); ++pt_idx) {
					ch_pt = candidate_part_list[pt_idx];
					modified_due = ch_pt->_comparing_var;
					printf("--->modified due %d\n", modified_due);
					if (minimum_modified_due < modified_due) {
						minimum_modified_due = modified_due;
						selected_pt_idx = pt_idx;
					}
				}
				printf("--->minimum modified due  %d\n", minimum_modified_due);
				//-------------------------------------------------------------


				// Operation(machinning) starts
				Part* selected_part = candidate_part_list[selected_pt_idx];
				Pallet* selected_pallet = pallet_list[selected_part->_pallet_idx];

				//Pallet process starts
				selected_pallet->_in_process = true;
				selected_pallet->_process_name = process::Machining;

				int processing_time(0);  //���� �����ð��� ����ū ��Ʈ�� �´��� üũ�ϱ�
				processing_time = selected_part->getProcessingTime(selected_part->_current_operation, i);

				selected_pallet->_process_duration = processing_time;
				selected_pallet->_current_processing_time = 0;

				//Transportation time
				if (i == 0) {
					selected_pallet->LocationUpdate_Mac1(loc::Machine0, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				if (i == 1) {
					selected_pallet->LocationUpdate_Mac2(loc::Machine1, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				if (i == 2) {
					selected_pallet->LocationUpdate_Mac3(loc::Machine2, selected_pallet,
						machine_pre_pallet[i], processing_time);
				}
				processing_time = selected_pallet->_spt_temp; //_spt_temp:processing time wiht trans time
				printf("processing_time(after LocationUpdate) %d\n", processing_time);

				//Machine starts
				machine_usage[i] = true;
				machine_processing_time[i] = processing_time;
				printf("selected processing time %d\n", machine_processing_time[i]);

				machine_current_time[i] = 0;
				machine_engaged_pallet_idx[i] = selected_pallet->_pallet_idx;
				machine_processing_part[i] = selected_part;

/* #if (SHOW_DEBUG_MESSAGE)
				printf("\n*** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->printfInfo(0);
				selected_pallet->printfInfo(0);
#endif */ 
				//record _pre_mac for adding transportation time
				printf("\n*** [Machine %d] Selected Part & Pallet *** \n", i);
				selected_part->print_PartInfo(0);
				selected_pallet->print_PalletMac(0);

				std::vector<Pallet*>::const_iterator pl_iter = pallet_list.begin();
				while (pl_iter != pallet_list.end()) {
					if ((*pl_iter)->_pallet_idx == selected_pallet[0]._pallet_idx) {
						if (i == 0) {
							(*pl_iter)->_pre_mac = 4;
						}
						else if (i == 1) {
							(*pl_iter)->_pre_mac = 5;
						}
						else if (i == 2) {
							(*pl_iter)->_pre_mac = 6;
						}
					}
					++pl_iter;
				}
			}
			else {
/* #if (SHOW_DEBUG_MESSAGE)
				printf("No candidate part\n");
#endif */
			} //End of if(candidate_part_list.size() > 0)
		} // The end of machine usage
	} // Machine loop
}
//----------------------------




void Algorithm_SetupAndMachining::_Update(const std::vector<Pallet*> & pallet_list) {

	machine_pre_pallet.resize(3);
	for (int i(0); i < _num_Machine; ++i) {
		if (machine_usage[i]) {
			if (machine_processing_time[i] - 1 == machine_current_time[i]) {  // Machining is done

			   // Part: current operaiton +1
				machine_processing_part[i]->_current_operation++;

				// Pallet: Process is done
				//-----------------------------------
				//pre pallet���� 
				machine_pre_pallet[i] = pallet_list[machine_engaged_pallet_idx[i]]->_pallet_idx;
				//-----------------------------------

				pallet_list[machine_engaged_pallet_idx[i]]->_in_process = false;
				pallet_list[machine_engaged_pallet_idx[i]]->LocationUpdate(loc::Buffer,
					pallet_list[machine_engaged_pallet_idx[i]]->_pallet_idx);


				machine_current_time[i] = 0;
				machine_usage[i] = false;
				continue;
			}
			++machine_current_time[i];
		}
	} // End of machine loop
}


void Algorithm_SetupAndMachining::printInfo() {
	printf("Num of Machine: %d\n", _num_Machine);
}



