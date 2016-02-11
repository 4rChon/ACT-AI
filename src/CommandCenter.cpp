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
	if (unit->isIdle())
		train(unit->getPlayer()->getRace().getWorker());
}