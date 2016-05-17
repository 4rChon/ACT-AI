#include "ComsatStation.h"

ComsatStation::ComsatStation(BWAPI::Unit unit)
	: lastScanned(-1)
	, lastScannedPos(-1, -1)
{
	this->unit = unit;
	unitID = unit->getID();
}

ComsatStation::~ComsatStation()
{
}

void ComsatStation::updateFreeActions()
{
	pollCoalitions();

	for (auto &unit : BWAPI::Broodwar->enemy()->getUnits())
	{
		if (unit->exists())
		{
			if ((unit->isCloaked() || unit->isBurrowed()) && !unit->isDetected())	//if can't see unit...
			{
				if (!scan(unit->getPosition())) break;								//try scanning.. if scan fails... stop trying to scan
			}
		}
	}

}

void ComsatStation::act()
{
	if (!exists())
		return;

	updateActions();

	if (free)
		updateFreeActions();
	else
		updateBoundActions();
}

bool ComsatStation::scan(BWAPI::Position target)
{
	if (BWAPI::Broodwar->getFrameCount() - lastScanned > 10 * 24 && unit->getEnergy() >= 50)
	{
		if(useAbility(BWAPI::TechTypes::Scanner_Sweep, target))
		{
			lastScanned = BWAPI::Broodwar->getFrameCount();
			lastScannedPos = target;
			return true;
		}
	}

	return false;
}