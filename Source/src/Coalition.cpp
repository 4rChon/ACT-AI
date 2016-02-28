#include "Coalition.h"
#include "AgentHelper.h"
#include "CoalitionHelper.h"
#include "Task.h"
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

Coalition::Coalition()
{
	creationFrame = BWAPI::Broodwar->getFrameCount();
	task = nullptr;
	taskID = -1;
	active = false;	
		
	coalitionID = CoalitionHelper::getNextID();
	cost = 0.0;
	profit = 0.0;
}

Coalition::Coalition(Composition targetComp, Task* task)
{
	creationFrame = BWAPI::Broodwar->getFrameCount();
	this->task = task;
	taskID = task->getID();
	active = false;

	this->targetComp = targetComp;

	coalitionID = CoalitionHelper::getNextID();
	
	cost = targetComp.getCost();
	profit = 0.0;	
}

Coalition::~Coalition()
{
	std::cout << "~Coalition : " << coalitionID << "\n";
	//logPerformance();
	active = false;	
	/*std::cout << "Unbinding Agents...\n";*/
	for (auto agent : agentSet)
	{
		/*std::cout << "\tUnbinding...\n";*/
		agent->unbind();
	}
	/*std::cout << "Agents Unbound...\n";*/
	agentSet.clear();
	/*std::cout << "Agentset Clear...\n";*/
	unitSet.clear();
	/*std::cout << "Unitset Clear...\n";*/
	/*CoalitionHelper::removeCoalition(this);*/
}

int Coalition::getAge() const
{
	return BWAPI::Broodwar->getFrameCount() - creationFrame;
}

int Coalition::getID() const
{
	return coalitionID;
}

BWAPI::Unitset &Coalition::getUnitSet()
{
	return unitSet;
}

Agentset &Coalition::getAgentSet()
{
	return agentSet;
}

double Coalition::getCost()
{
	if (!active)
		cost += (BWAPI::Broodwar->getFrameCount() - creationFrame);
	return 1 - (1 / ((cost + 500) / 500)); //convert to sigma function instead : 1/(1 + e^-x)
}

double Coalition::getProfit() const
{
	return this->profit;
}

Composition Coalition::getCurrentComp() const
{
	return this->currentComp;
}

Composition Coalition::getTargetComp() const
{
	return targetComp;
}

bool Coalition::isActive() const
{
	return active;
}

bool Coalition::addAgent(Agent* agent)
{
	if (!agentSet.count(agent))
	{
		for (auto type : targetComp.getTypes())
		{
			if (agent->getUnit()->getType() == type && targetComp[type] > currentComp[type])
			{
				/*std::cout << agent->getUnit()->getType().c_str() << " is joining a coalition\n";*/
				agentSet.insert(agentSet.begin(), agent);
				addUnit(agent->getUnit());
				agent->setCoalition(this);
				agent->setTask(task);				
				return true;
			}
		}
	}
	return false;
}

void Coalition::addUnit(BWAPI::Unit unit)
{
	unitSet.insert(unitSet.begin(), unit);
	currentComp += unit->getType();

	if (!active && currentComp == targetComp)
	{
		std::cout << "Coalition " << coalitionID << " Activated!\n";
		active = true;
		for (auto &agent : agentSet)
			agent->bind();
	}
}

void Coalition::removeAgent(Agent* agent)
{
	agentSet.erase(agent);
	removeUnit(agent->getUnit());

	if (active)
	{
		agent->unbind();
		//if all agents die while coalition is activated, the task is a failure
		if(agentSet.size() == 0)
			task->fail();
	}
}

void Coalition::removeUnit(BWAPI::Unit unit)
{
	unitSet.erase(unit);
	currentComp -= unit->getType();
	std::cout << "A " << unit->getType() << " has left a coalition\n";
}

void Coalition::outAttributes()
{
	std::cout << "Coalition " << coalitionID << "\n";
	currentComp.outAttributes();
}

void Coalition::logPerformance()
{
	std::cout << "Logging Performance...\n";
	std::ofstream composition;
	auto race = BWAPI::Broodwar->self()->getRace().toString();
	composition.open("compositions\\" + race + "_" + task->getName() + ".txt");
	composition << targetComp.toString();
	composition.close();
}