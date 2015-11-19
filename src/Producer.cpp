#include "Producer.h"
#include <random>
#include <iterator>
#include <chrono>
Producer::Producer(BWAPI::Unit unit, double freewill)
{	
	trainTotalChance = 0.0;
	this->unit = unit;
	this->freewill = freewill;
	commandMap[BWAPI::UnitCommandTypes::Train] = 1.0;	
	
	updateUnlocked();
	updateCommandWeights();
	updateTrainWeights();

	this->commandType = BWAPI::UnitCommandTypes::None;
}

void Producer::act()
{			
	if (this->commandType == BWAPI::UnitCommandTypes::Train)
	{
		chooseUnit();
		if (train())
			return;
	}

	this->commandType = BWAPI::UnitCommandTypes::None;
	chooseCommand();
}

bool Producer::train()
{
	if (this->trainType == BWAPI::UnitTypes::None)
		return false;

	if (this->unit->getTrainingQueue().size() < 1)
		return (this->unit->train(trainType));
	else
		return false;
}

void Producer::updateUnlocked()
{		
	for (auto unitType : this->unit->getType().buildsWhat())
	{
		if (!g_isUnlocked[unitType])
		{
			this->trainMap[unitType] = 0.0;
		}
		else
			this->trainMap[unitType] = 1.0 / (double)this->unit->getType().buildsWhat().size();
	}
}

void Producer::updateTrainWeights()
{
	this->trainType = BWAPI::UnitTypes::None;
	this->trainTotalChance = 0.0;
	this->trainWeights.clear();
	for (auto train : trainMap)
	{
		double currentMinerals = BWAPI::Broodwar->self()->minerals() - g_MinReserve;
		double currentGas = BWAPI::Broodwar->self()->gas() - g_GasReserve;
		if (train.first.mineralPrice() > currentMinerals || train.first.gasPrice() > currentGas)
			this->trainWeights.push_back(0.0);
		else
		{
			this->trainTotalChance += train.second;
			this->trainWeights.push_back(train.second);
		}
	}
}

void Producer::chooseUnit()
{	
	updateTrainWeights();
	if (trainWeights.size() > 0 && trainTotalChance > 0.0)
	{		
		std::size_t i = 0;
		std::discrete_distribution<> dist(trainWeights.size(), 0.0, 1.0, [this, &i](double){return this->trainWeights[i++]; });

		auto it = trainMap.begin();
		int number = dist(generator);
		advance(it, number);
		this->trainType = (*it).first;
	}
	else
		this->trainType = BWAPI::UnitTypes::None;	
}