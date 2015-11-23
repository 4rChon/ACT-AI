#pragma once

#include "Agent.h"
#include "Agentset.h"

class AgentManager
{
private:
	Agentset agentSet;
	static AgentManager* instance;
	int freeAgents;
	Agentset::iterator lastServiced;
public:
	//constructors and destructors
	AgentManager();
	~AgentManager();

	//setters
	void setLastServiced(Agentset::iterator lastServiced);

	//getters
	static AgentManager* getInstance();
	Agent* getAgent(int unitID);
	Agentset& getAgentset();
	int getFreeCount();
	Agentset::iterator getLastServiced();

	//-
	void addAgent(BWAPI::Unit unit);
	void removeAgent(BWAPI::Unit unit);
	void removeAgent(Agent* agent);
	Agentset::iterator removeAgent(Agentset::iterator agent);
	void freeAgent(Agent* agent);
	void bindAgent(Agent* agent);
	void updateUnlocked();
	void act();
	void incLastServiced();
};