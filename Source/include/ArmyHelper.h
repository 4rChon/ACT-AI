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
	void defend();
	void stopScouting();
	bool isDefending();	
	bool isAttacking();
	void addTargetPriority(BWAPI::Unit unit);
	void removeTargetPriority(BWAPI::Unit unit);
	void updateTargetPriority();
	void clearZoneTargets(Zone * zone);
	bool scan(BWAPI::Position target);
	void addScoutedUnit(int unitID, BWAPI::UnitType unitType);
	void removeScoutedUnit(int unitID);
	Composition getScoutedUnits();
	void printPriorityList(int i = 0);
}