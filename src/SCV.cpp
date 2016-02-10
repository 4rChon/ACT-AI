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
	if (!unit->isRepairing())
	{		
		auto repairSet = unit->getUnitsInRadius(unit->getType().sightRange(), BWAPI::Filter::IsOwned && BWAPI::Filter::HP_Percent < 100);
		if (repairSet.size())
		{			
			for (auto &damagedUnit : repairSet)
			{
				std::cout << "found repair target : " << damagedUnit->getID() << "\n";
				if (repair(damagedUnit))
				{
					std::cout << "repairing : " << damagedUnit->getID() << "\n";
					return;
				}
			}			
		}		
	}

	if (BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() <= 2 && BWAPI::Broodwar->self()->incompleteUnitCount(BWAPI::Broodwar->self()->getRace().getSupplyProvider()) < 1)
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
	{
		expand(AgentHelper::getCandidateBases());
		return;
	}
}
bool SCV::repair(BWAPI::Unit damagedUnit)
{
	if(EconHelper::haveMoney(1, damagedUnit->getType().gasPrice() > 0))
		return unit->repair(damagedUnit, true);
	return false;
}
