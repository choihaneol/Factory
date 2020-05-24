#pragma once

#ifndef FACTORY_H
#define FACTORY_H
#include <vector>

class Part;
class Pallet;

class Factory {
public:
	Factory(const std::string & file_name);
	//Factory(int argc1, _TCHAR* argv1[]);
	~Factory();

	// Functions
	bool All_Done();
	void ForwardOneStep();

	void printAllPartStatus();
	void printAllPalletStatus();
	void printTardiness();

	// Variables
	std::vector<Part*> part_list;
	std::vector<Pallet*> pallet_list;

	int Num_LU_Station;
	int Num_Machine;
	int Num_Parttype;
	int Part_Info_Size;
	int Num_Pallet;
	int Num_Dependency;
	int Num_Total_Part;
	int Num_Dependency_Pre;  //dependency predecessor
	int Num_Dependency_Suc;  //dependency successor
	int General_Parttype;    //general 
	int LU_Time;

	//Trnasportation time
	int **Transportation_Time;
	int TransTime;
	int MovingTime;


	// LU station available time
	std::vector<int> Machine_Schedule;

	int _sim_time;

	void printInfo();
};
#endif

