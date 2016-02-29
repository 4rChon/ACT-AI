#include "DesireHelper.h"
#include "AgentHelper.h"
#include "UtilHelper.h"

namespace DesireHelper
{
	namespace
	{
		static std::unordered_map<BWAPI::UnitType, double> unitDesireMap;
		static std::unordered_map<BWAPI::UpgradeType, double> upgradeDesireMap;
		static std::unordered_map<BWAPI::TechType, double> techDesireMap;
		static std::unordered_map<BWTA::BaseLocation*, double, std::hash<void*>> expansionDesireMap;
		static std::unordered_map<MapHelper::Zone*, double, std::hash<void*>> attackDesireMap;
		static std::unordered_map<MapHelper::Zone*, double, std::hash<void*>> defendDesireMap;
		static double supplyDesire;
		static double expandDesire;
	}

	void initialiseHelper()
	{
		for (auto &unitType : BWAPI::UnitTypes::allUnitTypes())
			if(unitType.getRace() == BWAPI::Broodwar->self()->getRace())
				unitDesireMap.insert(std::pair<BWAPI::UnitType, double>(unitType, 0.1));

		for (auto &upgradeType : BWAPI::UpgradeTypes::allUpgradeTypes())
			if(upgradeType.getRace() == BWAPI::Broodwar->self()->getRace())
				upgradeDesireMap.insert(std::pair<BWAPI::UpgradeType, double>(upgradeType, 0.1));

		for (auto &techType : BWAPI::TechTypes::allTechTypes())
			if (techType.getRace() == BWAPI::Broodwar->self()->getRace())
				techDesireMap.insert(std::pair<BWAPI::TechType, double>(techType, 0.1));

		for (auto &expansion : BWTA::getBaseLocations())
		{
			double desire = 0.0;
			if(!expansion->isIsland())
				desire = 10 + std::log(1.0 / expansion->getGroundDistance(BWTA::getStartLocation(BWAPI::Broodwar->self())));
			expansionDesireMap.insert(std::pair<BWTA::BaseLocation*, double>(expansion, desire));
		}

		for (auto &region : BWAPI::Broodwar->getAllRegions())
		{
			attackDesireMap.insert(std::pair<MapHelper::Zone*, double>(MapHelper::getZone(region), 0.0));
			defendDesireMap.insert(std::pair<MapHelper::Zone*, double>(MapHelper::getZone(region), 0.0));
		}

		supplyDesire = 1.0;
		expandDesire = 0.0;
	}

	void updateUnitDesireMap()
	{
		unitDesireMap[BWAPI::UnitTypes::Terran_Marine] = 1.0;
		if (BWAPI::Broodwar->self()->allUnitCount(BWAPI::Broodwar->self()->getRace().getWorker()) >= 70)
			unitDesireMap[BWAPI::Broodwar->self()->getRace().getWorker()] = 0.0;
		else
			unitDesireMap[BWAPI::Broodwar->self()->getRace().getWorker()] = 1.0;
	}

	BWAPI::UnitType getMostDesirableUnit(BWAPI::UnitType* producer)
	{
		
		auto bestUnit = std::pair<BWAPI::UnitType, double>(BWAPI::UnitTypes::None, 0.0);
		for each(auto &unit in unitDesireMap)
		{
			if (producer != nullptr)
				if (unit.first.whatBuilds().first != *producer)
					continue;
			if (unit.second > bestUnit.second)
				bestUnit = unit;
		}
		return bestUnit.first;
		
	}

	void updateUpgradeDesireMap()
	{
	}

	void updateTechDesireMap()
	{
	}

	void updateExpansionDesireMap()
	{
		for (auto &expansion : expansionDesireMap)
			if (BWAPI::Broodwar->getRegionAt(expansion.first->getPosition())->getUnits(BWAPI::Filter::IsResourceDepot).size())
				expansionDesireMap[expansion.first] = 0.0;
	}

	BWTA::BaseLocation* getBestExpansionLocation()
	{
		BWTA::BaseLocation* bestLocation = (*expansionDesireMap.begin()).first;
		double bestScore = (*expansionDesireMap.begin()).second;
		for (auto &expansion : expansionDesireMap)
			if (bestScore < expansion.second)
			{
				bestLocation = expansion.first;
				bestScore = expansion.second;
			}
		if (bestScore == 0.0)
			return nullptr;
		return bestLocation;
	}

	void updateDesireMaps()
	{
		updateUnitDesireMap();
		updateUpgradeDesireMap();
		updateTechDesireMap();
		updateExpansionDesireMap();
	}

	void updateSupplyDesire(BWAPI::UnitType unitType, bool justDied)
	{
		if (BWAPI::Broodwar->self()->supplyTotal() >= 200)
		{
			supplyDesire = 0.0;
			return;
		}
		
		int unitSupply = unitType.supplyProvided() - unitType.supplyRequired();
		if (justDied)
			unitSupply *= -1;
		supplyDesire -= (double)unitSupply / BWAPI::Broodwar->self()->getRace().getCenter().supplyProvided();		
	}

	void updateExpandDesire()
	{
		expandDesire = 0;
		auto resourceDepots = AgentHelper::getResourceDepots();
		for each(auto &resourceDepot in resourceDepots)
			expandDesire += resourceDepot->getExpandDesire();
		expandDesire /= BWAPI::Broodwar->self()->allUnitCount(BWAPI::Broodwar->self()->getRace().getCenter());
	}

	const std::unordered_map<BWAPI::UnitType, double>& getUnitDesireMap()
	{
		return unitDesireMap;
	}

	const std::unordered_map<BWAPI::UpgradeType, double>& getUpgradeDesireMap()
	{
		return upgradeDesireMap;
	}

	const std::unordered_map<BWAPI::TechType, double>& getTechDesireMap()
	{
		return techDesireMap;
	}

	const std::unordered_map<BWTA::BaseLocation*, double, std::hash<void*>>& getExpansionDesireMap()
	{
		return expansionDesireMap;
	}

	double getSupplyDesire()
	{
		return supplyDesire;
	}

	double getExpandDesire()
	{
		return expandDesire;
	}

	void setExpansionDesire(BWTA::BaseLocation* baseLocation, double desire)
	{
		expansionDesireMap[baseLocation] = desire;
	}

	double getExpansionDesire(BWTA::BaseLocation* baseLocation)
	{
		return expansionDesireMap[baseLocation];
	}

	const std::unordered_map<MapHelper::Zone*, double, std::hash<void*>>& getAttackDesireMap()
	{
		return attackDesireMap;
	}

	const std::unordered_map<MapHelper::Zone*, double, std::hash<void*>>& getDefendDesireMap()
	{
		return defendDesireMap;
	}
}
