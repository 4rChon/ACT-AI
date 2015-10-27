#include "SatisfyRequirement.h"
#include "Globals.h"
#include "CreateUnit.h"

SatisfyRequirement::SatisfyRequirement(BWAPI::UnitType unitType)
{
	this->unitType = unitType;
}

void SatisfyRequirement::assign()
{
	//std::cout << "Satisfying Requirements\n";
	if (!this->assigned)
	{
		if (!g_isUnlocked[this->unitType.whatBuilds().first])
		{
			CreateUnit* createUnit = new CreateUnit(this->unitType.whatBuilds().first);
			subTasks.push_back(createUnit);
		}
	}

	for each (auto requirement in unitType.requiredUnits())
	{
		if (!g_isUnlocked[requirement.first])
		{
			CreateUnit* createUnit = new CreateUnit(requirement.first);
			subTasks.push_back(createUnit);
		}
	}
	assigned = true;
}