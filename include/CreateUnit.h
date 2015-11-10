#pragma once
#include "Task.h"

class CreateUnit : public Task
{
private:
	BWAPI::UnitType unitType;
	int unitCount;
public:
	//constructors and destructors
	CreateUnit(BWAPI::UnitType unitType, int unitCount = 1);
	~CreateUnit();

	//-
	void assign();
	void act();	
	void update();
};