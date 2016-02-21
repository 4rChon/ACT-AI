#include "..\include\SatisfyUnitRequirement.h"
#include "..\include\CreateUnit.h"
#include "..\include\ResearchTech.h"
#include "..\include\TaskHelper.h"
#include "..\include\CoalitionHelper.h"

SatisfyUnitRequirement::SatisfyUnitRequirement(BWAPI::UnitType unitType)
{
	taskName = "SatisfyUnitRequirement(" + unitType.getName() + ")";
	
	this->unitType = unitType;
	debug = false;
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
		printDebugInfo("Require : " + requirement.first.getName());
		if (!BWAPI::Broodwar->self()->hasUnitTypeRequirement(requirement.first, requirement.second)
			&& BWAPI::Broodwar->self()->incompleteUnitCount(requirement.first) < requirement.second)
		{
			CreateUnit* createUnit = new CreateUnit(requirement.first, requirement.second - AgentHelper::getTypeCount(requirement.first));
			printDebugInfo(" Creating : " + requirement.first.getName() + " : " + std::to_string(requirement.second));
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
	{
		cleanSubTasks();
		return;
	}

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

