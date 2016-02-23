#include "Coalition.h"
#include "AgentHelper.h"
#include "CoalitionHelper.h"
#include "Task.h"

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
	initAttributes();
	outAttributes();
}

Coalition::~Coalition()
{
	std::cout << "~Coalition : " << coalitionID << "\n";
	active = false;
	for (auto agent : agentSet)
		agent->unbind();

	taskID = -1;
	task = nullptr;

	agentSet.clear();
	unitSet.clear();
	/*CoalitionHelper::removeCoalition(this);*/
}

int Coalition::getAge() const
{
	return BWAPI::Broodwar->getFrameCount() - creationFrame;
}

int Coalition::getID() const
{
	return this->coalitionID;
}

BWAPI::Unitset Coalition::getUnitSet() const
{
	return this->unitSet;
}

Agentset Coalition::getAgentSet() const
{
	return this->agentSet;
}

double Coalition::getCost()
{
	if (!active)
		cost += (BWAPI::Broodwar->getFrameCount() - creationFrame);
	return 1 - (1 / ((this->cost + 500) / 500)); //convert to sigma function instead : 1/(1 + e^-x)
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
	return this->targetComp;
}

Coalition::Attributes &Coalition::getAttributes()
{
	return attributes;
}

bool Coalition::isActive() const
{
	return this->active;
}

bool Coalition::addAgent(Agent* agent)
{
	if (!agentSet.count(agent))
	{
		for (auto type : this->targetComp.getTypes())
		{
			if (agent->getUnit()->getType() == type && this->targetComp[type] > this->currentComp[type])
			{
				std::cout << agent->getUnit()->getType().c_str() << " is joining a coalition\n";
				this->agentSet.insert(this->agentSet.begin(), agent);
				this->addUnit(agent->getUnit());
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
	this->unitSet.insert(this->unitSet.begin(), unit);
	this->currentComp += unit->getType();

	if (!active && currentComp == targetComp)
	{
		std::cout << "A coalition has been activated!\n";
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
	this->unitSet.erase(unit);
	this->currentComp -= unit->getType();
	std::cout << "A " << unit->getType() << " has left a coalition\n";
}

void Coalition::initAttributes()
{
	attributes.airDPS = 0.0;
	attributes.groundDPS = 0.0;
	attributes.avgAirRange = 0.0;
	attributes.avgGroundRange = 0.0;
	attributes.avgSpeed = 0.0;
	attributes.detection = false;	
	attributes.maxAirRange = 0;
	attributes.maxGroundRange = 0;
	attributes.totalHealth = 0;
	
	for each(auto &unitType in targetComp.getTypes())
	{				
		attributes.airDPS += unitType.airWeapon().damageAmount() ? targetComp.getUnitMap()[unitType] * (unitType.airWeapon().damageAmount() * unitType.maxAirHits() * (24 / (double)unitType.airWeapon().damageCooldown())) : 0;		
		attributes.groundDPS += unitType.groundWeapon().damageAmount() ? targetComp.getUnitMap()[unitType] * (unitType.groundWeapon().damageAmount() * unitType.maxGroundHits() * (24 / (double)unitType.groundWeapon().damageCooldown())) : 0;
		attributes.avgAirRange += targetComp.getUnitMap()[unitType] * (unitType.airWeapon().maxRange() / 32);
		attributes.avgGroundRange += targetComp.getUnitMap()[unitType] * (unitType.groundWeapon().maxRange() / 32);
		attributes.avgSpeed += targetComp.getUnitMap()[unitType] * (unitType.topSpeed());
		attributes.detection = unitType.isDetector() ? unitType.isDetector() : attributes.detection;
		attributes.maxAirRange = unitType.airWeapon().maxRange() / 32 > attributes.maxAirRange ? unitType.airWeapon().maxRange() / 32 : attributes.maxAirRange;
		attributes.maxGroundRange = unitType.groundWeapon().maxRange() / 32 > attributes.maxGroundRange ? unitType.groundWeapon().maxRange() / 32 : attributes.maxGroundRange;
		attributes.totalHealth += targetComp.getUnitMap()[unitType] * (unitType.maxHitPoints() + unitType.maxShields());
	}

	int totalCount = 0;
	for each(auto &unitType in targetComp.getTypes())
		totalCount += targetComp.getUnitMap()[unitType];

	attributes.avgAirRange /= totalCount;
	attributes.avgGroundRange /= totalCount;
	attributes.avgSpeed /= totalCount;
}

void Coalition::outAttributes()
{
	std::cout << "Coalition " << coalitionID << "\n";
	std::cout << "\tairDPS: " << attributes.airDPS << "\n";
	std::cout << "\tgroundDPS: " << attributes.groundDPS << "\n";
	std::cout << "\tavgAirRange: " << attributes.avgAirRange << "\n";
	std::cout << "\tavgGroundRange: " << attributes.avgGroundRange << "\n";
	std::cout << "\tavgSpeed: " << attributes.avgSpeed << "\n";
	std::cout << "\tdetection: " << attributes.detection << "\n";
	std::cout << "\tmaxAirRange: " << attributes.maxAirRange << "\n";
	std::cout << "\tmaxGroundRange: " << attributes.maxGroundRange << "\n";
	std::cout << "\ttotalHealth: " << attributes.totalHealth << "\n";
}