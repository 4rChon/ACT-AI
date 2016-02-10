#pragma once
#include "ResourceDepot.h"

class CommandCenter : public ResourceDepot
{
public:
	CommandCenter(BWAPI::Unit unit);

	//getters

	//-
	void act();

	//commands
	bool buildAddon(BWAPI::UnitType);

	//helpers
};