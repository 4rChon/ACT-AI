#include "Attack.h"
#include "Composition.h"
#include "GlobalSets.h"
#include "CreateCoalition.h"

Attack::Attack(Zone* target)
{	
	this->taskType = ATT;
	this->taskName = "Attack(Zone*)";
	this->target = target;
}

// assign an attacking coalition
void Attack::assign()
{
	if (!this->assigned)
	{
		std::cout << "Attack: Assign\n";
		Composition c;
		c.addType(BWAPI::UnitTypes::Terran_Marine, 10);
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
	if (this->assigned && this->coalition->isActive())
		act();
	
	if ((!this->complete && this->target->getConfidence() > 0.8 && this->target->getEnemyScore() == 0))
	{
		std::cout << "Attack: Complete\n";
		this->complete = true;
		this->coalition->disband();
		this->cleanSubTasks();
		g_Tasks.remove(this);		
	}
}