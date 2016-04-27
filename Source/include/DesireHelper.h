#pragma once
#include "BWAPI.h"
#include "BWTA.h"
#include "MapHelper.h"
#include "Zone.h"
#include "Composition.h"
#include <unordered_map>

namespace DesireHelper
{
	void initialiseHelper();

	Zone* getMostDesirableAttackZone();
	Zone* getMostDesirableDefenseZone(bool buildingBunker);
	void setDefendDesire(Zone * target, double desireMod);
	void updateAttackDesire(Zone * target, double desireMod);
	
	void updateUnitDesireMap();
	BWAPI::UnitType getMostDesirableUnit(BWAPI::UnitType producer);
	BWAPI::UnitType getMostDesirableUnit();
	BWAPI::UnitType getMostDesirableAddon(BWAPI::UnitType building);
	void updateUpgradeDesireMap();
	void updateTechDesireMap();
	void updateExpansionDesireMap();
	void updateDesireMaps();
	void updateSupplyDesire(BWAPI::UnitType unitType, bool justDied = false);
	void updateSupplyDesire();
	void updateExpandDesire();

	const std::unordered_map<BWAPI::UnitType, double>& getUnitDesireMap();
	const std::unordered_map<BWAPI::UpgradeType, double>& getUpgradeDesireMap();
	const std::unordered_map<BWAPI::TechType, double>& getTechDesireMap();
	const std::unordered_map<BWTA::BaseLocation*, double, std::hash<void*>>& getExpansionDesireMap();
	double getSupplyDesire();
	double getExpandDesire();
	double getExpansionDesire(BWTA::BaseLocation* baseLocation);
	const std::unordered_map<Zone*, double, std::hash<void*>>& getAttackDesireMap();
	const std::unordered_map<Zone*, double, std::hash<void*>>& getDefendDesireMap();

	BWTA::BaseLocation * getBestExpansionLocation();

	void setExpansionDesire(BWTA::BaseLocation* baseLocation, double desire);
}