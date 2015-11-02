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

		for each (auto requirement in unitType.requiredUnits())
		{
			std::cout << "required unit: " << requirement.first.c_str() << "\n";
			if (g_TotalCount[requirement.first] < requirement.second)
			{
				//std::cout << requirement.first.c_str() << " requirement\n";
				CreateUnit* createUnit = new CreateUnit(requirement.first);
				std::cout << "Creating Requirement: " << requirement.first.c_str() << "\n";
				subTasks.push_back(createUnit);
			}
		}
		assigned = true;
	}
}