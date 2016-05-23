#pragma once
#include <BWAPI.h>
#include "MapHelper.h"
#include "Composition.h"
#include "Zone.h"

namespace ArmyHelper
{
	void initialiseHelper();
	void updateArmyMovement();
	void scout();
	void attack();
	void stopScouting();
	void addTargetPriority(BWAPI::Unit unit);
	void removeTargetPriority(BWAPI::Unit unit);
	void updateTargetPriority();
	void clearZoneTargets(Zone* const& zone);
	void addScoutedUnit(int unitID, BWAPI::UnitType unitType);
	void removeScoutedUnit(int unitID);
	Composition getScoutedUnits();
	void printPriorityList(int i = 0);
}