#pragma once

#include "Agent.h"
#include "Agentset.h"

class AgentManager
{
private:
	Agentset agentSet;	
	static AgentManager* instance;
	int freeAgents;
public:
	//constructors and destructors
	AgentManager();
	~AgentManager();	

	//getters
	static AgentManager* getInstance();
	Agent* getAgent(int unitID);
	Agentset& getAgentset();
	int getFreeCount();

	//-
	void addAgent(BWAPI::Unit unit);
	void removeAgent(BWAPI::Unit unit);	
	void removeAgent(Agent* agent);
	Agentset::iterator removeAgent(Agentset::iterator agent);
	void freeAgent(Agent* agent);
	void bindAgent(Agent* agent);
	void act();
};