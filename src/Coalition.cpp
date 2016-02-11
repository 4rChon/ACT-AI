#include "..\include\Coalition.h"
#include "..\include\AgentHelper.h"
#include "..\include\CoalitionHelper.h"
#include "..\include\Task.h"

Coalition::Coalition()
{
	active = false;
	creationFrame = BWAPI::Broodwar->getFrameCount();	
	task = nullptr;
	taskID = -1;
	coalitionID = CoalitionHelper::getNextID();
	cost = 0.0;
	profit = 0.0;
}

Coalition::Coalition(Composition targetComp, Task* task)
{
	this->task = task;
	this->targetComp = targetComp;

	coalitionID = CoalitionHelper::getNextID();
	active = false;
	taskID = task->getID();
	creationFrame = BWAPI::Broodwar->getFrameCount();
	cost = targetComp.getCost();
	profit = 0.0;
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
	CoalitionHelper::removeCoalition(this);
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
	//{
		cost += (BWAPI::Broodwar->getFrameCount() - creationFrame);
		//creationFrame = BWAPI::Broodwar->getFrameCount();
	//}
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
	this->agentSet.erase(agent);
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