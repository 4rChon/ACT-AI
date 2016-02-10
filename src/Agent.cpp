#include "..\include\Agent.h"

Agent::Agent()
{
	this->unit = nullptr;
	this->unitID = -1;
	this->coalitionID = -1;
	this->taskID = -1;
}

Agent::Agent(BWAPI::Unit unit)
{
	this->unit = unit;
	this->unitID = unit->getID();
	this->coalitionID = -1;
	this->taskID = -1;
}

Agent::~Agent()
{
	std::cout << "~Agent\n";
}

void Agent::setCoalitionID(int id)
{
	coalitionID = id;
}

void Agent::setTaskID(int id)
{
	taskID = id;
}

void Agent::setUnit(BWAPI::Unit unit)
{
	this->unit = unit;
}

int Agent::getID() const
{
	return unitID;
}

int Agent::getCoalitionID() const
{
	return coalitionID;
}

int Agent::getTaskID() const
{
	return taskID;
}

BWAPI::Unit Agent::getUnit() const
{
	return unit;
}

double Agent::getPrice() const
{
	return unit->getType().mineralPrice() + (unit->getType().gasPrice() * 1.5);
}

void Agent::act()
{
	//do stuff
}

void Agent::debugInfo()
{
	std::cout << "\tID  : " << unitID << "\n";
	std::cout << "\tUnit: " << unit->getType() << "\n";
}