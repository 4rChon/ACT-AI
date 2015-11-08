#pragma once
#include "Task.h"

class CreateUnit : public Task
{
private:
	BWAPI::UnitType unitType;
	bool satisfied = true;
	int unitCount;
public:
	//constructors
	CreateUnit(BWAPI::UnitType unitType, int unitCount);

	//-
	void assign();
	void act();	
	void update();
};