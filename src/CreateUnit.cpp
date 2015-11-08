#include "CreateUnit.h"
#include "CreateCoalition.h"
#include "GlobalSets.h"
#include "SatisfyRequirement.h"
#include "IncreaseIncome.h"

CreateUnit::CreateUnit(BWAPI::UnitType unitType, int unitCount)
{
	this->taskType = CRU;
	this->taskName = "CreateUnit(unitType)";
	this->unitType = unitType;
	this->unitCount = unitCount;
}

// assign a producer coalition
void CreateUnit::assign()
{	
	if (!this->assigned)
	{
		std::cout << "CreateUnit: Assign - " << this->unitType.whatBuilds().first.c_str() << "\n";
		Composition producer;
		producer.addType(this->unitType.whatBuilds().first, this->unitType.whatBuilds().second);
		CreateCoalition* createCoalition = new CreateCoalition(producer, this);
		this->addSubTask(createCoalition);
		this->assigned = true;
	}
}

// produce a unit
void CreateUnit::act()
{
	if (!this->acting)
	{	
		while (this->unitCount > 0)
		{			
			if (!this->coalition->getUnitSet().train(this->unitType))
				return;
			else this->unitCount--;
		}
		this->acting = true;
	}
}

void CreateUnit::update()
{
	double abandonChance = (((double)rand() / RAND_MAX) * this->getCost() + (BWAPI::Broodwar->getFrameCount() - this->age));
	if (abandonChance <= 100000)
		if (this->coalition->isActive())
			act();
	if (!this->complete && (this->acting || abandonChance > 100000))
	{
		std::cout << "CreateUnit: Complete\n";
		this->complete = true;		
		this->cleanSubTasks();
		this->coalition->disband();
		g_Tasks.remove(this);
	}
}