#pragma once
#include "Task.h"

class CreateUnit : public Task
{
private:
	BWAPI::UnitType unitType;
	int unitCount;
	bool satisfying;
	bool satisfied;
	bool requiresGas;
public:
	//constructors
	CreateUnit(BWAPI::UnitType unitType, int unitCount = 1);

	//-
	void createCoalition();
	void satisfyRequirements();
	void decrementUnitCount();
	void assign();	
	void act();
	void update();
};