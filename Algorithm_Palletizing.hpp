#pragma once

#ifndef ALGORITHM_PALLETIZING_H
#define ALGORITHM_PALLETIZING_H

#include <vector>

class Part;
class Pallet;

class Algorithm_Palletizing {
public:
	Algorithm_Palletizing();
	~Algorithm_Palletizing();

	void run(const std::vector<Part*> & part_list,
		std::vector<Pallet*> & pallet_list);
	void print_PalletizingInfo(int part_type);


private:
	void _EariestDueDateMethod(const std::vector<Part*> & part_list,
		std::vector<Pallet*> & pallet_list);
};

#endif