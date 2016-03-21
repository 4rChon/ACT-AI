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
	int successfulOrders;
public:
	//constructors
	CreateUnit(BWAPI::UnitType unitType, int unitCount = 1);

	BWAPI::UnitType getUnitType();

	//-
	void satisfyRequirements();
	void createCoalition();
	void decrementUnitCount();
	void assign();	
	void act();
	void update();
};