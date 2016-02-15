#pragma once
#include "Task.h"

class CreateUnit : public Task
{
private:
	BWAPI::UnitType unitType;
	int unitCount;
	bool satisfyingTech;
	bool satisfyingUnit;
	bool satisfied;
	bool building;
	bool reserved;
public:
	//constructors
	CreateUnit(BWAPI::UnitType unitType, int unitCount = 1);	

	//-
	void assign();	
	void satisfyUnitRequirements();
	void satisfyTechRequirements();
	void createCoalition();
	void act();
	void update();
};