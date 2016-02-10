#include "..\include\AgentHelper.h"
#include "..\include\CommandCenter.h"
#include "..\include\Hatchery.h"
#include "..\include\SCV.h"

namespace AgentHelper
{
	void AgentHelper::initialiseHelper()
	{		
		lastServiced = agentSet.begin();
		candidateBases = BWTA::getBaseLocations();
		candidateBases.erase(BWTA::getStartLocation(BWAPI::Broodwar->self()));
	}

	Agent* AgentHelper::getAgent(int id)
	{
		for (auto &a : agentSet)
			if (a->getID() == id)
				return a;
		return nullptr;
	}

	const Agentset& AgentHelper::getAgentset()
	{
		return agentSet;
	}

	const Baseset& AgentHelper::getResourceDepots()
	{
		return resourceDepots;
	}

	std::set<BWTA::BaseLocation*>& AgentHelper::getCandidateBases()
	{
		return candidateBases;
	}

	Agentset::iterator AgentHelper::getLastServiced()
	{
		if (lastServiced == agentSet.end())
			resetLastServiced();
		return lastServiced;
	}

	void AgentHelper::resetLastServiced()
	{
		lastServiced = agentSet.begin();
	}

	void AgentHelper::setLastServiced(Agentset::iterator lastServiced)
	{
		lastServiced = lastServiced;
	}

	void AgentHelper::createAgent(BWAPI::Unit unit)
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

		if (unit->getType().isBuilding() && unit->getType().canProduce())
		{
			if (unit->getType().isResourceDepot())
			{
				if (unit->getType() == BWAPI::UnitTypes::Terran_Command_Center)
				{
					agent = new CommandCenter(unit);
					std::cout << "Added new Command Center : " << unit->getID() << "\n";
					resourceDepots.insert((CommandCenter*)agent);
				}
				else if (unit->getPlayer()->getRace() == BWAPI::Races::Zerg)
				{
					agent = new Hatchery(unit);
					std::cout << "Added new Hatchery : " << unit->getID() << "\n";
					resourceDepots.insert((Hatchery*)agent);
				}
				else
				{
					agent = new ResourceDepot(unit);
					std::cout << "Added new Resource Depot : " << unit->getID() << "\n";
					resourceDepots.insert((ResourceDepot*)agent);
				}
			}
			else
			{
				agent = new Producer(unit);
				std::cout << "Added new Producer : " << unit->getType() << " : " << unit->getID() << "\n";
			}
		}
		else
		{
			if (unit->getType().isWorker())
			{
				if (unit->getType() == BWAPI::UnitTypes::Terran_SCV)
				{
					agent = new SCV(unit);
					std::cout << "Added new SCV : " << unit->getType() << " : " << unit->getID() << "\n";
				}
				else
				{
					agent = new Worker(unit);
					std::cout << "Added new Worker : " << unit->getType() << " : " << unit->getID() << "\n";
				}
			}
			else
			{
				agent = new Agent(unit);
				std::cout << "Added new Unit : " << unit->getType() << " : " << unit->getID() << "\n";
			}
		}
		agentSet.insert(agent);
	}

	void AgentHelper::removeAgent(int id)
	{
		//std::cout << "Removing Agent : " << id << "\n";
		Agent* agent = getAgent(id);
		if (!agent)
			std::cout << "\tAgent not found\n";
		else
		{
			agentSet.erase(agent);
			if (resourceDepots.count((ResourceDepot*)agent))
				resourceDepots.erase((ResourceDepot*)agent);
			delete agent;
		}
		resetLastServiced();
	}

	Agentset::iterator AgentHelper::removeAgent(Agentset::iterator agent)
	{
		return agentSet.erase(agent);
	}
}