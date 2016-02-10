#include "..\include\DesireHelper.h"
#include "..\include\UtilHelper.h"
#include "..\include\MapHelper.h"

namespace DesireHelper
{
	namespace
	{
		std::unordered_map<BWAPI::UnitType, double> unitDesireMap;
		std::unordered_map<BWAPI::UpgradeType, double> upgradeDesireMap;
		std::unordered_map<BWAPI::TechType, double> techDesireMap;
		std::unordered_map<BWTA::BaseLocation*, double, std::hash<void*>> expansionDesireMap;
		std::unordered_map<MapHelper::Zone*, double, std::hash<void*>> attackDesireMap;
		std::unordered_map<MapHelper::Zone*, double, std::hash<void*>> defendDesireMap;
		
	}

	void initialiseHelper()
	{
		for (auto &unitType : BWAPI::UnitTypes::allUnitTypes())
			unitDesireMap.insert(std::pair<BWAPI::UnitType, double>(unitType, 0.0));

		for (auto &upgradeType : BWAPI::UpgradeTypes::allUpgradeTypes())
			upgradeDesireMap.insert(std::pair<BWAPI::UpgradeType, double>(upgradeType, 0.0));

		for (auto &techType : BWAPI::TechTypes::allTechTypes())
			techDesireMap.insert(std::pair<BWAPI::TechType, double>(techType, 0.0));

		for (auto &expansion : BWTA::getBaseLocations())
			expansionDesireMap.insert(std::pair<BWTA::BaseLocation*, double>(expansion, 0.0));

		for (auto &region : BWAPI::Broodwar->getAllRegions())
		{
			attackDesireMap.insert(std::pair<MapHelper::Zone*, double>(MapHelper::getZone(region), 0.0));
			defendDesireMap.insert(std::pair<MapHelper::Zone*, double>(MapHelper::getZone(region), 0.0));
		}
	}

	void updateUnitDesireMap()
	{
	}

	void updateUpgradeDesireMap()
	{
	}

	void updateTechDesireMap()
	{
	}

	void updateExpansionDesireMap()
	{
	}

	void updateDesireMaps()
	{
		updateUnitDesireMap();
		updateUpgradeDesireMap();
		updateTechDesireMap();
		updateExpansionDesireMap();
	}
}
