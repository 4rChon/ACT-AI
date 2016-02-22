#pragma once
#include "BWAPI.h"
#include "BWTA.h"
#include "MapHelper.h"
#include <unordered_map>

namespace DesireHelper
{
	void initialiseHelper();
	
	void updateUnitDesireMap();
	void updateUpgradeDesireMap();
	void updateTechDesireMap();
	void updateExpansionDesireMap();
	void updateDesireMaps();
	void updateSupplyDesire(BWAPI::UnitType unitType, bool justDied = false);

	const std::unordered_map<BWAPI::UnitType, double>& getUnitDesireMap();
	const std::unordered_map<BWAPI::UpgradeType, double>& getUpgradeDesireMap();
	const std::unordered_map<BWAPI::TechType, double>& getTechDesireMap();
	const std::unordered_map<BWTA::BaseLocation*, double, std::hash<void*>>& getExpansionDesireMap();
	double getSupplyDesire();	
	double getExpansionDesire(BWTA::BaseLocation* baseLocation);
	const std::unordered_map<MapHelper::Zone*, double, std::hash<void*>>& getAttackDesireMap();
	const std::unordered_map<MapHelper::Zone*, double, std::hash<void*>>& getDefendDesireMap();

	void setExpansionDesire(BWTA::BaseLocation* baseLocation, double desire);
}