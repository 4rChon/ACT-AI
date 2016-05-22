#include "Defend.h"
#include "CreateCoalition.h"
#include "EconHelper.h"
#include "CompositionHelper.h"
#include <string>

Defend::Defend(BWAPI::Unit unit)
	:unit(unit)
{
	taskName = "Defend(" + unit->getType().getName() + ", " + std::to_string(unit->getRegion()->getID()) + ")";
	target = MapHelper::getZone(unit->getRegion());	
	taskType = DEF;
	target->defend(true);
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

	int enemyScore = 0;
	int friendScore = 0;

	for (auto &zone : target->getNeighbourhood())
	{
		enemyScore += zone->getEnemyScore();
		friendScore += zone->getFriendScore();
	}

	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() < 5 && enemyScore <= 0 && BWAPI::Broodwar->getFrameCount() - creationFrame > 720)
	{
		succeed();
		return;
	}	

	if(friendScore <= 0)
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
	target->defend(false);
	complete = true;
	profit = 1.0;// coalition->getProfit();
	printDebugInfo("Success!", true);
}

void Defend::fail()
{
	target->defend(false);
	complete = true;
	profit = 0.0;// coalition->getProfit();
	printDebugInfo("Failure!", true);
}
