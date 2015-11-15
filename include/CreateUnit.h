#pragma once
#include "Task.h"

class CreateUnit : public Task
{
private:
	BWAPI::UnitType unitType;
	int unitCount;
	bool satisfied;
public:
	//constructors
	CreateUnit(BWAPI::UnitType unitType, int unitCount = 1);

	//-
	void assign();
	void act();	
	void update();
};