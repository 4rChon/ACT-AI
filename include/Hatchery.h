#pragma once
#include "ResourceDepot.h"

class Hatchery : public ResourceDepot
{
public:
	Hatchery(BWAPI::Unit unit);

	//getters	

	//-	
	void act();

	//commands
	bool train(BWAPI::UnitType unitType);
	bool morph();
	bool upgrade(BWAPI::UpgradeType upgradeType);

	//helpers
};