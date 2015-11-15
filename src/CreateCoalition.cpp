#include "CreateCoalition.h"
#include "CreateUnit.h"
#include "GlobalSets.h"

CreateCoalition::CreateCoalition(Composition composition, Task* task)
{	
	this->taskType = CRC;
	this->taskName = "CreateCoalition(Composition, Task*)";

	this->taskCoalition = new Coalition(composition, task->getType());
	task->setCoalition(this->taskCoalition);

	g_Coalitions.insert(this->taskCoalition);	

	this->cost = composition.getCost();
	this->assign();
}

bool CreateCoalition::getCoalitionState()
{
	return this->taskCoalition->isActive();
}

// add coalition to open coalitions
void CreateCoalition::assign()
{
	if (!this->assigned)
	{		
		std::cout << "CreateCoalition: Assign\n";
		g_OpenCoalitions.insert(this->taskCoalition);
		this->assigned = true;
	}
}

// attempt to activate coalition
void CreateCoalition::act()
{	
	if (!this->acting)
	{		
		std::cout << "CreateCoalition: Acting\n";
		Composition differenceComposition = this->taskCoalition->getTargetComp() - this->taskCoalition->getCurrentComp();
		for (auto unitType : differenceComposition.getTypes())
		{
			if (differenceComposition[unitType] > 0)
			{
				std::cout << "I need " << differenceComposition[unitType] << " more " << unitType.c_str() << "\n";
				CreateUnit *createUnit = new CreateUnit(unitType, differenceComposition[unitType]);
				this->addSubTask(createUnit);
			}
		}
		this->acting = true;
	}
}

void CreateCoalition::update()
{
	/*double abandonChance = (((double)rand() / RAND_MAX) * this->getCost() + ((BWAPI::Broodwar->getFrameCount() - this->age) / this->getCost()));
	if (abandonChance <= 100000)*/
	if (this->complete)
	{		
		this->cleanSubTasks();
		return;
	}
	
	if (this->assigned)
		act();

	if (this->taskCoalition->isActive())// || abandonChance > 100000))
	{		
		this->complete = true;
		std::cout << "CreateCoalition: Complete\n";
	}
}

double CreateCoalition::getCost()
{
	this->cost = this->taskCoalition->getTargetComp().getCost() - this->taskCoalition->getCurrentComp().getCost();
	for (auto task : this->subTasks)
		this->cost += task->getCost();

	return this->cost;
}