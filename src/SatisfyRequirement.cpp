#include "SatisfyRequirement.h"
#include "GlobalSets.h"
#include "CreateUnit.h"

SatisfyRequirement::SatisfyRequirement(BWAPI::UnitType unitType)
{
	this->unitType = unitType;
}

void SatisfyRequirement::assign()
{	
	if (!this->assigned)
	{
		std::cout << "SatisfyRequirement: Assign\n";
		this->assigned = true;
	}
}

void SatisfyRequirement::act()
{
	if (!this->acting)
	{
		std::cout << "SatisfyRequirement: Acting\n";
		for each (auto requirement in unitType.requiredUnits())
		{
			std::cout << "required unit: " << requirement.first.c_str() << "\n";
			if (g_TotalCount[requirement.first] < requirement.second)
			{
				//std::cout << requirement.first.c_str() << " requirement\n";
				//if requirement is under construction
				//continue
				//else
				CreateUnit* createUnit = new CreateUnit(requirement.first, requirement.second - g_TotalCount[requirement.first]);
				std::cout << "Creating Requirement: " << requirement.first.c_str() << "\n";
				subTasks.push_back(createUnit);
			}
		}

		/*if (g_TotalCount[unitType.whatBuilds().first] < unitType.whatBuilds().second)
		if (g_TotalCount[unitType.whatBuilds().first] == 0)
		continue;
		else
		{
		CreateUnit* createUnit = new CreateUnit(this->unitType.whatBuilds().first);
		subTasks.push_back(createUnit);
		}*/
		//if (!g_isUnlocked[this->unitType.whatBuilds().first])
		//{
		//	CreateUnit* createUnit = new CreateUnit(this->unitType.whatBuilds().first);
		//	subTasks.push_back(createUnit);
		//}
		acting = true;
	}
}

void SatisfyRequirement::update()
{
	if (this->complete)
	{
		this->cleanSubTasks();
		return;
	}
	else
		act();

	if (this->acting)// || abandonChance > 100000))
	{
		for (auto t : this->subTasks)
			if (!t->isComplete())
				return;
		this->complete = true;
		std::cout << "SatisfyRequirement: Complete\n";
	}
}

