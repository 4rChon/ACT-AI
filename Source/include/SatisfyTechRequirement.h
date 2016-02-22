#pragma once
#include <BWAPI.h>

#include "Task.h"
#include "ResearchTech.h"

class SatisfyTechRequirement : public Task
{
private:
	BWAPI::TechType techType;
public:
	SatisfyTechRequirement(BWAPI::TechType techType);
	void assign();
	void act();
	void update();
};