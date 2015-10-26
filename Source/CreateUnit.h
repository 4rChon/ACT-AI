#pragma once
#include "Task.h"

class CreateUnit : public Task
{
private:
	BWAPI::UnitType unitType;
public:
	//constructors
	CreateUnit(BWAPI::UnitType unitType);

	//-
	void assign();
	void act();	
	void update();
};