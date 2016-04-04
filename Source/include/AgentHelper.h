#pragma once

#include "Agent.h"
#include "ResourceDepot.h"
#include "Worker.h"
#include <BWTA.h>
#include <vector>

typedef BWAPI::SetContainer < Agent*, std::hash<void*>> Agentset;
typedef BWAPI::SetContainer < ResourceDepot*, std::hash<void*>> Baseset;

namespace AgentHelper
{
	void initialiseHelper();

	///setters
	//
	void setLastServiced(Agentset::iterator lastServiced);

	//getters
	Agent* getAgent(int id);
	const Agentset& getAgentset();
	const Baseset& getResourceDepots();
	const Agentset & getComsatStations();
	Agentset::iterator getLastServiced();

	//-
	void resetLastServiced();
	void createAgent(BWAPI::Unit unit);
	void removeAgent(Agent* agent);
	void removeAgent(int id);
	Agentset::iterator removeAgent(Agentset::iterator agent);
}