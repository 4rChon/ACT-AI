#include "Attack.h"
#include "Scout.h"
#include "CreateCoalition.h"
#include "ArmyHelper.h"
#include "CoalitionHelper.h"
#include "EconHelper.h"
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
	c.addType(BWAPI::UnitTypes::Terran_Marine, (int)(5 * EconHelper::getUnitMultiplier()));
	c.addType(BWAPI::UnitTypes::Terran_Medic, (int)(5 * EconHelper::getUnitMultiplier()));
	c.addType(BWAPI::UnitTypes::Terran_Firebat, (int)(5 * EconHelper::getUnitMultiplier()));
	//c.addType(BWAPI::UnitTypes::Terran_Marine, 5);
	//c.addType(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, 1 + (int)(1 * EconHelper::getUnitMultiplier()));
	for (auto &unit : BWAPI::Broodwar->self()->getUnits())
	{
		if (!unit->getType().isWorker() && !unit->getType().isBuilding() && !unit->getType().isSpell())
			c.addType(unit->getType(), 1);
	}
	//c.addType(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, 5);
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

	if (!acting && assigned || BWAPI::Broodwar->self()->supplyUsed() == 200)
		act();

	printDebugInfo("Update End");
}