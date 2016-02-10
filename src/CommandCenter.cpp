#include "..\include\CommandCenter.h"
#include "..\include\EconHelper.h"

CommandCenter::CommandCenter(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
	baseLocation = BWTA::getNearestBaseLocation(unit->getPosition());
}

void CommandCenter::act()
{
	buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
	train(unit->getPlayer()->getRace().getWorker());
}

bool CommandCenter::buildAddon(BWAPI::UnitType addon)
{
	if(EconHelper::haveMoney(addon))
		return unit->buildAddon(addon);
	return false;
}
