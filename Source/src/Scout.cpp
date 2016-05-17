#include "Scout.h"
#include "CreateCoalition.h"
#include "CompositionHelper.h"
#include "ArmyHelper.h"
#include <string>

Scout::Scout(Zone* target)
{
	taskName = "Scout(" + std::to_string(target->getID()) + ")";
	this->target = target;
	taskType = SCO;
	//debug = true;
}

void Scout::assign()
{
	printDebugInfo("Assign");
	createCoalition();
	assigned = true;
	printDebugInfo("Assign End");
}

void Scout::act()
{
	printDebugInfo("Acting");	
	acting = coalition->getUnitSet().move(target->getRegion()->getCenter());
	printDebugInfo("Acting End");
}

void Scout::update()
{
	printDebugInfo("Update");
	if (complete)
		return;

	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() < 5)
	{
		succeed();
		return;
	}

	if (!assigned)
	{
		assign();
		return;
	}

	if (coalition->isActive())
	{
		act();
		return;
	}	

	printDebugInfo("Update End");
}

void Scout::succeed()
{
	complete = true;
	profit = 1.0;
	printDebugInfo("Success!", true);

	ArmyHelper::stopScouting();

	deleteSubTasks();
}

