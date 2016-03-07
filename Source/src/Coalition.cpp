#include "Coalition.h"
#include "AgentHelper.h"
#include "CoalitionHelper.h"
#include "Task.h"
#include "UtilHelper.h"
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
	engageDuration = 0;
	cost = 0.0;
	profit = 0.0;
	killCount = 0;
}

Coalition::Coalition(Composition targetComp, Task* task)
{
	creationFrame = BWAPI::Broodwar->getFrameCount();
	this->task = task;
	taskID = task->getID();
	active = false;

	this->targetComp = targetComp;

	coalitionID = CoalitionHelper::getNextID();
	engageDuration = 0;
	killCount = 0;
	cost = 0;
	profit = 0.0;	
}

Coalition::~Coalition()
{
	std::cout << "~Coalition : " << coalitionID << "\n";
	logPerformance();
	active = false;	
	for (auto agent : agentSet)
		agent->unbind();
	agentSet.clear();
	unitSet.clear();
}

Task* Coalition::getTask() const
{
	return task;
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
	std::vector<double> valueArr;
	std::vector<double> coeffArr;
	valueArr.push_back(currentComp.getCost());
	coeffArr.push_back(0.0001);
	valueArr.push_back(BWAPI::Broodwar->getFrameCount() - creationFrame);
	coeffArr.push_back(0.00001);
	cost = normaliseValues(valueArr, coeffArr);
	return cost;
}

double Coalition::getProfit()
{
	std::vector<double> valueArr;
	std::vector<double> coeffArr;
	valueArr.push_back(killCount);
	coeffArr.push_back(1.0);
	valueArr.push_back(engageDuration);
	coeffArr.push_back(0.8);
	profit = normaliseValues(valueArr, coeffArr);
	return profit;
}

void Coalition::addEngagement()
{
	engageDuration++;
}

void Coalition::addKillCount(int killCount)
{
	this->killCount += killCount;
}

Composition Coalition::getCurrentComp() const
{
	return currentComp;
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
	//composition.open("compositions\\" + BWAPI::Broodwar->mapName() + "\\" + BWAPI::Broodwar->enemy()->getRace().getName() + "\\" + race + "_" + task->getName() + ".txt");
	composition.open("compositions\\" + BWAPI::Broodwar->enemy()->getRace().getName() + "\\" + race + "_" + task->getName() + ".txt");
	composition << "Map Name: " << BWAPI::Broodwar->mapName() << " : " << BWAPI::Broodwar->mapFileName() << "\n";
	composition << "EngageDuration: " << engageDuration << "\n";
	composition << "KillCount: " << killCount << "\n";
	composition << profit << " | " << cost << "\n";
	composition << targetComp.toString();
	composition.close();
}