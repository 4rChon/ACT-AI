#include "..\include\Hatchery.h"
#include "..\include\EconHelper.h"

Hatchery::Hatchery(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
	baseLocation = BWTA::getNearestBaseLocation(unit->getPosition());
}

void Hatchery::act()
{	
	if (unit->isIdle())
		if (!train(BWAPI::UnitTypes::Zerg_Drone))
			unit->train(BWAPI::UnitTypes::Zerg_Overlord);
	if(unit->getType() != BWAPI::UnitTypes::Zerg_Hive)
		morph(*unit->getType().buildsWhat().begin());
	upgrade(*unit->getType().upgradesWhat().begin());
}
