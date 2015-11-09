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
			if (!(this->unitType.mineralPrice() <= BWAPI::Broodwar->self()->minerals() && this->unitType.gasPrice() <= BWAPI::Broodwar->self()->gas() && this->unitType.supplyRequired() <= BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed()))
				return;
			if (this->unitType.isBuilding() && this->unitType.whatBuilds().first == BWAPI::Broodwar->self()->getRace().getWorker())
			{
				std::cout << "I have enough resources to build a " << this->unitType.c_str() << "\n";
				for (auto builder : this->coalition->getUnitSet())
				{
					BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(this->unitType, builder->getTilePosition());
					if (targetBuildLocation)
					{
						if (!builder->move(BWAPI::Position(targetBuildLocation.x * BWAPI::TILEPOSITION_SCALE, targetBuildLocation.y * BWAPI::TILEPOSITION_SCALE))) return;
						if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
						{							
							if (!builder->morph(true)) return;
							this->unitCount--;
						}
						else
						{
							if (!builder->build(this->unitType, targetBuildLocation)) return;
							std::cout << "I found a suitable location to build a " << this->unitType.c_str() << "\n";							
							this->unitCount--;
						}

						// Register an event that draws the target build location
						BWAPI::Broodwar->registerEvent([this, targetBuildLocation, builder](BWAPI::Game*)
						{
							BWAPI::Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation),
								BWAPI::Position(targetBuildLocation + this->unitType.tileSize()),
								BWAPI::Colors::Red);
							BWAPI::Broodwar->drawDotMap(builder->getPosition(), BWAPI::Colors::Red);
							BWAPI::Broodwar->drawLineMap(builder->getPosition(), BWAPI::Position(targetBuildLocation), BWAPI::Colors::Red);
						},
							nullptr,  // condition
							this->unitType.buildTime() + 100);  // frames to run
					}
				}
			}
			else
			{
				for (auto producer : this->coalition->getUnitSet())
				{
					if (producer->getTrainingQueue().size() < 1)
					{
						if (this->coalition->getUnitSet().train(this->unitType)) this->unitCount--;
					}
					else
						return;
				}
			}			
		}
		this->acting = true;
	}
}

void CreateUnit::update()
{
	//double abandonChance = (((double)rand() / RAND_MAX) * this->getCost() + (BWAPI::Broodwar->getFrameCount() - this->age));
	//if (abandonChance <= 100000)
		if (this->coalition->isActive())
			act();
	if (!this->complete && (this->acting))// || abandonChance > 100000))
	{
		std::cout << "CreateUnit: Complete\n";
		this->complete = true;		
		this->cleanSubTasks();
		this->coalition->disband();
		g_Tasks.remove(this);
	}
}