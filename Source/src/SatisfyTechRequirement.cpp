#include "SatisfyTechRequirement.h"
#include "CreateUnit.h"
#include "TaskHelper.h"
#include "CoalitionHelper.h"

SatisfyTechRequirement::SatisfyTechRequirement(BWAPI::TechType techType)
{
	taskName = "SatisfyTechRequirement(" + techType.getName() + ")";
	
	this->techType = techType;
	taskType = STR;
	//debug = true;
}

void SatisfyTechRequirement::assign()
{
	assigned = true;
}

void SatisfyTechRequirement::act()
{
	//create units to satisfy requirements
	printDebugInfo("Acting");
	if (util::getSelf()->allUnitCount(techType.requiredUnit()) < 1
		&& util::getSelf()->incompleteUnitCount(techType.requiredUnit()) < 1)
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
		return;

	if (!assigned)
	{
		assign();
		return;
	}

	if (!acting && assigned)
	{
		act();
		return;
	}

	if (acting)
	{
		if (util::getSelf()->isResearchAvailable(techType))
			succeed();
	}
	printDebugInfo("Update End");
}