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

	//getters
	BWAPI::UnitType getUnitType()	{ return unitType; }
	int getUnitCount()				{ return unitCount; }
	bool isSatisfying()				{ return satisfying; }
	bool isSatisfied()				{ return satisfied; }
	bool isRequiresGas()			{ return requiresGas; }

	//-
	void createCoalition();
	void satisfyRequirements();
	void decrementUnitCount()		{ unitCount--; }
	void assign();	
	void act();
	void update();
};