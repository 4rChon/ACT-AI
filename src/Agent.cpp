#include "Agent.h"
#include "GlobalVariables.h"
#include <iterator>
#include <chrono>

Agent::Agent()
{
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);
}

Agent::Agent(BWAPI::Unit unit, double freewill)
{	
	this->freeAgent = true;
	this->unitID = unit->getID();
	this->coalitionID = -1;
	this->unit = unit;
	this->freewill = freewill;	
	this->commandType = BWAPI::UnitCommandTypes::None;

	Agent();
}

void Agent::setUnit(BWAPI::Unit unit)
{
	this->unit = unit;
}

void Agent::setFree(bool free)
{
	this->freeAgent = free;
}

bool Agent::isFree() const
{
	return this->freeAgent;
}

int Agent::getID() const
{
	return this->unitID;
}

BWAPI::Unit Agent::getUnit() const
{
	return this->unit;
}

void Agent::act()
{	
}

void Agent::updateUnlocked()
{
}

void Agent::updateCommandWeights()
{
	for (auto command : commandMap)
		commandWeights.push_back(command.second);
}

void Agent::chooseCommand()
{	
	if (commandWeights.size() > 0)
	{
		std::size_t i = 0;
		std::discrete_distribution<> dist(commandWeights.size(), 0.0, 1.0, [this, &i](double){return this->commandWeights[i++]; });

		auto it = commandMap.begin();
		int number = dist(generator);
		advance(it, number);
		this->commandType = (*it).first;
	}
	else
		this->commandType = BWAPI::UnitCommandTypes::None;	
}