#pragma once
#include "Task.h"

class CreateUnit : public Task
{
private:
	BWAPI::UnitType unitType;
	int unitCount;
	bool satisfying;
	bool satisfied;
	bool building;
	bool reserved;
	bool requiresGas;
	int successfulOrders;
public:
	//constructors
	CreateUnit(BWAPI::UnitType unitType, int unitCount = 1);	

	//-
	void assign();
	void satisfyRequirements();
	void createCoalition();
	void decrementUnitCount();
	void act();
	void update();
};