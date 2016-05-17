#include "Coalition.h"
#include "AgentHelper.h"
#include "CoalitionHelper.h"
#include "EconHelper.h"
#include "Task.h"
#include "UtilHelper.h"
#include "CompositionHelper.h"
#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <istream>
#include <fstream>

Coalition::Coalition()
	: creationFrame(BWAPI::Broodwar->getFrameCount())
	, activationFrame(-1)
	, task(nullptr)
	, taskID(-1)
	, active(false)
	, focusFire(true)
	, coalitionID(CoalitionHelper::getNextID())
	, engageDuration(0)
	, cost(0.0)
	, profit(0.0)
	, killCount(0)
	, unitMultiplier(1)
{
}

Coalition::Coalition(Composition targetComp, Task* task)
	: creationFrame(BWAPI::Broodwar->getFrameCount())
	, activationFrame(-1)
	, task(task)
	, taskID(task->getID())
	, targetComp(targetComp)
	, active(false)
	, focusFire(true)
	, coalitionID(CoalitionHelper::getNextID())
	, engageDuration(0)
	, cost(0.0)
	, profit(0.0)
	, killCount(0)
{
	if (task->getType() == ATT)
		unitMultiplier = EconHelper::getUnitMultiplier(targetComp);
	else
		unitMultiplier = 1;
	
}

Coalition::~Coalition()
{
	/*std::cout << "~Coalition : " << coalitionID << "\n";*/

	CompositionHelper::saveComposition(this);

	for (auto agent : agentSet)
		agent->unbind();
}

Task* Coalition::getTask() const
{
	return task;
}

int Coalition::getAge() const
{
	if(!active)
		return BWAPI::Broodwar->getFrameCount() - creationFrame;
	return BWAPI::Broodwar->getFrameCount() - activationFrame;
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
	valueArr.push_back(getAge());
	coeffArr.push_back(0.00001);
	cost = util::normaliseValues(valueArr, coeffArr);
	return cost;
}

double Coalition::getProfit()
{
	std::vector<double> valueArr;
	std::vector<double> coeffArr;
	valueArr.push_back(killCount);
	coeffArr.push_back(0.9);
	valueArr.push_back(engageDuration);
	coeffArr.push_back(0.4);
	profit = util::normaliseValues(valueArr, coeffArr);
	return profit;
}

double Coalition::getFitness()
{
	std::vector<double> valueArr;
	std::vector<double> coeffArr;
	valueArr.push_back(profit/cost);
	coeffArr.push_back(0.1);
	return util::normaliseValues(valueArr, coeffArr);
}

double Coalition::getUnitMultiplier()
{
	return unitMultiplier;
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

void Coalition::activate()
{
	active = true;
	targetComp = currentComp;
	for (auto &agent : agentSet)
		agent->bind();
}

bool Coalition::addAgent(Agent* agent)
{	
	if (agent->getCoalitionID() == this->coalitionID)
		return false;

	/*if (agent->getCoalitionID() != -1)
		agent->getCoalition()->removeAgent(agent);*/

	auto unitType = agent->getUnit()->getType();

	if (targetComp[unitType] > currentComp[unitType])
	{
		/*std::cout << agent->getUnit()->getType().c_str() << " is joining a coalition\n";*/
		agentSet.insert(agent);
		addUnit(agent->getUnit());
		agent->setCoalition(this);
		agent->setTask(task);				
		return true;
	}

	return false;
}

void Coalition::addUnit(BWAPI::Unit unit)
{
	unitSet.insert(unitSet.begin(), unit);
	currentComp.addType(unit->getType());

	if (!active && currentComp >= targetComp)	
		activate();	
}

void Coalition::removeAgent(Agent* agent)
{	
	agentSet.erase(agent);
	removeUnit(agent->getUnit());	

	//if all agents die while coalition is activated, the task is a failure
	//or
	//if no agents can deal damage in an attack coalition, the task is a failure
	if (active && (currentComp.getCost() == 0 || agentSet.size() == 0 || (task->getType() == ATT && currentComp.getAttributes().groundDPS == 0 && currentComp.getAttributes().airDPS == 0)))
		task->fail();

	if (agent->getCoalition() != this)
		return;

	agent->unbind();
}

void Coalition::removeUnit(BWAPI::Unit unit)
{
	unitSet.erase(unit);
	currentComp.removeType(unit->getType());
	/*std::cout << "A " << unit->getType() << " has left a coalition\n";*/
}

void Coalition::printAttributes()
{
	std::cout << "Coalition " << coalitionID << "\n";
	currentComp.printAttributes();
}

void Coalition::logCoalition()
{
	std::cout << "Logging Performance...\n";
	std::ofstream composition;
	auto race = util::game::getSelf()->getRace().toString();
	//composition.open("compositions\\" + BWAPI::Broodwar->mapName() + "\\" + util::getEnemy()->getRace().getName() + "\\" + race + "_" + task->getName() + ".txt");
	composition.open("compositions\\" + util::game::getEnemy()->getRace().getName() + "\\" + race + "_" + task->getName() + ".txt");
	composition << "Map Name: " << BWAPI::Broodwar->mapName() << " : " << BWAPI::Broodwar->mapFileName() << "\n";
	composition << "EngageDuration: " << engageDuration << "\n";
	composition << "KillCount: " << killCount << "\n";
	composition << "Fitness: " << getFitness() << "\n";
	composition << "Cost: " << getCost() << "\n";
	composition << "Profit: " << getProfit() << "\n";
	composition << targetComp.toString();
	composition.close();
}