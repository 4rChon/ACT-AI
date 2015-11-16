#include "Attack.h"
#include "Composition.h"
#include "GlobalSets.h"
#include "CreateCoalition.h"
#include "Scout.h"

Attack::Attack(Zone* target)
{	
	this->taskType = ATT;
	this->taskName = "Attack(Zone*)";
	this->target = target;
	this->scouting = false;
}

// assign an attacking coalition
void Attack::assign()
{
	if (!this->assigned)
	{
		if (target->getConfidence() < 0.8)
		{
			if (!scouting)
			{
				Scout* scout = new Scout(target);
				this->addSubTask(scout);
				scouting = true;
			}			
		}

		std::cout << "Attack: Assign\n";
		Composition c;
		c.addType(BWAPI::UnitTypes::Terran_Marine, 5 + g_TotalCount[BWAPI::UnitTypes::Terran_Marine]);
		//for (auto unitType : g_TotalCount)
		//	if (!unitType.first.isWorker() && !unitType.first.isBuilding() && unitType.first != BWAPI::UnitTypes::Terran_Marine)
		//		c.addType(unitType.first, unitType.second);
		//c.addType(BWAPI::UnitTypes::Terran_Firebat, 5);
		//c.addType(BWAPI::UnitTypes::Terran_Medic, 5);
		//c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 10);
		CreateCoalition *createCoalition = new CreateCoalition(c, this);
		addSubTask(createCoalition);
		this->assigned = true;
	}
}

// attack with coalition
void Attack::act()
{
	if (!this->acting)
	{
		std::cout << "Attack: Act\n";
		this->coalition->getUnitSet().attack(this->target->getRegion()->getCenter());
		this->acting = true;
	}
}

void Attack::update()
{
	if (this->complete)
	{
		this->cleanSubTasks();
		return;
	}
	
	if (this->assigned && this->coalition->isActive())
		act();

	if (this->target->getConfidence() > 0.8 && this->target->getEnemyScore() == 0)
	{
		this->complete = true;
		std::cout << "Attack: Complete\n";
	}
	else if (this->acting)
	{
		if (this->coalition->getUnitSet().size() == 0)
		{
			this->complete = true;
			std::cout << "Attack: Failed\n";
		}
	}
}