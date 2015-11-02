#include "CreateUnit.h"
#include "CreateCoalition.h"
#include "Globals.h"
#include "SatisfyRequirement.h"

CreateUnit::CreateUnit(BWAPI::UnitType unitType)
{
	this->taskType = CRU;
	this->taskName = "CreateUnit(unitType)";
	this->unitType = unitType;
}

void CreateUnit::assign()
{
	if (!this->assigned)
	{
		std::cout << "Assigning Create " << unitType.c_str() << "\n";
		if (!g_isUnlocked[unitType])
		{
			std::cout << unitType.c_str() << " unavailable\n";
			SatisfyRequirement* satisfyRequirement = new SatisfyRequirement(unitType);
			this->subTasks.push_back(satisfyRequirement);
		}

		if (unitType.supplyRequired() > BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed())
		{
			std::cout << "You must construct additional pylons\n";
			CreateUnit* createUnit = new CreateUnit(BWAPI::Broodwar->self()->getRace().getSupplyProvider());
			this->subTasks.push_back(createUnit);
		}

		Composition producer;
		producer.addType(unitType.whatBuilds().first, unitType.whatBuilds().second);
		std::cout << "Composition created: \n";
		for (auto unit : producer.getUnitMap())
			std::cout << "\t" << unit.first.c_str() << " : " << unit.second << "\n";
		
		CreateCoalition* createCoalition = new CreateCoalition(producer, this);
		this->subTasks.push_back(createCoalition);
		this->assigned = true;
	}
}

void CreateUnit::act()
{
	if (!this->acting && this->assigned)
	{
		this->acting = true;
		if (unitType.mineralPrice() > BWAPI::Broodwar->self()->minerals() && unitType != BWAPI::Broodwar->self()->getRace().getWorker())
		{
			this->complete = true;
			/*CreateUnit* createUnit = new CreateUnit(BWAPI::Broodwar->self()->getRace().getWorker());
			subTasks.push_back(createUnit);*/
			return;
		}
		if (this->unitType.whatBuilds().first.isBuilding())
		{
			this->coalition->getUnitSet().train(unitType);
			this->complete = true;
		}
		else if (this->unitType.whatBuilds().first == BWAPI::UnitTypes::Zerg_Larva)
		{
			this->coalition->getUnitSet().morph(unitType);
			this->complete = true;
		}
		else if (this->unitType.isBuilding())
		{
			for (auto builder : this->coalition->getUnitSet())
			{
				BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(unitType, builder->getTilePosition());
				if (targetBuildLocation)
				{
					/*if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
					{
						builder->move(BWAPI::Position(targetBuildLocation.x, targetBuildLocation.y));
						if (builder->morph(true))
						{
							this->acting = true;
							this->complete = true;
						}
					}*/
					if (builder->build(unitType, targetBuildLocation))
					{
						this->complete = true;
					}
					// Register an event that draws the target build location
					BWAPI::Broodwar->registerEvent([this, targetBuildLocation](BWAPI::Game*)
					{
						BWAPI::Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation),
							BWAPI::Position(targetBuildLocation + this->unitType.tileSize()),
							BWAPI::Colors::Blue);
					},
						nullptr,  // condition
						this->unitType.buildTime() + 100);  // frames to run
				}
			}
		}
		else
		{
			std::cout << "I don't know what to do\n";
			std::cout << "\t" << this->unitType.c_str() << "\n";
			std::cout << "\t" << this->unitType.whatBuilds().first.c_str() << "\n";
		}
	}		
}

void CreateUnit::update()
{
	if (this->coalition->isActive() && !this->complete)
	{
		act();

		cleanSubTasks();		

		g_Tasks.remove(this);
	}

	if (this->complete)
		coalition->disband();
}