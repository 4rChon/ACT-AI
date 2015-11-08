#include "Agent.h"
#include "GlobalVariables.h"
#include <random>

Agent::Agent(BWAPI::Unit unit)
{
	this->unit = unit;
	if (this->unit->getType().isWorker())
	{
		this->commandMap[BWAPI::UnitCommandTypes::Attack_Unit] = 0;
		this->commandMap[BWAPI::UnitCommandTypes::Repair] = 0;
		this->commandMap[BWAPI::UnitCommandTypes::Gather] = 1;
		this->commandMap[BWAPI::UnitCommandTypes::Build] = 0;
	
		for (auto unitType : this->unit->getType().buildsWhat())
			this->produceMap[unitType] = 0;
	}

	if (this->unit->getType().canProduce() && this->unit->getType().isBuilding())
	{
		commandMap[BWAPI::UnitCommandTypes::Train] = 1;
		for (auto unitType : this->unit->getType().buildsWhat())
			this->produceMap[unitType] = 0;

	}

	if (this->unit->getType().isResourceDepot())
	{
		this->produceMap[BWAPI::Broodwar->self()->getRace().getWorker()] = 1;
	}
	this->commandType = BWAPI::UnitCommandTypes::None;
}

void Agent::setUnit(BWAPI::Unit unit)
{
	this->unit = unit;
}

BWAPI::Unit Agent::getUnit() const
{
	return this->unit;
}

void Agent::act()
{

	if (this->unit->getType().isWorker())
	{
		this->commandMap[BWAPI::UnitCommandTypes::Build] = g_Supply * (int)(BWAPI::Broodwar->self()->getRace().getSupplyProvider().mineralPrice() < BWAPI::Broodwar->self()->minerals());
		this->commandMap[BWAPI::UnitCommandTypes::Gather] = 1 - this->commandMap[BWAPI::UnitCommandTypes::Build];
		this->produceMap[BWAPI::Broodwar->self()->getRace().getSupplyProvider()] = g_Supply;
	}	

	for (auto command : this->commandMap)
	{
		if (((double)rand() / RAND_MAX) < command.second)
		{
			this->commandType = command.first;
			/*BWAPI::Broodwar->registerEvent([this](BWAPI::Game*)
			{
				BWAPI::Broodwar->drawTextMap(this->getUnit()->getPosition(), commandType.c_str());
			},
				[this](BWAPI::Game*)
			{
				return !this->getUnit()->getLastCommand().getType() == this->commandType;
			});*/
			break;
		}
	}

	for (auto produce : this->produceMap)
	{
		if (((double)rand() / RAND_MAX) < produce.second)
		{
			this->produceType = produce.first;
			break;
		}
	}	

	bool hasCommand = false;

	if (this->commandType == BWAPI::UnitCommandTypes::Build)
	{				
		BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(this->produceType, this->unit->getTilePosition());
		if (targetBuildLocation)
		{
			if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
			{
				this->unit->move(BWAPI::Position(targetBuildLocation.x, targetBuildLocation.y));
				hasCommand = this->unit->morph(true);
			}
			else
				hasCommand = this->unit->build(this->produceType, targetBuildLocation);
			// Register an event that draws the target build location
			if (hasCommand)
			{
				BWAPI::Broodwar->registerEvent([this, targetBuildLocation](BWAPI::Game*)
				{
					BWAPI::Broodwar->drawBoxMap(BWAPI::Position(targetBuildLocation),
						BWAPI::Position(targetBuildLocation + this->produceType.tileSize()),
						BWAPI::Colors::Blue);
					BWAPI::Broodwar->drawDotMap(this->getUnit()->getPosition(), BWAPI::Colors::Blue);
					BWAPI::Broodwar->drawLineMap(this->getUnit()->getPosition(), BWAPI::Position(targetBuildLocation), BWAPI::Colors::Blue);
				},
					nullptr,  // condition
					this->produceType.buildTime() + 100);  // frames to run
				return;
			}
		}
	}
	else if (this->commandType == BWAPI::UnitCommandTypes::Train)
	{
		if (this->unit->getTrainingQueue().size() < 1)
			if(this->unit->train(produceType))
				return;
	}

	if (this->commandType == unit->getLastCommand().getType())
		return;	

	if (this->commandType == BWAPI::UnitCommandTypes::Attack_Unit)
	{
		if(this->unit->attack(this->unit->getClosestUnit(BWAPI::Filter::IsEnemy)))
			return;
	}
	if (this->commandType == BWAPI::UnitCommandTypes::Repair)
	{
		if(this->unit->repair(this->unit->getClosestUnit(BWAPI::Filter::IsOwned && BWAPI::Filter::IsBuilding && (BWAPI::Filter::HP_Percent < 100))))
			return;
	}
	if (this->commandType == BWAPI::UnitCommandTypes::Gather)
	{
		if(this->unit->gather(this->unit->getClosestUnit(BWAPI::Filter::IsResourceContainer)))
			return;
	}

	this->commandType = BWAPI::UnitCommandTypes::None;
}