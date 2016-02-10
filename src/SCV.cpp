#include "..\include\SCV.h"
#include "..\include\AgentHelper.h"
#include "..\include\EconHelper.h"

SCV::SCV(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
}

void SCV::act()
{
	if (repair())
		return;

	if (BWAPI::Broodwar->self()->supplyTotal() != 200 && BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() <= 2 && BWAPI::Broodwar->self()->incompleteUnitCount(BWAPI::Broodwar->self()->getRace().getSupplyProvider()) < 1)
		if(build(BWAPI::Broodwar->self()->getRace().getSupplyProvider(), nullptr))
			return;

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

	if (!mining)
		expand();
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
