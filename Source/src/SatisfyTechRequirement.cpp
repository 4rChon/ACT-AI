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
	printDebugInfo("Acting");	
	printDebugInfo(techType.c_str());

	if (util::getSelf()->allUnitCount(techType.requiredUnit()) < 1)
	{
		CreateUnit* createUnit = new CreateUnit(techType.requiredUnit());
		addSubTask(createUnit);
	}

	if (util::getSelf()->allUnitCount(techType.whatResearches()) < 1)
	{
		CreateUnit* createUnit = new CreateUnit(techType.whatResearches());
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
		if (util::canResearch(techType))
			succeed();
	}
	printDebugInfo("Update End");
}