#include "..\include\SatisfyUnitRequirement.h"
#include "..\include\CreateUnit.h"
#include "..\include\ResearchTech.h"
#include "..\include\TaskHelper.h"

SatisfyUnitRequirement::SatisfyUnitRequirement(BWAPI::UnitType unitType)
{
	taskName = "SatisfyUnitRequirement(" + unitType.getName() + ")";
	
	this->unitType = unitType;
}

void SatisfyUnitRequirement::assign()
{
	assigned = true;
}

void SatisfyUnitRequirement::act()
{
	//create units to satisfy requirements
	printDebugInfo("Acting");
	for (auto &requirement : unitType.requiredUnits())
	{
		std::cout << "required unit: " << requirement.first.c_str() << "\n";
		if (AgentHelper::getTypeCountMap()[requirement.first] < requirement.second)
		{
			if (BWAPI::Broodwar->self()->incompleteUnitCount(requirement.first) >= requirement.second)
				continue;
			CreateUnit* createUnit = new CreateUnit(requirement.first, requirement.second - AgentHelper::getTypeCountMap()[requirement.first]);
			std::cout << "Creating Requirement: " << requirement.first.c_str() << " : " << requirement.second << "\n";
			addSubTask(createUnit);
		}
	}

	if(unitType.requiredTech() != BWAPI::TechTypes::None)
	{
		ResearchTech* researchTech = new ResearchTech(unitType.requiredTech());
		addSubTask(researchTech);
	}

	acting = true;
	printDebugInfo("Acting End");
}

void SatisfyUnitRequirement::update()
{
	printDebugInfo("Update");
	if (complete)
		return;

	if (!assigned)
		assign();				

	if(!acting)
		act();

	if (acting)
	{
		for (auto &task : subTasks)
			if (!task->isComplete())
				return;
		succeed();
	}
	printDebugInfo("Update End");
}

