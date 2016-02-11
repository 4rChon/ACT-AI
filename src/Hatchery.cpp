#include "..\include\Hatchery.h"
#include "..\include\EconHelper.h"
#include "..\include\TaskHelper.h"
#include "..\include\Task.h"
#include "..\include\Expand.h"

Hatchery::Hatchery(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
	baseLocation = BWTA::getNearestBaseLocation(unit->getPosition());
}

void Hatchery::act()
{	
	updateExpandDesire();

	//temp contents
	if (unit->isIdle())
		if (!train(BWAPI::UnitTypes::Zerg_Drone))
			unit->train(BWAPI::UnitTypes::Zerg_Overlord);

	if (unit->getType() != BWAPI::UnitTypes::Zerg_Hive)
		morph(*unit->getType().buildsWhat().begin());
		
	if(unit->getType() != BWAPI::UnitTypes::Zerg_Hatchery)
		upgrade(*unit->getType().upgradesWhat().begin());
}

bool Hatchery::upgrade(BWAPI::UpgradeType upgradeType)
{
	if (unit->canUpgrade(upgradeType) && EconHelper::haveMoney(upgradeType))
		return unit->upgrade(upgradeType);
	return false;
}
