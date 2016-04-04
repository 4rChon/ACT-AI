#include "DesireHelper.h"
#include "AgentHelper.h"
#include "UtilHelper.h"
#include "CoalitionHelper.h"

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
			if(unitType.getRace() == util::getSelf()->getRace())
				unitDesireMap.insert(std::pair<BWAPI::UnitType, double>(unitType, 0.0));

		for (auto &upgradeType : BWAPI::UpgradeTypes::allUpgradeTypes())
			if(upgradeType.getRace() == util::getSelf()->getRace())
				upgradeDesireMap.insert(std::pair<BWAPI::UpgradeType, double>(upgradeType, 0.0));

		for (auto &techType : BWAPI::TechTypes::allTechTypes())
			if (techType.getRace() == util::getSelf()->getRace())
				techDesireMap.insert(std::pair<BWAPI::TechType, double>(techType, 0.0));

		for (auto &expansion : BWTA::getBaseLocations())
		{
			double desire = 0.0;
			if (!expansion->isIsland())
			{
				std::vector<double> valueArr;
				std::vector<double> coeffArr;
				valueArr.push_back(1/expansion->getGroundDistance(BWTA::getStartLocation(util::getSelf())));
				coeffArr.push_back(1);
				desire = util::normaliseValues(valueArr, coeffArr);
			}
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
		Composition comp = Composition();
		for (auto& coalition : CoalitionHelper::getCoalitionset())
		{
			Composition differenceComposition = coalition->getTargetComp() - coalition->getCurrentComp();
			for (auto& unit : differenceComposition.getUnitMap())
				comp.addType(unit.first, unit.second);
		}

		for (auto& unit : unitDesireMap)
			unitDesireMap[unit.first] = comp[unit.first];
	}

	BWAPI::UnitType getMostDesirableUnit(BWAPI::UnitType producer)
	{
		if (util::getSelf()->allUnitCount(util::getSelf()->getRace().getWorker()) >= 70)
			unitDesireMap[util::getSelf()->getRace().getWorker()] = 0.0;
		else
			unitDesireMap[util::getSelf()->getRace().getWorker()] = 1.0;

		auto bestUnit = std::pair<BWAPI::UnitType, double>(BWAPI::UnitTypes::None, 0.0);
		for each(auto &unit in unitDesireMap)
		{
			if (producer != BWAPI::UnitTypes::None)
				if (unit.first.whatBuilds().first != producer || !util::getSelf()->isUnitAvailable(unit.first))
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
		for (auto &expansion : BWTA::getBaseLocations())
		{
			if (expansion->isIsland() || BWAPI::Broodwar->getRegionAt(expansion->getPosition())->getUnits(BWAPI::Filter::IsResourceDepot).size() > 0)
				expansionDesireMap[expansion] = 0.0;
			else
			{
				std::vector<double> valueArr;
				std::vector<double> coeffArr;
				valueArr.push_back(1/expansion->getGroundDistance(BWTA::getStartLocation(util::getSelf())));
				coeffArr.push_back(1.0);
				expansionDesireMap[expansion] = util::normaliseValues(valueArr, coeffArr);
			}
		}

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
		updateUpgradeDesireMap();
		updateTechDesireMap();
		updateExpansionDesireMap();
		updateUnitDesireMap();
	}

	void updateSupplyDesire(BWAPI::UnitType unitType, bool justDied)
	{
		int unitSupply = unitType.supplyProvided() - unitType.supplyRequired();
		if (justDied)
			unitSupply *= -1;
		supplyDesire -= (double)unitSupply / util::getSelf()->getRace().getCenter().supplyProvided();
	}

	void updateSupplyDesire()
	{
		if (util::getSelf()->supplyTotal() >= 400)
		{
			supplyDesire = 0.0;
			return;
		}

		supplyDesire = 1 - ((double)(util::getSelf()->supplyTotal() - util::getSelf()->supplyUsed())/ util::getSelf()->getRace().getCenter().supplyProvided());
	}

	void updateExpandDesire()
	{
		bool canExpand = false;
		expandDesire = 0;
		for (auto &expand : expansionDesireMap)
		{
			if (expand.second > 0)
				canExpand = true;
		}
		if (!canExpand)
			return;

		auto resourceDepots = AgentHelper::getResourceDepots();
		for each(auto &resourceDepot in resourceDepots)
			expandDesire += resourceDepot->getMineralSaturation();
		expandDesire /= util::getSelf()->allUnitCount(util::getSelf()->getRace().getCenter());
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
