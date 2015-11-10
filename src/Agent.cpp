#include "Agent.h"
#include "GlobalVariables.h"
#include <random>
#include <iterator>
#include <chrono>
Agent::Agent(BWAPI::Unit unit, double freewill)
{
	this->produceTotalChance = 0;
	this->unit = unit;
	this->freewill = freewill;
	if (this->unit->getType().isWorker())
	{
		this->commandMap[BWAPI::UnitCommandTypes::Attack_Unit] = 0.0;
		this->commandMap[BWAPI::UnitCommandTypes::Repair] = 0.0;
		this->commandMap[BWAPI::UnitCommandTypes::Gather] = 0.99;
		this->commandMap[BWAPI::UnitCommandTypes::Build] = 0.01;
	}

	if (this->unit->getType().canProduce() && this->unit->getType().isBuilding())
		commandMap[BWAPI::UnitCommandTypes::Train] = 1.0;


	for (auto unitType : this->unit->getType().buildsWhat())
	{
		if (!g_isUnlocked[unitType])
			this->produceMap[unitType] = 0;
		else
		{
			if (unitType == BWAPI::Broodwar->self()->getRace().getWorker() && this->unit->getType().isResourceDepot())
			{
				this->produceMap[unitType] = 1.0;
				this->produceTotalChance += 1.0;
			}
			else
			{
				this->produceMap[unitType] = 1.0 / (double)this->unit->getType().buildsWhat().size();
				this->produceTotalChance += this->produceMap[unitType];
			}
		}
		//std::cout << unitType.c_str() << " : " << this->produceMap[unitType] << "\n";
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

	for (auto unitType : this->unit->getType().buildsWhat())
	{
		if (!g_isUnlocked[unitType])
			this->produceMap[unitType] = 0.0001;
		else
			this->produceMap[unitType] = 1.0 / (double)this->unit->getType().buildsWhat().size();
		//std::cout << unitType.c_str() << " : " << this->produceMap[unitType] << "\n";
	}

	if (this->unit->getType().isWorker())
	{
		this->commandMap[BWAPI::UnitCommandTypes::Build] = g_Supply *(int)(BWAPI::Broodwar->self()->getRace().getSupplyProvider().mineralPrice() < BWAPI::Broodwar->self()->minerals());
		this->commandMap[BWAPI::UnitCommandTypes::Gather] = 1 - this->commandMap[BWAPI::UnitCommandTypes::Build];
		this->produceMap[BWAPI::Broodwar->self()->getRace().getSupplyProvider()] = g_Supply *(int)(BWAPI::Broodwar->self()->getRace().getSupplyProvider().mineralPrice() < BWAPI::Broodwar->self()->minerals());
	}

	std::vector<double> commandWeights;
	commandWeights.reserve(commandMap.size());
	for (auto command : commandMap)
		commandWeights.push_back(command.second);
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	if (commandWeights.size() > 0)
	{		
		std::size_t i = 0;
		std::discrete_distribution<> dist(commandWeights.size(), 0.0, 1.0, [&commandWeights, &i](double){return commandWeights[i++]; });

		auto it = commandMap.begin();
		int number = dist(generator);
		advance(it, number);
		//std::cout << (*it).first.c_str() << "\n";
		this->commandType = (*it).first;
	}
	else
		this->commandType = BWAPI::UnitCommandTypes::None;

	this->produceType = BWAPI::UnitTypes::None;
	this->produceTotalChance = 0.0;
	if (this->commandType == BWAPI::UnitCommandTypes::Build || this->commandType == BWAPI::UnitCommandTypes::Train)
	{
		std::vector<double> produceWeights;
		produceWeights.reserve(produceMap.size());
		for (auto produce : produceMap)
		{
			if (produce.first.mineralPrice() > BWAPI::Broodwar->self()->minerals() - g_MinReserve
				&& produce.first.gasPrice() > BWAPI::Broodwar->self()->gas() - g_GasReserve)
			{
				produceWeights.push_back(0.0);
				return;
			}
			produceTotalChance += produce.second;
			produceWeights.push_back(produce.second);
		}
	
		if (produceWeights.size() > 0 && produceTotalChance > 0.0)
		{
			std::size_t i = 0;
			std::discrete_distribution<> dist(produceWeights.size(), 0.0, 1.0, [&produceWeights, &i](double){return produceWeights[i++]; });

			auto it = produceMap.begin();
			int number = dist(generator);
			advance(it, number);
			//std::cout << "\t" << (*it).first.c_str() << "\n";
			this->produceType = (*it).first;
			std::cout << produceTotalChance << "\n";
		}		
	}

	bool hasCommand = false;

	if (this->commandType == BWAPI::UnitCommandTypes::Build && produceType != BWAPI::UnitTypes::None)
	{				
		std::cout << produceTotalChance << "\n";
		if (produceType == BWAPI::Broodwar->self()->getRace().getSupplyProvider())
			g_Supply *= 0.25;

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
				std::cout << "Found a place to build a " << produceType.c_str() << "\n";
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
	else if (this->commandType == BWAPI::UnitCommandTypes::Train && produceType != BWAPI::UnitTypes::None)
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

bool build()
{
	return true;
}

bool train()
{
	return true;
}