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
			//if can't see unit...
			if ((unit->isCloaked() || unit->isBurrowed()) && !unit->isDetected())
			{
				//try scanning
				if (!scan(unit->getPosition()))
				{
					//if scan fails... stop looking for units to scan
					break;
				}
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