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
	this->satisfied = false;
	this->building = false;
	this->reserved = false;
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
			if (!g_isUnlocked[unitType])
			{
				if (satisfied)
					return;
				std::cout << unitType.c_str() << " is locked\n";
				SatisfyRequirement* satisfyRequirement = new SatisfyRequirement(unitType);
				this->addSubTask(satisfyRequirement);
				satisfied = true;
				return;
			}
			if (!(this->unitType.mineralPrice() <= (BWAPI::Broodwar->self()->minerals() - g_MinReserve) && this->unitType.gasPrice() <= (BWAPI::Broodwar->self()->gas() - g_GasReserve) && this->unitType.supplyRequired() <= BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed())) 
				return;
			if (this->unitType.isAddon())
			{
				if (!this->coalition->getUnitSet().buildAddon(this->unitType)) 
					return;
				this->unitCount--;
			}
			if (this->unitType.isBuilding() && this->unitType.whatBuilds().first == BWAPI::Broodwar->self()->getRace().getWorker())
			{				
				if (this->building)
					return;				
				
				if (!reserved)
				{
					std::cout << "I have enough resources to build a " << this->unitType.c_str() << "\n";
					std::cout << "Reserving " << this->unitType.mineralPrice() << " Minerals and " << this->unitType.gasPrice() << " Gas to build a " << this->unitType.c_str() << "\n";
					g_MinReserve += this->unitType.mineralPrice();
					g_GasReserve += this->unitType.gasPrice();
					reserved = true;
				}

				for (auto builder : this->coalition->getUnitSet())
				{					
					std::cout << "Selecting Builder\n";
					BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(this->unitType, builder->getTilePosition());
					if (targetBuildLocation)
					{
						std::cout << "I found a suitable location to build a " << this->unitType.c_str() << "\n";
						if (!builder->move(BWAPI::Position(targetBuildLocation.x * BWAPI::TILEPOSITION_SCALE, targetBuildLocation.y * BWAPI::TILEPOSITION_SCALE))) 
							return;
						if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
						{							
							if (!builder->morph(true)) return;
							this->unitCount--;
						}
						else
						{
							this->building = builder->build(this->unitType, targetBuildLocation);
							if (!this->building) return;
							std::cout << "Moving to build a " << this->unitType.c_str() << "\n";							
							int min = this->unitType.mineralPrice();
							int gas = this->unitType.gasPrice();
							auto uType = this->unitType;							
							BWAPI::Broodwar->registerEvent([builder, min, gas, uType](BWAPI::Game*)
							{
								std::cout << "Releasing " << min << " Minerals and " << gas << " Gas after placing a " << uType.c_str() << "\n";
								g_MinReserve -= min;
								g_GasReserve -= gas;								
							},
								[builder](BWAPI::Game*){return builder->getOrder() == BWAPI::Orders::ConstructingBuilding; },
								1);

							this->unitCount--;
						}
					}
				}
			}
			else
			{
				if (this->coalition->getUnitSet().size() == 0)
					return;

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
	if (this->complete)
	{
		this->cleanSubTasks();
		return;
	}
	
	if (this->coalition->isActive())
		act();
	
	if (this->acting)// || abandonChance > 100000))
	{
		this->complete = true;		
		std::cout << "CreateUnit: Complete\n";
	}	
}