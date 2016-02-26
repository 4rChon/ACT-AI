#include "Attack.h"
#include "Scout.h"
#include "CreateCoalition.h"
#include "EconHelper.h"
#include "ArmyHelper.h"
#include "CoalitionHelper.h"
#include <string>

Attack::Attack(MapHelper::Zone* target)
{
	taskName = "Attack(" + std::to_string(target->getID()) + ")";
	this->target = target;
	taskType = ATT;
	//debug = true;
}

void Attack::createCoalition()
{
	Composition c;
	c.addType(BWAPI::UnitTypes::Terran_Marine, 5);	
	CreateCoalition *createCoalition = new CreateCoalition(c, this);
	addSubTask(createCoalition);
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
	if (!coalition->isActive())
		return;
	coalition->getUnitSet().attack(target->getRegion()->getCenter());
	acting = true;
	printDebugInfo("Acting End");
}

void Attack::update()
{	
	printDebugInfo("Update");

	if (complete)
		return;

	if (!assigned)
		assign();

	if (!acting && assigned)
		act();

	if (BWAPI::Broodwar->getFrameCount() - target->getLastVisited() < 5 && target->getEnemyScore() == 0)
		succeed();

	printDebugInfo("Update End");
}

void Attack::succeed() //move to task?
{
	complete = true;
	profit = 1.0;
	printDebugInfo("Success!", true);
	ArmyHelper::defend();

	CoalitionHelper::removeCoalition(coalition);
	cleanSubTasks();
}

void Attack::fail() //move to task?
{
	complete = true;
	profit = 0.0;
	printDebugInfo("Failure!", true);
	ArmyHelper::defend();

	CoalitionHelper::removeCoalition(coalition);
	cleanSubTasks();
}