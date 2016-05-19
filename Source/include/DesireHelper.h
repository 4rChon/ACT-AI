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
	Zone* getBunkerDefenseTarget();
	Zone* getTurretDefenseTarget();
	Zone* getUnitDefenseTarget();

	const std::unordered_map<BWAPI::UnitType, double>& getUnitDesireMap();
	const std::unordered_map<BWAPI::UpgradeType, double>& getUpgradeDesireMap();
	const std::unordered_map<BWAPI::TechType, double>& getTechDesireMap();
	const std::unordered_map<BWTA::BaseLocation*, double, std::hash<void*>>& getExpansionDesireMap();

	const std::unordered_map<Zone*, double, std::hash<void*>>& getBunkerDefenseDesireMap();
	const std::unordered_map<Zone*, double, std::hash<void*>>& getAttackDesireMap();
	const std::unordered_map<Zone*, double, std::hash<void*>>& getTurretDefenseDesireMap();
	const std::unordered_map<Zone*, double, std::hash<void*>>& getUnitDefenseDesireMap();

	double getSupplyDesire();
	double getExpandDesire();
	double getExpansionDesire(BWTA::BaseLocation* const& baseLocation);

	BWTA::BaseLocation* getBestExpansionLocation();
	BWAPI::UnitType getMostDesirableUnit(BWAPI::UnitType producer);
	BWAPI::UnitType getMostDesirableUnit();
	BWAPI::UnitType getMostDesirableAddon(BWAPI::UnitType building);


	void setExpansionDesire(BWTA::BaseLocation* const& baseLocation, double desire);
	void setDefendDesire(Zone* const& target, double desireMod);
	void updateAttackDesire(Zone* const& target, double desireMod);

	void updateUnitDesireMap();
	void updateUpgradeDesireMap();
	void updateTechDesireMap();
	void updateExpansionDesireMap();
	void updateDesireMaps();
	void updateSupplyDesire(BWAPI::UnitType unitType, bool justDied = false);
	void updateSupplyDesire();
	void updateExpandDesire();
}