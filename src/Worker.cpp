#include "Worker.h"
#include <random>
#include <iterator>
#include <chrono>
Worker::Worker(BWAPI::Unit unit, double freewill)
{
	this->buildTotalChance = 0.0;
	this->unit = unit;
	this->freewill = freewill;
	this->commandMap[BWAPI::UnitCommandTypes::Attack_Unit] = 0.0;
	this->commandMap[BWAPI::UnitCommandTypes::Repair] = 0.0;
	this->commandMap[BWAPI::UnitCommandTypes::Gather] = 1.0;
	this->commandMap[BWAPI::UnitCommandTypes::Build] = 0.10;

	updateUnlocked();
	updateCommandWeights();
	updateBuildWeights();

	this->commandType = BWAPI::UnitCommandTypes::None;
}

void Worker::act()
{	
	//this->commandMap[BWAPI::UnitCommandTypes::Build] = g_Supply *(int)(BWAPI::Broodwar->self()->getRace().getSupplyProvider().mineralPrice() < BWAPI::Broodwar->self()->minerals());
	//this->commandMap[BWAPI::UnitCommandTypes::Gather] = 1 - this->commandMap[BWAPI::UnitCommandTypes::Build];
	//this->buildMap[BWAPI::Broodwar->self()->getRace().getSupplyProvider()] = g_Supply *(int)(BWAPI::Broodwar->self()->getRace().getSupplyProvider().mineralPrice() < BWAPI::Broodwar->self()->minerals());

	chooseCommand();
	if (this->commandType == BWAPI::UnitCommandTypes::Build)
		if (build())
			return;

	if (this->commandType == unit->getLastCommand().getType())
		return;

	if (this->commandType == BWAPI::UnitCommandTypes::Attack_Unit)
		if (attack())
			return;
	
	if (this->commandType == BWAPI::UnitCommandTypes::Repair)
		if (repair())
			return;
	
	if (this->commandType == BWAPI::UnitCommandTypes::Gather)
		if (gather())
			return;

	this->commandType = BWAPI::UnitCommandTypes::None;
}

bool Worker::attack()
{
	return this->unit->attack(this->unit->getClosestUnit(BWAPI::Filter::IsEnemy));
}

bool Worker::repair()
{
	return this->unit->repair(this->unit->getClosestUnit(BWAPI::Filter::IsOwned && BWAPI::Filter::IsBuilding && (BWAPI::Filter::HP_Percent < 100)));
}

bool Worker::gather()
{	
	return this->unit->gather(this->unit->getClosestUnit(BWAPI::Filter::IsMineralField || (BWAPI::Filter::IsOwned && BWAPI::Filter::IsResourceContainer)));
}

bool Worker::build()
{
	if ((double)rand() / RAND_MAX > freewill)
		return false;
	chooseBuilding();
	if (this->buildType == BWAPI::UnitTypes::None)
		return false;
	//std::cout << buildTotalChance << "\n";
	if (buildType == BWAPI::Broodwar->self()->getRace().getSupplyProvider())
		g_Supply *= 0.25;
	
	BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(this->buildType, this->unit->getTilePosition());

	if (targetBuildLocation)
	{
		if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
		{
			this->unit->move(BWAPI::Position(targetBuildLocation.x, targetBuildLocation.y));
			return this->unit->morph(true);
		}
		
		return this->unit->build(this->buildType, targetBuildLocation);
	}
	else 
		return false;
}

void Worker::updateUnlocked()
{
	for (auto unitType : this->unit->getType().buildsWhat())
	{
		if (!g_isUnlocked[unitType])
			this->buildMap[unitType] = 0.0;
		else
		{
			if (unitType == BWAPI::UnitTypes::Terran_Barracks || unitType == BWAPI::UnitTypes::Terran_Supply_Depot)
				this->buildMap[unitType] = 1.0 / (double)this->unit->getType().buildsWhat().size();
		}
		//std::cout << unitType.c_str() << " : " << this->produceMap[unitType] << "\n";
	}
}

void Worker::updateBuildWeights()
{
	//set production weights and choose production type
	this->buildType = BWAPI::UnitTypes::None;
	this->buildTotalChance = 0.0;
	buildMap[BWAPI::Broodwar->self()->getRace().getSupplyProvider()] = g_Supply;
	if (this->commandType == BWAPI::UnitCommandTypes::Build)
	{
		buildWeights.clear();
		for (auto build : buildMap)
		{
			if (build.first.mineralPrice() > BWAPI::Broodwar->self()->minerals() - g_MinReserve
				|| build.first.gasPrice() > BWAPI::Broodwar->self()->gas() - g_GasReserve)
			{
				buildWeights.push_back(0.0);
			}
			else
			{	
				if (build.first == BWAPI::Broodwar->self()->getRace().getSupplyProvider())
				{
					buildTotalChance += build.second;
					buildWeights.push_back(build.second);
				}
				else if (build.second - g_Supply > 0)
				{
					buildTotalChance += build.second - g_Supply;
					buildWeights.push_back(build.second - g_Supply);
				}
				else
				{
					buildWeights.push_back(0);
				}
			}
		}
	}	
}

void Worker::chooseBuilding()
{
	updateBuildWeights();
	if (buildWeights.size() > 0 && buildTotalChance > 0.0)
	{
		std::size_t i = 0;
		std::discrete_distribution<> dist(buildWeights.size(), 0.0, 1.0, [this, &i](double){return this->buildWeights[i++]; });

		auto it = buildMap.begin();
		int number = dist(generator);
		advance(it, number);
		//std::cout << "\t" << (*it).first.c_str() << "\n";
		this->buildType = (*it).first;
		//std::cout << this->buildType.c_str() << "\n";
	}
	else
		this->buildType = BWAPI::UnitTypes::None;
}

