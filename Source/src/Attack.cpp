#include "Attack.h"
#include "Scout.h"
#include "CreateCoalition.h"
#include "EconHelper.h"
#include <string>

Attack::Attack(MapHelper::Zone* target)
{
	taskName = "Attack(" + std::to_string(target->getID()) + ")";
	this->target = target;
	debug = false;
	scouting = false;
	scoutingFinished = false;
}

void Attack::createCoalition()
{
	Composition c;
	c.addType(BWAPI::UnitTypes::Protoss_Dragoon, 5);
	c.addType(BWAPI::UnitTypes::Protoss_Zealot, 5);
	c.addType(BWAPI::UnitTypes::Protoss_Observer, 1);
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	addSubTask(createCoalition);
}

void Attack::scout()
{
	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() > 5)
	{
		if (!scouting)
		{
			Scout* scout = new Scout(target);
			addSubTask(scout);
			scouting = true;
		}
	}
	else
		scoutingFinished = true;
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
	scout();
	if (!scoutingFinished)
		return;

	if (!coalition->isActive())
		return;
	coalition->getUnitSet().attack(target->getRegion()->getCenter());
	acting = true;
	printDebugInfo("Acting End");
}

void Attack::update()
{	
	printDebugInfo("Update");
	if (acting && coalition->getAgentSet().size() == 0)
	{
		printDebugInfo("Coalition is dead", true);
		fail();
	}

	if (complete)
	{
		cleanSubTasks();
		return;
	}	

	if (!assigned)
		assign();

	if (!acting && assigned)
		act();

	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() < 5 && target->getEnemyScore() == 0)
		succeed();

	printDebugInfo("Update End");
}