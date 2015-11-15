#pragma once
#include "Agent.h"

class Worker : public Agent
{
private:	
	std::unordered_map<BWAPI::UnitType, double> buildMap;
	std::vector<double> buildWeights;
	BWAPI::UnitType buildType;
	double buildTotalChance;	
public:
	Worker(BWAPI::Unit unit, double freewill);
	
	//-
	void act();

	//commands
	bool attack();
	bool repair();
	bool gather();
	bool build();

	//helpers
	void updateUnlocked();
	void updateBuildWeights();

	void chooseBuilding();
};