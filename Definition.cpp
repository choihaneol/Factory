#ifndef DEFINITION_H
#define DEFINITION_H

#define NUM_FIXTURE_TYPE 2
#include <string>
#include <vector>

namespace loc {
	constexpr int Outside = 0; // Out of system
	constexpr int LoadingStation = 1;
	constexpr int Buffer = 2;
	constexpr int Machine = 3;
	//---------------------------------------------------
	constexpr int Machine1 = 4;  //follow variable 'Machine = 3'
	constexpr int Machine2 = 5;
	constexpr int Machine3 = 6;
	//---------------------------------------------------


	inline std::string printLocation(int loc) {
		std::string pallet_loc_str;
		if (loc == loc::Outside) { pallet_loc_str = "Outside"; }
		if (loc == loc::LoadingStation) { pallet_loc_str = "LoadingStation"; }
		if (loc == loc::Buffer) { pallet_loc_str = "Buffer"; }
		if (loc == loc::Machine) { pallet_loc_str = "Machine"; }
		//--------------------------------------------------
		if (loc == loc::Machine1) { pallet_loc_str = "Machine1"; }
		if (loc == loc::Machine2) { pallet_loc_str = "Machine2"; }
		if (loc == loc::Machine3) { pallet_loc_str = "Machine3"; }
		//---------------------------------------------------

		return pallet_loc_str;
	}
};

namespace process {
	constexpr int Nothing = -1;
	constexpr int Loading = 0;
	constexpr int Unloading = 1;
	constexpr int Machining = 2;
};

struct MachiningInfo {
	std::vector<int> machine_idx;
	std::vector<std::string> machine_name;
	std::vector<int> processing_time;
};

#endif