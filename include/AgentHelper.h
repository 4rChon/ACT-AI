#pragma once

#include "Agent.h"
#include "Agentset.h"
#include "Baseset.h"
#include "ResourceDepot.h"
#include "Worker.h"
#include <BWTA.h>
#include <vector>

namespace AgentHelper
{
	void initialiseHelper();

	//setters
	void setLastServiced(Agentset::iterator lastServiced);

	//getters
	Agent* getAgent(int id);
	const Agentset& getAgentset();
	const Baseset& getResourceDepots();
	Agentset::iterator getLastServiced();

	//-
	void resetLastServiced();	
	void createAgent(BWAPI::Unit unit);
	void removeAgent(int id);
	void freeAgent(Agent* agent);
	void unbind(Agent* agent);
	Agentset::iterator removeAgent(Agentset::iterator agent);
}