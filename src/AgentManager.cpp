#include "AgentManager.h"
#include "Producer.h"
#include "Worker.h"

AgentManager* AgentManager::instance = nullptr;

AgentManager::AgentManager()
{
	this->freeAgents = 0;
}

AgentManager::~AgentManager()
{
	this->agentSet.clear();
	this->instance = nullptr;
}

AgentManager* AgentManager::getInstance()
{
	if (!instance)
		instance = new AgentManager();
	return instance;
}

int AgentManager::getFreeCount()
{
	return this->freeAgents;
}

Agent* AgentManager::getAgent(int unitID)
{
	for (auto &agent : this->agentSet)
		if (agent->getID() == unitID)
			return agent;
	return nullptr;
}

Agentset& AgentManager::getAgentset()
{
	return this->agentSet;
}

void AgentManager::addAgent(BWAPI::Unit unit)
{
	Agent* agent = nullptr;
	if (unit->getType().isBuilding() && unit->getType().canProduce())
		agent = new Producer(unit, 1);
	else if (unit->getType().isWorker()) //refactor
		agent = new Worker(unit, 0.01);
	else
		agent = new Agent(unit, 1);

	this->agentSet.insert(agent);
	this->freeAgents++;	
}

void AgentManager::removeAgent(BWAPI::Unit unit)
{
	for (auto &agent : this->agentSet)
		if (agent->getID() == unit->getID())
		{
			removeAgent(agent);			
			return;
		}
}

void AgentManager::removeAgent(Agent* agent)
{	
	if (agent->isFree())
		this->freeAgents--;
	this->agentSet.erase(agent);
}

Agentset::iterator AgentManager::removeAgent(Agentset::iterator agent)
{	
	if ((*agent)->isFree())
		this->freeAgents--;
	return this->agentSet.erase(agent);
}

void AgentManager::freeAgent(Agent* agent)
{
	if (agent->isFree() || !agent->getUnit()->exists())
		return;
	agent->setFree(true);
	this->freeAgents++;
}

void AgentManager::bindAgent(Agent* agent)
{
	if (!agent->isFree() || !agent->getUnit()->exists())
		return;
	agent->setFree(false);
	this->freeAgents--;
}


void AgentManager::act()
{
	for (auto &agent : this->agentSet)
		if (agent->isFree())
			agent->act();
}

void AgentManager::updateUnlocked()
{
	for (auto &agent : this->agentSet)
		agent->updateUnlocked();
}