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
		static std::unordered_map<Zone*, double, std::hash<void*>> attackDesireMap;
		static std::unordered_map<Zone*, double, std::hash<void*>> bunkerDefenseDesireMap;
		static std::unordered_map<Zone*, double, std::hash<void*>> turretDefenseDesireMap;
		static std::unordered_map<Zone*, double, std::hash<void*>> unitDefenseDesireMap;
		static double supplyDesire;
		static double expandDesire;
	}

	void initialiseHelper()
	{
		for (auto &unitType : BWAPI::UnitTypes::allUnitTypes())
			if(unitType.getRace() == util::game::getSelf()->getRace())
				unitDesireMap.insert(std::pair<BWAPI::UnitType, double>(unitType, 0.0));

		for (auto &upgradeType : BWAPI::UpgradeTypes::allUpgradeTypes())
			if(upgradeType.getRace() == util::game::getSelf()->getRace())
				upgradeDesireMap.insert(std::pair<BWAPI::UpgradeType, double>(upgradeType, 0.0));

		for (auto &techType : BWAPI::TechTypes::allTechTypes())
			if (techType.getRace() == util::game::getSelf()->getRace())
				techDesireMap.insert(std::pair<BWAPI::TechType, double>(techType, 0.0));

		for (auto &expansion : BWTA::getBaseLocations())
		{
			double desire = 0.0;
			if (!expansion->isIsland())
			{
				std::vector<double> valueArr;
				std::vector<double> coeffArr;
				valueArr.push_back(1/expansion->getGroundDistance(BWTA::getStartLocation(util::game::getSelf())));
				coeffArr.push_back(1);
				desire = util::calc::normaliseValues(valueArr, coeffArr);
			}
			expansionDesireMap.insert(std::pair<BWTA::BaseLocation*, double>(expansion, desire));
		}

		for (auto &region : BWAPI::Broodwar->getAllRegions())
		{
			attackDesireMap.insert(std::pair<Zone*, double>(MapHelper::getZone(region), 0.0));			
			unitDefenseDesireMap.insert(std::pair<Zone*, double>(MapHelper::getZone(region), 0.0));
			bunkerDefenseDesireMap.insert(std::pair<Zone*, double>(MapHelper::getZone(region), 0.0));
			turretDefenseDesireMap.insert(std::pair<Zone*, double>(MapHelper::getZone(region), 0.0));
		}

		supplyDesire = 1.0;
		expandDesire = 0.0;
	}

	Zone* getMostDesirableAttackZone()
	{
		std::pair<Zone*, double> bestZone = std::pair<Zone*, double>(nullptr, 0.0);
		for (auto &zone : attackDesireMap)
		{
			if (zone.second > bestZone.second)
				bestZone = zone;
		}

		return bestZone.first;
	}

	Zone* getBunkerDefenseTarget()
	{
		std::pair<Zone*, double> bestZone = std::pair<Zone*, double>(nullptr, 0.0);
		for (auto &zone : bunkerDefenseDesireMap)
		{
			if (zone.second > bestZone.second)
				bestZone = zone;
		}

		return bestZone.first;
	}

	Zone* getTurretDefenseTarget()
	{
		std::pair<Zone*, double> bestZone = std::pair<Zone*, double>(nullptr, 0.0);
		for (auto &zone : turretDefenseDesireMap)
		{
			if (zone.second > bestZone.second)
				bestZone = zone;
		}

		return bestZone.first;
	}

	Zone* getUnitDefenseTarget()
	{
		std::pair<Zone*, double> bestZone = std::pair<Zone*, double>(nullptr, 0.0);
		for (auto &zone : unitDefenseDesireMap)
		{
			if (zone.second > bestZone.second)
				bestZone = zone;
		}

		return bestZone.first;
	}

	void setDefendDesire(Zone* const& target, double desireMod)
	{
		for (auto &zone : target->getNeighbourhood())
		{
			int defenseMod = 0;
			for (auto &neighbour : zone->getNeighbourhood())
			{
				if (neighbour->getRegion()->getUnits(BWAPI::Filter::IsOwned && BWAPI::Filter::IsBuilding).size() > 0)
				{
					defenseMod = 1;
					break;
				}
			}
			
			//double defenseMultiplier = zone->getTimesDefended()  / (zone->getRegion()->getUnits(BWAPI::Filter::IsOwned && !BWAPI::Filter::IsBuilding && !BWAPI::Filter::IsWorker).size() + 1);
			unitDefenseDesireMap[zone] = (desireMod + 1) * zone->getTimesDefended() * defenseMod;

			if (!zone->hasBunkerDefense())
				bunkerDefenseDesireMap[zone] = zone->getTimesDefended() * defenseMod;
			else
				bunkerDefenseDesireMap[zone] = 0.0;

			if (!zone->hasTurretDefense())
				turretDefenseDesireMap[zone] = zone->getTimesDefended() * defenseMod;
			else
				turretDefenseDesireMap[zone] = 0.0;
		}
	}

	void updateAttackDesire(Zone* const& target, double desireMod)
	{
		attackDesireMap[target] = desireMod;
	}

	void updateUnitDesireMap()
	{
		for (auto &unit : unitDesireMap)
			unit.second = 0.0;

		for (auto &coalition : CoalitionHelper::getCoalitionset())
		{
			auto targetUnitMap = coalition->getTargetComp().getUnitMap();
			auto currentUnitMap = coalition->getCurrentComp().getUnitMap();
		
			for (auto &unit : targetUnitMap)
				unitDesireMap[unit.first] += 1 + (double)(unit.second - currentUnitMap[unit.first]) / (double)unit.second;
		}
	}	

	BWAPI::UnitType getMostDesirableUnit(BWAPI::UnitType producer)
	{
		updateUnitDesireMap();

		if (producer == BWAPI::UnitTypes::None)
			return BWAPI::UnitTypes::None;

		if (producer == util::game::getSelf()->getRace().getCenter())
			if (util::game::getSelf()->allUnitCount(util::game::getSelf()->getRace().getWorker()) < 70)
				return util::game::getSelf()->getRace().getWorker();

		auto bestUnit = std::pair<BWAPI::UnitType, double>(BWAPI::UnitTypes::None, 0.0);		

		for each(auto &unit in producer.buildsWhat())
		{
			if (util::game::canMakeUnit(unit) && unitDesireMap[unit] > bestUnit.second)// || (unitDesireMap[unit] == bestUnit.second && unit.buildScore() > bestUnit.first.buildScore())))
				bestUnit = std::pair<BWAPI::UnitType, double>(unit, unitDesireMap[unit]);
		}

		return bestUnit.first;
	}

	BWAPI::UnitType getMostDesirableUnit()
	{
		auto bestUnit = std::pair<BWAPI::UnitType, double>(BWAPI::UnitTypes::None, 0.0);
		for each(auto &unit in unitDesireMap)
		{
			if (util::game::canMakeUnit(unit.first) && unit.second > bestUnit.second)
				bestUnit = unit;
		}

		return bestUnit.first;
	}

	BWAPI::UnitType getMostDesirableAddon(BWAPI::UnitType building)
	{
		if (!building.canBuildAddon() || building == BWAPI::UnitTypes::None)
			return BWAPI::UnitTypes::None;		

		auto bestAddon = std::pair<BWAPI::UnitType, double>(BWAPI::UnitTypes::None, 0.0);
		for each(auto &unit in building.buildsWhat())
		{	
			if (!unit.isAddon())
				continue;

			if (util::game::canMakeUnit(unit) && (unitDesireMap[unit] > bestAddon.second || (unitDesireMap[unit] == bestAddon.second && unit.buildScore() > bestAddon.first.buildScore())))
				bestAddon = std::pair<BWAPI::UnitType, double>(unit, unitDesireMap[unit]);
		}
			
		return bestAddon.first;
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
				valueArr.push_back(1/expansion->getGroundDistance(BWTA::getStartLocation(util::game::getSelf())));
				coeffArr.push_back(1.0);
				expansionDesireMap[expansion] = util::calc::normaliseValues(valueArr, coeffArr);
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
		supplyDesire -= (double)unitSupply / util::game::getSelf()->getRace().getCenter().supplyProvided();
	}

	void updateSupplyDesire()
	{
		if (util::game::getSelf()->supplyTotal() >= 400)
		{
			supplyDesire = 0.0;
			return;
		}

		supplyDesire = 1 - ((double)(util::game::getSelf()->supplyTotal() - util::game::getSelf()->supplyUsed())/ util::game::getSelf()->getRace().getCenter().supplyProvided());
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
		expandDesire /= util::game::getSelf()->allUnitCount(util::game::getSelf()->getRace().getCenter());
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

	void setExpansionDesire(BWTA::BaseLocation* const& baseLocation, double desire)
	{
		expansionDesireMap[baseLocation] = desire;
	}

	double getExpansionDesire(BWTA::BaseLocation* const& baseLocation)
	{
		return expansionDesireMap[baseLocation];
	}

	const std::unordered_map<Zone*, double, std::hash<void*>>& getAttackDesireMap()
	{
		return attackDesireMap;
	}

	const std::unordered_map<Zone*, double, std::hash<void*>>& getBunkerDefenseDesireMap()
	{
		return bunkerDefenseDesireMap;
	}

	const std::unordered_map<Zone*, double, std::hash<void*>>& getTurretDefenseDesireMap()
	{
		return turretDefenseDesireMap;
	}

	const std::unordered_map<Zone*, double, std::hash<void*>>& getUnitDefenseDesireMap()
	{
		return unitDefenseDesireMap;
	}
}
