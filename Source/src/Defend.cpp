#include "Defend.h"
#include "CreateCoalition.h"
#include "EconHelper.h"
#include "CompositionHelper.h"
#include <string>

Defend::Defend(BWAPI::Unit unit)
{
	taskName = "Defend(" + unit->getType().getName() + ", " + std::to_string(unit->getRegion()->getID()) + ")";
	this->unit = unit;
	this->target = MapHelper::getZone(unit->getRegion());	
	taskType = DEF;
}

void Defend::assign()
{
	//ArmyHelper::addDefendZone(target);
	createCoalition();
	assigned = true;
}

void Defend::act()
{
	printDebugInfo("Acting");
	if (coalition->isActive() || util::getSelf()->supplyUsed() >= 400)
	{
		coalition->getUnitSet().attack(target->getRegion()->getCenter());
		acting = true;		
	}
	printDebugInfo("Acting End");
}

void Defend::update()
{
	printDebugInfo("Update");

	if (complete)
		return;

	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() < 5 && target->getEnemyScore() == 0)
	{		
		succeed();
		return;
	}

	if (unit->getHitPoints() == 0)
	{
		
		fail();
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

void Defend::succeed()
{
	target->setDefending(false);
	complete = true;
	profit = coalition->getProfit();
	printDebugInfo("Success!", true);

	cleanSubTasks();
}
