#pragma once

#include "Agent.h"
#include "Agentset.h"
#include "Baseset.h"
#include "ResourceDepot.h"
#include "Worker.h"
#include "vector"
#include "BWTA.h"

class AgentManager
{
private:
	Agentset agentSet;
	Baseset resourceDepots;
	std::set<BWTA::BaseLocation*> candidateBases;

	static AgentManager* instance;
	Agentset::iterator lastServiced;
public:
	//constructors and destructors
	AgentManager();
	~AgentManager();

	//setters
	void setLastServiced(Agentset::iterator lastServiced);

	//getters
	static AgentManager* getInstance();
	Agent* getAgent(int id);
	const Agentset& getAgentset();
	const Baseset& getResourceDepots();
	std::set<BWTA::BaseLocation*>& getCandidateBases();
	Agentset::iterator getLastServiced();

	//-
	void resetLastServiced();	
	void createAgent(BWAPI::Unit unit);
	void removeAgent(int id);
	Agentset::iterator removeAgent(Agentset::iterator agent);
};