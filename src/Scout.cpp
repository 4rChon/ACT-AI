#include "Scout.h"
#include "CreateCoalition.h"

Scout::Scout(Zone* target)
{
	this->taskType = SCO;
	this->taskName = "Scout(Zone*)";
	this->target = target;
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
		std::cout << "Scout: Acting\n";
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

	if (this->assigned && this->coalition->isActive())
		this->act();

	if (target->getConfidence() > 0.8)
	{
		this->complete = true;
		std::cout << "Scout: Complete\n";
	}
	else if (this->acting)
	{
		std::cout << "Acting with " << this->coalition->getUnitSet().size() << "\n";
		if (this->coalition->getUnitSet().size() == 0)
		{
			this->complete = true;
			std::cout << "Scout: Failed\n";
		}
	}
}

