#include "Coalition.h"
#include "GlobalSets.h"
#include "AgentManager.h"

Coalition::Coalition()
{
	active = false;
	age = BWAPI::Broodwar->getFrameCount();
}

Coalition::Coalition(Composition targetComp, int taskID, TaskType taskType)
{
	this->taskID = taskID;
	this->coalitionID = g_NextCoalitionID++;
	active = false;
	this->targetComp = targetComp;
	this->currentTask = taskType;
	age = BWAPI::Broodwar->getFrameCount();
}

Coalition::~Coalition()
{
	this->active = false;
	for (auto agent : agentSet)
		AgentManager::getInstance()->freeAgent(agent);

	this->agentSet.clear();
	this->unitSet.clear();
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

int Coalition::getID() const
{
	return this->coalitionID;
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

int Coalition::getAge() const
{
	return BWAPI::Broodwar->getFrameCount() - age;
}

void Coalition::addUnit(BWAPI::Unit unit)
{
	this->unitSet.insert(this->unitSet.begin(), unit);
	this->currentComp += unit;

	if (!active && currentComp == targetComp)
	{
		std::cout << "A coalition has been activated!\n";
		active = true;
		this->updateFreeAgents();
	}
}

void Coalition::addAgent(Agent* agent)
{
	if (this->agentSet.find(agent) == agentSet.end())
	{
		for (auto type : this->targetComp.getTypes())
		{
			if (agent->getUnit()->getType() == type)
			{
				std::cout << agent->getUnit()->getType().c_str() << " is joining a coalition\n";
				this->agentSet.insert(this->agentSet.begin(), agent);
				this->addUnit(agent->getUnit());

				return;
			}
		}
	}
}

void Coalition::removeUnit(BWAPI::Unit unit)
{
	this->unitSet.erase(unit);
	this->currentComp -= unit;
	std::cout << "A " << unit->getType() << " has left a coalition\n";
}

void Coalition::removeAgent(Agent* agent)
{
	this->agentSet.erase(agent);
	removeUnit(agent->getUnit());
	if (active)
		AgentManager::getInstance()->freeAgent(agent);
}

void Coalition::updateFreeAgents()
{
	for (auto agent : this->agentSet)
		AgentManager::getInstance()->bindAgent(agent);
}