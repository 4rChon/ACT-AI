#include "SatisfyTechRequirement.h"
#include "CreateUnit.h"
#include "TaskHelper.h"
#include "CoalitionHelper.h"

SatisfyTechRequirement::SatisfyTechRequirement(BWAPI::TechType techType)
{
	taskName = "SatisfyTechRequirement(" + techType.getName() + ")";
	
	this->techType = techType;
	taskType = STR;
	debug = false;
}

void SatisfyTechRequirement::assign()
{
	assigned = true;
}

void SatisfyTechRequirement::act()
{
	//create units to satisfy requirements
	printDebugInfo("Acting");
	if (AgentHelper::getTypeCount(techType.requiredUnit()) < 1
		&& BWAPI::Broodwar->self()->incompleteUnitCount(techType.requiredUnit()) < 1)
	{
		printDebugInfo(techType.c_str());
		CreateUnit* createUnit = new CreateUnit(techType.requiredUnit());
		addSubTask(createUnit);
	}
	acting = true;
	printDebugInfo("Acting End");
}

void SatisfyTechRequirement::update()
{
	printDebugInfo("Update");
	if (complete)
	{
		cleanSubTasks();
		return;
	}

	if (!assigned)
		assign();

	if (!acting)
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