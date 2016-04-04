#pragma once
#include "Worker.h"


class SCV : public Worker
{	
public:
	///constructor
	SCV(BWAPI::Unit unit);
			
	///-
	//Compute actions
	void act();

	///commands
	//Repair target unit
	bool repair(BWAPI::Unit unit);

	//Repair nearby units
	bool repair();

	//Defend target by constructing defensive structures
	bool defend(BWAPI::PositionOrUnit target);
};