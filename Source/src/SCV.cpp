#include "SCV.h"
#include "AgentHelper.h"
#include "EconHelper.h"
#include "DesireHelper.h"

SCV::SCV(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
}

void SCV::act()
{	
	//temp contents
	if (repair())
		return;

	if (DesireHelper::getSupplyDesire() > 0.6
		&& EconHelper::haveMoney(BWAPI::Broodwar->self()->getRace().getSupplyProvider()))
	{
		if (build(BWAPI::Broodwar->self()->getRace().getSupplyProvider(), nullptr))
		{
			DesireHelper::updateSupplyDesire(BWAPI::Broodwar->self()->getRace().getSupplyProvider());
			return;
		}
	}

	if (unit->isIdle())
	{
		bool mining = false;
		auto resourceDepots = AgentHelper::getResourceDepots();
		for (auto &base : resourceDepots)
		{
			if (!base->isMineralSaturated())
			{
				setMiningBase(base, false);
				mining = true;
				return;
			}
		}
	}
}

bool SCV::repair(BWAPI::Unit damagedUnit)
{
	int costsGas = damagedUnit->getType().gasPrice() > 0;
	if (EconHelper::haveMoney(1, costsGas))
		return unit->repair(damagedUnit, true);
	return false;
}

bool SCV::repair()
{
	auto repairSet = unit->getUnitsInRadius(unit->getType().sightRange(), BWAPI::Filter::IsOwned && BWAPI::Filter::HP_Percent < 100);
	for (auto &damagedUnit : repairSet)
		return(repair(damagedUnit));
	return false;
}
