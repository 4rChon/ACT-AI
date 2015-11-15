#pragma once
#include "Agent.h"

class Producer : public Agent
{
private:
	std::unordered_map<BWAPI::UnitType, double> trainMap;
	std::vector<double> trainWeights;
	BWAPI::UnitType trainType;
	double trainTotalChance;
public:
	//constructors
	Producer(BWAPI::Unit unit, double freewill);

	//-
	void act();

	//commands
	bool train();

	//helpers
	void updateUnlocked();	
	void updateTrainWeights();
	
	void chooseUnit();
};