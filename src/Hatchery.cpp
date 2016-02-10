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
	if(!train(BWAPI::UnitTypes::Zerg_Drone))
		unit->train(BWAPI::UnitTypes::Zerg_Overlord);
	morph();
	upgrade(*unit->getType().upgradesWhat().begin());
}

bool Hatchery::train(BWAPI::UnitType unitType)
{
	if (EconHelper::haveMoney(unitType) && EconHelper::haveSupply(unitType))
		return this->unit->train(unitType);
	return false;	
}

bool Hatchery::morph()
{
	if (unit->getType() != BWAPI::UnitTypes::Zerg_Hive)
	{
		BWAPI::UnitType unitMorph = *unit->getType().buildsWhat().begin();
		if (EconHelper::haveMoney(unitMorph))
			return(unit->morph(unitMorph));
	}
	return false;
}

bool Hatchery::upgrade(BWAPI::UpgradeType upgradeType)
{
	if(EconHelper::haveMoney(upgradeType))
		return unit->upgrade(upgradeType);
	return false;
}
