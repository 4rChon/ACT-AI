#include "Coalition.h"
#include "Globals.h"

Coalition::Coalition()
{ 
	active = false;
}

Coalition::Coalition(Composition targetComp, TaskType taskType)
{
	active = false;
	this->targetComp = targetComp;
	this->currentTask = taskType;
}

void Coalition::setUnitSet(BWAPI::Unitset unitSet)
{
	this->unitSet = unitSet;
	currentComp = Composition(unitSet);
}

void Coalition::setActive(bool active)
{
	this->active = active;
}

bool Coalition::isActive() const
{
	return this->active;
}

TaskType Coalition::getCurrentTaskType() const
{
	return this->currentTask;
}

std::string Coalition::getCurrentTaskString() const
{
	switch (this->currentTask)
	{	
	case ATT: return "ATT";
	case DEF: return "DEF";
	case SCO: return "SCO";
	case EXP: return "EXP";
	case CRC: return "CRC";
	case CRU: return "CRU";
	case RES: return "RES";
	case SAT: return "SAT";
	default: return "NON";
	}
}

BWAPI::Unitset Coalition::getUnitSet() const
{
	return this->unitSet;
}

Composition Coalition::getCurrentComp() const
{
	return this->currentComp;
}

Composition Coalition::getTargetComp() const
{
	return this->targetComp;
}

void Coalition::addUnit(BWAPI::Unit unit)
{
	this->unitSet.insert(unitSet.begin(), unit);
	this->currentComp += unit;
	std::cout << "A " << unit->getType() << " has joined a coalition\n";
	if (!active)
	{
		if (currentComp == targetComp)
		{
			std::cout << "A coalition has been activated!\n";
			g_OpenCoalitions.erase(this);
			active = true;
		}
	}
}

void Coalition::removeUnit(BWAPI::Unit unit)
{
	this->unitSet.erase(unit);
	this->currentComp -= unit;
	std::cout << "A " << unit->getType() << " has left a coalition\n";
}