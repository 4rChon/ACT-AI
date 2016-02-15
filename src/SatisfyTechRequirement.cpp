#include "..\include\SatisfyTechRequirement.h"
#include "..\include\CreateUnit.h"
#include "..\include\TaskHelper.h"

SatisfyTechRequirement::SatisfyTechRequirement(BWAPI::TechType techType)
{
	taskName = "SatisfyTechRequirement(" + techType.getName() + ")";
	this->techType = techType;
	debug = true;
	TaskHelper::addTask(this, false);
}

void SatisfyTechRequirement::assign()
{
	assigned = true;
}

void SatisfyTechRequirement::act()
{
	//create units to satisfy requirements
	printDebugInfo("Acting");
	if (AgentHelper::getTypeCountMap()[techType.requiredUnit()] == 0
		&& BWAPI::Broodwar->self()->incompleteUnitCount(techType.requiredUnit() == 0))
	{
		printDebugInfo(techType.c_str());
		CreateUnit* createUnit = new CreateUnit(techType.requiredUnit());
		subTasks.insert(createUnit);
	}
	acting = true;
	printDebugInfo("Acting End");
}

void SatisfyTechRequirement::update()
{
	printDebugInfo("Update");
	if (complete)
		return;

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