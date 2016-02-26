#pragma once
#include <BWAPI.h>

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
}