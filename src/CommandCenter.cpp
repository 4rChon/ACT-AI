#include "..\include\CommandCenter.h"
#include "..\include\EconHelper.h"
#include "..\include\TaskHelper.h"
#include "..\include\Task.h"
#include "..\include\Expand.h"

CommandCenter::CommandCenter(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
	baseLocation = BWTA::getNearestBaseLocation(unit->getPosition());
}

void CommandCenter::act()
{
	updateExpandDesire();

	//temp contents
	buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
	if (unit->isIdle())
		train(unit->getPlayer()->getRace().getWorker());
}