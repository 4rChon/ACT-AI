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

Coalition::Coalition(Composition targetComp, Task * const & task)
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

	for (auto &agent : agentSet)
		agent->unbind();
}

int Coalition::getAge() const
{
	if(!active)
		return BWAPI::Broodwar->getFrameCount() - creationFrame;
	//std::cout << "Creation Frame " << creationFrame << "\n";
	return activationFrame - creationFrame;
}

double Coalition::getCost()
{
	std::vector<double> valueArr;
	std::vector<double> coeffArr;
	valueArr.push_back(currentComp.getCost());
	coeffArr.push_back(0.0001);
	valueArr.push_back(getAge());
	coeffArr.push_back(0.0001);
	cost = util::calc::normaliseValues(valueArr, coeffArr);
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
	profit = util::calc::normaliseValues(valueArr, coeffArr);
	return profit;
}

double Coalition::getFitness()
{
	std::vector<double> valueArr;
	std::vector<double> coeffArr;
	valueArr.push_back(getProfit()/getCost());
	coeffArr.push_back(0.1);
	return util::calc::normaliseValues(valueArr, coeffArr);
}

void Coalition::activate()
{
	activationFrame = BWAPI::Broodwar->getFrameCount();
	active = true;
	targetComp = currentComp;
	for (auto &agent : agentSet)
		agent->bind();
}

bool Coalition::addAgent(Agent* const& agent)
{	
	if (agent->getCoalitionID() != -1)
		return false;

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

void Coalition::removeAgent(Agent* const& agent)
{	
	agentSet.erase(agent);
	removeUnit(agent->getUnit());	

	//if all agents die while coalition is activated, the task is a failure
	//or
	//if no agents can deal damage in an attack coalition, the task is a failure
	if (active && (currentComp.getCost() == 0 || agentSet.size() == 0 || (task->getType() == ATT && currentComp.getAttributes().groundDPS == 0 && currentComp.getAttributes().airDPS == 0)))
		task->fail();

	if (agent->getCoalitionID() != coalitionID)
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