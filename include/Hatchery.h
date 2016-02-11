#pragma once
#include "ResourceDepot.h"

class Hatchery : public ResourceDepot
{
public:
	//constructor
	Hatchery(BWAPI::Unit unit);

	//-	
	void act();
	bool upgrade(BWAPI::UpgradeType upgradeType);
	
};