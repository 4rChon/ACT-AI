#include "Defend.h"
#include "CreateCoalition.h"

Defend::Defend(Zone* target)
{
	this->taskType = DEF;
	this->taskName = "Defend(Zone*)";
	this->target = target;
}

void Defend::assign()
{
	if (!this->assigned)
	{
		std::cout << "Defend: Assign\n";
		Composition c;
		c.addType(BWAPI::UnitTypes::Terran_Marine, 5 + g_TotalCount[BWAPI::UnitTypes::Terran_Marine]);
		/*for (auto unitType : g_TotalCount)
			if (!unitType.first.isWorker() && !unitType.first.isBuilding() && unitType.first != BWAPI::UnitTypes::Terran_Marine)
				c.addType(unitType.first, unitType.second);*/
		//c.addType(BWAPI::UnitTypes::Terran_Firebat, 5);
		//c.addType(BWAPI::UnitTypes::Terran_Medic, 5);
		//c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 10);
		CreateCoalition *createCoalition = new CreateCoalition(c, this);
		addSubTask(createCoalition);
		this->assigned = true;
	}
}

void Defend::act()
{
	if (!this->acting)
	{
		std::cout << "Defend: Act\n";
		this->coalition->getUnitSet().attack(this->target->getRegion()->getCenter());
		this->acting = true;
	}
}

void Defend::update()
{
	if (this->complete)
	{
		this->cleanSubTasks();
		return;
	}

	if (this->assigned && this->coalition->isActive())
		act();

	if (this->target->getConfidence() > 0.8 && this->target->getEnemyScore() == 0 )
	{
		this->complete = true;
		std::cout << "Defend: Complete\n";
	}
}