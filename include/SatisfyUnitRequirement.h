#pragma once
#include <BWAPI.h>

#include "Task.h"
#include "CreateUnit.h"

class SatisfyUnitRequirement : public Task
{
private:
	BWAPI::UnitType unitType;
	CreateUnit* superTask;
public:
	SatisfyUnitRequirement(BWAPI::UnitType unitType);
	void assign();
	void act();
	void update();
};