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

//std::unordered_set<Agent*> Coalition::getAgentSet() const
//{
//	return this->agentSet;
//}

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
	//std::cout << "A " << unit->getType() << " has joined a coalition\n";
	if (!active)
	{
		if (currentComp == targetComp)
		{
			//std::cout << "A coalition has been activated!\n";
			/*for (auto agent : agentSet)
				g_FreeAgents.erase(agent);*/
			
			//g_OpenCoalitions.erase(this);
			active = true;
		}
	}
}

void Coalition::addAgent(Agent* agent)
{
	if (agentSet.find(agent) == agentSet.end())
	{
		for (auto type : targetComp.getTypes())
			if (agent->getUnit()->getType() == type)
			{
				std::cout << agent->getUnit()->getType().c_str() << " is joining a coalition\n";
				agentSet.insert(agentSet.begin(), agent);
				addUnit(agent->getUnit());
				return;
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
	g_FreeAgents.insert(agent);
	removeUnit(agent->getUnit());
}

void Coalition::updateFreeAgents()
{
	if (active)
		for (auto agent : this->agentSet)
			g_FreeAgents.erase(agent);
}

void Coalition::disband()
{
	g_FreeAgents.insert(this->agentSet.begin(), this->agentSet.end());
	this->agentSet.clear();
	this->unitSet.clear();
	g_Coalitions.erase(this);
}