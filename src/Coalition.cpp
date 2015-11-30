#include "Coalition.h"
#include "GlobalSets.h"
#include "AgentManager.h"

Coalition::Coalition()
{
	this->active = false;
	this->age = BWAPI::Broodwar->getFrameCount();
	this->cost = 0.0;
	this->profit = 0.0;
}

Coalition::Coalition(Composition targetComp, int taskID, TaskType taskType)
{
	this->taskID = taskID;
	this->coalitionID = g_NextCoalitionID++;	
	this->targetComp = targetComp;
	this->currentTask = taskType;
	this->active = false;
	this->age = BWAPI::Broodwar->getFrameCount();		
	this->cost = targetComp.getCost();
	this->profit = 0.0;
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
	this->currentComp = Composition(unitSet);
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

Agentset Coalition::getAgentSet() const
{
	return this->agentSet;
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
			if (agent->getUnit()->getType() == type && this->targetComp[type] > this->currentComp[type])
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

double Coalition::getCost()
{
	if (!active)
	{
		this->cost += (BWAPI::Broodwar->getFrameCount() - age) / 24.0;
		this->age = BWAPI::Broodwar->getFrameCount();
	}
	return 1 - (1 / ((this->cost + 500) / 500));
}

double Coalition::getProfit()
{
	return this->profit;
}