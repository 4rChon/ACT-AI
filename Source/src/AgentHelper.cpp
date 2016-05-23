#include "AgentHelper.h"
#include "Coalition.h"

namespace AgentHelper
{
	namespace
	{
		static Agentset agentSet;
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
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva
			|| unit->getType() == BWAPI::UnitTypes::Zerg_Egg
			|| unit->getType() == BWAPI::UnitTypes::Zerg_Cocoon
			|| unit->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg			
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
			ResourceDepot* const& agent = new ResourceDepot(unit);
			resourceDepots.insert(agent);
			agentSet.insert(agent);
		}
		else if (unit->getType().isWorker())
		{
			Worker* const& agent = new Worker(unit);
			agentSet.insert(agent);
		}
		else if (unit->getType() == BWAPI::UnitTypes::Terran_Comsat_Station)
		{
			ComsatStation* const& agent = new ComsatStation(unit);
			agentSet.insert(agent);
		}
		else
		{
			Agent* const& agent = new Agent(unit);
			agentSet.insert(agent);
		}

		
	}

	void removeAgent(int id)
	{
		Agent* const& agent = getAgent(id);
		if (!agent)
			std::cout << "\tAgent not found\n";
		else
		{
			auto coalition = agent->getCoalition();
			if (coalition)
				coalition->removeAgent(agent);

			agent->unbind();
			agentSet.erase(agent);
			if (resourceDepots.count((ResourceDepot*)agent))
				resourceDepots.erase((ResourceDepot*)agent);

			delete agent;
		}

		resetLastServiced();
	}
}