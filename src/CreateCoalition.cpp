#include "CreateCoalition.h"
#include "CreateUnit.h"
#include "Globals.h"

CreateCoalition::CreateCoalition(Composition composition, Task* task)
{	
	this->taskType = CRC;
	this->taskName = "CreateCoalition(Composition, Task*)";
	this->taskCoalition = new Coalition(composition, task->getType());
	task->setCoalition(this->taskCoalition);
	g_Coalitions.insert(taskCoalition);
	g_OpenCoalitions.insert(taskCoalition);
}

bool CreateCoalition::getCoalitionState()
{
	return this->taskCoalition->isActive();
}

void CreateCoalition::assign()
{
	if (!this->assigned)
	{
		for (auto unitType : taskCoalition->getTargetComp().getTypes())
		{
			std::cout << unitType.c_str() << "\n";
			BWAPI::Broodwar->registerEvent([this, unitType](BWAPI::Game*)
			{
				if (this->taskCoalition->getCurrentComp()[unitType] < this->taskCoalition->getTargetComp()[unitType])
				{
					std::cout << "Coalition Requires more " << unitType.c_str() << "\n";
					CreateUnit* createUnit = new CreateUnit(unitType);
					this->subTasks.push_back(createUnit);
				}
			},
			[this](BWAPI::Game*){return !this->taskCoalition->isActive(); },
			-1,//parameter
			unitType.buildTime());
		}
		this->assigned = true;
	}
}

void CreateCoalition::act()
{
	if (!acting)
	{
		for (auto unitType : taskCoalition->getTargetComp().getTypes())
		{
			if (taskCoalition->getCurrentComp()[unitType] < taskCoalition->getTargetComp()[unitType])
			{
				//std::cout << "Coalition Requires more " << unitType.c_str() << "\n";			
				//CreateUnit* createUnit = new CreateUnit(unitType);
				//this->subTasks.push_back(createUnit);
			}
		}
	}
	acting = true;
}

void CreateCoalition::update()
{
	if (this->taskCoalition->isActive() || BWAPI::Broodwar->getFrameCount() - age > 500)
	{		
		cleanSubTasks();

		this->complete = true;

		g_Tasks.remove(this);		
	}
}