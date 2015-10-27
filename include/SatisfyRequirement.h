#pragma once
#include <BWAPI.h>

#include "Task.h"

class SatisfyRequirement : public Task
{
private:
	BWAPI::UnitType unitType;
public:
	SatisfyRequirement(BWAPI::UnitType unitType);
	void assign();
};