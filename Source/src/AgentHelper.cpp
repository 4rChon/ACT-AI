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
		static TypeCountMap typeCountMap;
		static int lastErrorFrame;
		static BWAPI::Error lastError;
	}

	void initialiseHelper()
	{		
		lastServiced = agentSet.begin();
		typeCountMap.clear();
		lastErrorFrame = 0;
		lastError = BWAPI::Errors::None;
	}

	bool updateLastError()
	{
		if (BWAPI::Broodwar->getLastError() != BWAPI::Errors::None)
		{
			lastErrorFrame = BWAPI::Broodwar->getFrameCount();
			lastError = BWAPI::Broodwar->getLastError();
			BWAPI::Broodwar->setLastError();
		}
		return true;
	}

	int getLastErrorFrame()
	{
		return lastErrorFrame;
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

	int getTypeCount(BWAPI::UnitType unitType)
	{
		return typeCountMap[unitType];
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
			//std::cout << "Added new Resource Depot : " << unit->getID() << "\n";
			resourceDepots.insert((ResourceDepot*)agent);
		}
		else if (unit->getType().isWorker())
		{
			if (unit->getType() == BWAPI::UnitTypes::Terran_SCV)
			{
				agent = new SCV(unit);
				//std::cout << "Added new SCV : " << unit->getType() << " : " << unit->getID() << "\n";
			}
			else
			{
				agent = new Worker(unit);
				//std::cout << "Added new Worker : " << unit->getType() << " : " << unit->getID() << "\n";
			}
		}
		else
		{
			agent = new Agent(unit);
			/*std::cout << "Added new Unit : " << unit->getType() << " : " << unit->getID() << "\n";*/
		}

		agentSet.insert(agent);
		if (unit->getType() == BWAPI::UnitTypes::Terran_Comsat_Station)
			comsatStations.insert(agent);
		typeCountMap[agent->getUnit()->getType()]++;
	}

	void removeAgent(Agent* agent)
	{
		agent->unbind();
		agentSet.erase(agent);
		if(agent->getUnit()->getType().isResourceDepot())
			resourceDepots.erase((ResourceDepot*)agent);
		comsatStations.erase(agent);
		typeCountMap[agent->getUnit()->getType()]--;
	}

	void removeAgent(int id)
	{
		//std::cout << "Removing Agent : " << id << "\n";
		Agent* agent = getAgent(id);
		if (!agent)
			std::cout << "\tAgent not found\n";
		else
		{
			if (agent->getCoalitionID() != -1)
				agent->getCoalition()->removeAgent(agent);
			else
				agent->unbind();
			agentSet.erase(agent);
			if (resourceDepots.count((ResourceDepot*)agent))
				resourceDepots.erase((ResourceDepot*)agent);
			delete agent;
		}
		resetLastServiced();
	}

	Agentset::iterator removeAgent(Agentset::iterator agent)
	{
		(*agent)->unbind();
		return agentSet.erase(agent);
	}
}