#pragma once

#include "Agent.h"
#include "Agentset.h"
#include "Baseset.h"
#include "ResourceDepot.h"
#include "Worker.h"
#include "vector"
#include "BWTA.h"

namespace AgentHelper
{
	static Agentset agentSet;
	static Baseset resourceDepots;
	static std::set<BWTA::BaseLocation*> candidateBases;
	static Agentset::iterator lastServiced;
	
	void initialiseHelper();

	//setters
	void setLastServiced(Agentset::iterator lastServiced);

	//getters
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
}