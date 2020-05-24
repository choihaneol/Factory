#pragma once

#ifndef AlGORITHM_LOADINGUNUNLOADING_H
#define AlGORITHM_LOADINGUNUNLOADING_H

#include  <vector>

class Pallet;
class Factory;

class Algorithm_LoadingUnloading {
public:
	Algorithm_LoadingUnloading(const Factory & factory);
	~Algorithm_LoadingUnloading();

	void run(int curr_time, const std::vector<Pallet*> & pallet_list);
	void print_LUStationInfo();
private:
	std::vector<bool> LU_station_usage;
	std::vector<int> LU_station_task_type; // 1:Loading, -1 Unloading, other: 0
	std::vector<int> LU_station_using_time;
	std::vector<int> LU_station_engaged_pallet_idx;
	int _LU_time;
	int _num_LU_station;
	int _moving_time;

	void _FirstInFirstOut(int curr_time, const std::vector<Pallet*> & pallet_list);
	void _Update(int curr_time, const std::vector<Pallet*> & pallet_list);
};

#endif