#include "CreateUnit.h"
#include "CreateCoalition.h"
#include "Globals.h"

CreateUnit::CreateUnit(BWAPI::UnitType unitType)
{
	this->taskType = CRU;
	this->taskName = "CreateUnit(unitType)";
	this->unitType = unitType;
}

void CreateUnit::assign()
{
	if (!assigned)
	{
		if (!g_isUnlocked[unitType])
		{
			std::cout << unitType.c_str() << " unavailable\n";
			//	subTasks.push_back(SatisfyRequirement(unitType));
		}

		if (unitType.supplyRequired() > BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed())
		{
			std::cout << "You must construct additional pylons\n";
			//subTasks.push_back(CreateUnit(supplyProvider));
		}

		Composition producer;
		producer.addType(unitType.whatBuilds().first, unitType.whatBuilds().second);
		std::cout << "Composition created: \n";
		for (auto unit : producer.getUnitMap())
			std::cout << "\t" << unit.first.c_str() << " : " << unit.second << "\n";
		
		CreateCoalition* createCoalition = new CreateCoalition(producer, this);
		subTasks.push_back(createCoalition);
		g_Tasks.push_back(createCoalition);
		assigned = true;
	}
}

void CreateUnit::act()
{
	if (this->coalition->isActive())
		if (!this->acting && this->assigned)
		{
			std::cout << "Creating Unit\n";
			if (unitType.isBuilding())
				this->coalition->getUnitSet().train(unitType);
			if (!unitType.isBuilding())
				this->coalition->getUnitSet().build(unitType);
			acting = true;
		}
}

void CreateUnit::update()
{
	if (coalition->isActive())
	{
		act();

		cleanSubTasks(subTasks);
		g_Tasks.remove(this);

		this->complete = true;		
	}
	
}