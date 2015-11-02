#include "Attack.h"
#include "Composition.h"
#include "Globals.h"
#include "CreateCoalition.h"

Attack::Attack(Zone* target)
{	
	this->taskType = ATT;
	this->taskName = "Attack(Zone*)";
	this->target = target;
	std::cout << "Attack(" << this->target->getRegion()->getID() << ")\n";
}

void Attack::assign()
{
	if (!this->assigned)
	{
		//Scout* scout = new Scout(target);
		//subTasks.push_back(scout);

		Composition composition;
		composition.addType(BWAPI::UnitTypes::Terran_Marine, 20);		
		CreateCoalition* createCoalition = new CreateCoalition(composition, this);		
		subTasks.push_back(createCoalition);
		this->assigned = true;
	}
}

void Attack::act()
{
	if (!acting)
	{
		if (this->coalition->isActive())
		{
			if (!this->acting && this->assigned)
			{
				this->coalition->getUnitSet().attack(this->target->getRegion()->getCenter());
				acting = true;
			}
		}
	}
}

void Attack::update()
{
	if (coalition->isActive())
	{
		act();

		cleanSubTasks();

		this->complete = true;
		//coalition->disband();

		g_Tasks.remove(this);
	}
}