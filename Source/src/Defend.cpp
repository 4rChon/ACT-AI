#include "Defend.h"
#include "CreateCoalition.h"
#include "EconHelper.h"
#include "CompositionHelper.h"
#include <string>

Defend::Defend(BWAPI::Unit unit)
{
	taskName = "Defend(" + unit->getType().getName() + ", " + std::to_string(unit->getRegion()->getID()) + ")";
	this->unit = unit;
	target = MapHelper::getZone(unit->getRegion());	
	taskType = DEF;
	target->setDefending(true);
}

void Defend::assign()
{
	//createCoalition();
	assigned = true;
}

void Defend::act()
{
	printDebugInfo("Acting");
	acting = true;		
	printDebugInfo("Acting End");
}

void Defend::update()
{
	printDebugInfo("Update");

	if (complete)
		return;

	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() < 5 && target->getEnemyScore() <= 0)
	{		
		succeed();
		return;
	}

	if (target->getFriendScore() == 0)
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
	profit = 1.0;// coalition->getProfit();
	printDebugInfo("Success!", true);

	cleanSubTasks();
}

void Defend::fail()
{
	target->setDefending(false);
	complete = true;
	profit = 0.0;// coalition->getProfit();
	printDebugInfo("Failure!", true);

	cleanSubTasks();
}
