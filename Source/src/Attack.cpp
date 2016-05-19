#include "Attack.h"
#include "Scout.h"
#include "CreateCoalition.h"
#include "ArmyHelper.h"
#include "CoalitionHelper.h"
#include "CompositionHelper.h"
#include "EconHelper.h"
#include <string>

Attack::Attack(Zone* const& target)
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

	if (util::game::getSelf()->supplyUsed() >= 400)
		coalition->activate();

	if (coalition->isActive())
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

	if (coalition)
	{
		if (coalition->getTargetComp().getCost() == 0)
		{
			fail();
			return;
		}
	}

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

	if (!acting)
		act();

	printDebugInfo("Update End");
}

void Attack::fail()
{
	complete = true;
	profit = coalition->getProfit();
	printDebugInfo("Failure!", true);

	ArmyHelper::defend();
	ArmyHelper::updateTargetPriority();

	deleteSubTasks();
}

void Attack::succeed()
{
	complete = true;
	profit = coalition->getProfit();
	printDebugInfo("Success!", true);

	ArmyHelper::defend();
	ArmyHelper::updateTargetPriority();
	ArmyHelper::clearZoneTargets(target);

	deleteSubTasks();
}