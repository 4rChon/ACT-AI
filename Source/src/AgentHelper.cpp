#include "AgentHelper.h"
#include "SCV.h"
#include "Coalition.h"

namespace AgentHelper
{
	namespace
	{
		static Agentset agentSet;
		static Agentset comsatStations;
		static Baseset resourceDepots;
		static Agentset::iterator lastServiced;
	}

	void initialiseHelper()
	{
		lastServiced = agentSet.begin();
	}

	Agent* getAgent(int id)
	{
		for (auto &a : agentSet)
			if (a->getID() == id)
				return a;
		return nullptr;
	}

	const Agentset& getAgentset()
	{
		return agentSet;
	}

	const Baseset& getResourceDepots()
	{
		return resourceDepots;
	}

	const Agentset& getComsatStations()
	{
		return comsatStations;
	}

	Agentset::iterator getLastServiced()
	{
		if (lastServiced == agentSet.end())
			resetLastServiced();
		return lastServiced;
	}

	void resetLastServiced()
	{
		lastServiced = agentSet.begin();
	}

	void setLastServiced(Agentset::iterator newLastServiced)
	{
		lastServiced = newLastServiced;
	}

	void createAgent(BWAPI::Unit unit)
	{
		Agent* agent = nullptr;
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva
			|| unit->getType() == BWAPI::UnitTypes::Zerg_Egg
			|| unit->getType() == BWAPI::UnitTypes::Zerg_Cocoon
			|| unit->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg
			|| unit->getType() == BWAPI::UnitTypes::Terran_Supply_Depot
			|| unit->getType() == BWAPI::UnitTypes::Protoss_Pylon
			)
			return;

		if (unit->getType() == BWAPI::UnitTypes::Zerg_Lair
			|| unit->getType() == BWAPI::UnitTypes::Zerg_Hive)
		{
			getAgent(unit->getID())->setUnit(unit);
			return;
		}

		if (unit->getType().isResourceDepot())
		{
			agent = new ResourceDepot(unit);
			resourceDepots.insert((ResourceDepot*)agent);
		}
		else if (unit->getType().isWorker())
		{
			if (unit->getType() == BWAPI::UnitTypes::Terran_SCV)
				agent = new SCV(unit);
			else
				agent = new Worker(unit);
		}
		else
			agent = new Agent(unit);

		agentSet.insert(agent);
		if (unit->getType() == BWAPI::UnitTypes::Terran_Comsat_Station)
			comsatStations.insert(agent);
	}

	void removeAgent(int id)
	{
		Agent* agent = getAgent(id);
		if (!agent)
			std::cout << "\tAgent not found\n";
		else
		{
			if (agent->getCoalitionID() != -1)
				agent->getCoalition()->removeAgent(agent);

			agent->unbind();
			agentSet.erase(agent);
			if (resourceDepots.count((ResourceDepot*)agent))
				resourceDepots.erase((ResourceDepot*)agent);
			if (comsatStations.count(agent))
				comsatStations.erase(agent);

			delete agent;
		}

		resetLastServiced();
	}
}