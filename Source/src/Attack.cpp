#include "Attack.h"
#include "Scout.h"
#include "CreateCoalition.h"
#include "ArmyHelper.h"
#include "CoalitionHelper.h"
#include "CompositionHelper.h"
#include "EconHelper.h"
#include <string>

Attack::Attack(MapHelper::Zone* target)
{
	taskName = "Attack(" + std::to_string(target->getID()) + ")";
	this->target = target;
	taskType = ATT;
	//debug = true;
}

// assign an attacking coalition
void Attack::assign()
{
	printDebugInfo("Assign");	
	createCoalition();
	assigned = true;
	printDebugInfo("Assign End");
}

// attack with coalition
void Attack::act()
{	
	printDebugInfo("Acting");			
	if (coalition->isActive() || util::getSelf()->supplyUsed() >= 400)
	{
		coalition->getUnitSet().attack(target->getRegion()->getCenter());
		acting = true;
	}
	printDebugInfo("Acting End");
}

void Attack::update()
{	
	printDebugInfo("Update");

	if (complete)
		return;

	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() < 5 && target->getEnemyScore() == 0)
	{
		succeed();
		return;
	}

	if (!assigned)
	{
		assign();
		return;
	}

	if (!acting && assigned)
		act();

	printDebugInfo("Update End");
}

void Attack::succeed()
{
	complete = true;
	profit = coalition->getProfit();
	printDebugInfo("Success!", true);

	ArmyHelper::defend();
	ArmyHelper::updateTargetPriority();
	ArmyHelper::clearZoneTargets(target);

	cleanSubTasks();
}