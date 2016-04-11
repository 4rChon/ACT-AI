#include "SCV.h"
#include "AgentHelper.h"
#include "EconHelper.h"
#include "DesireHelper.h"

SCV::SCV(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
}

void SCV::updateFreeActions()
{
	pollCoalitions();

	if (repair())
		return;

	if (DesireHelper::getSupplyDesire() > 0.6
		&& EconHelper::haveMoney(BWAPI::UnitTypes::Terran_Supply_Depot))
	{
		if (miningBase)
			build(util::getSelf()->getRace().getSupplyProvider(), &miningBase->getBaseLocation()->getTilePosition());
		else
			build(util::getSelf()->getRace().getSupplyProvider());
	}

	if (unit->isIdle())
	{
		bool mining = false;
		auto resourceDepots = AgentHelper::getResourceDepots();
		for (auto &base : resourceDepots)
		{
			if (!base->isGasSaturated())
			{
				setMiningBase(base, true);
				mining = true;
				return;
			}
		}

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

	if (defend(BWAPI::Position(util::getSelf()->getStartLocation())))
		return;
}

void SCV::act()
{	
	if (free)
		updateFreeActions();		
	else
		updateBoundActions();
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

bool SCV::defend(BWAPI::PositionOrUnit target)
{
	return false;/*build(BWAPI::UnitTypes::Terran_Bunker, &BWAPI::TilePosition(BWTA::getNearestChokepoint((target.getPosition()))->getCenter()));*/
	//build(BWAPI::UnitTypes::Terran_Missile_Turret, &BWAPI::TilePosition(target.getPosition()));
}