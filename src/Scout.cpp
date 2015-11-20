#include "Scout.h"
#include "CreateCoalition.h"
#include "Threatfield.h"
#include "GlobalVariables.h"

Scout::Scout(Zone* target)
{
	this->target = target;
	this->taskType = SCO;
	this->taskName = "Scout()";
}

void Scout::assign()
{
	if (!this->assigned)
	{
		std::cout << "Scout: Assign\n";
		Composition c;
		c.addType(BWAPI::UnitTypes::Terran_SCV, 1);
		//c.addType(BWAPI::UnitTypes::Terran_Firebat, 5);
		//c.addType(BWAPI::UnitTypes::Terran_Medic, 5);
		//c.addType(BWAPI::Broodwar->self()->getRace().getWorker(), 10);
		CreateCoalition *createCoalition = new CreateCoalition(c, this);
		addSubTask(createCoalition);
		this->assigned = true;
	}
}

void Scout::act()
{
	if (!this->acting)
	{
		std::cout << "Scout: Acting - Scouting " << this->target->getRegion()->getCenter() << "\n";
		this->coalition->getUnitSet().move(this->target->getRegion()->getCenter());
			
		this->acting = true;
	}
}

void Scout::update()
{
	if (this->complete)
	{
		this->cleanSubTasks();
		return;
	}

	if (this->assigned)
	if (this->coalition->isActive())
	if (this->assigned && this->coalition->isActive())
		this->act();

	if (target->getConfidence() > 0.8)
	{
		this->complete = true;
		std::cout << "Scout: Complete\n";
	}
	else if (this->acting)
	{
		if (this->coalition->getUnitSet().size() == 0)
		{
			this->complete = true;
			std::cout << "Scout: Failed\n";
		}

		for (auto &unit : this->coalition->getUnitSet())
		{
			if (unit->isMoving())
				return;
		}
		this->complete = true;
		std::cout << "Scout: Failed\n";
	}
}

