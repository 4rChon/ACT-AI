#include "Worker.h"
#include "ResourceDepot.h"
#include "AgentHelper.h"
#include "EconHelper.h"
#include "DesireHelper.h"
#include "UtilHelper.h"
#include "MapHelper.h"
#include "Task.h"
#include "CreateUnit.h"
#include <BWAPI.h>

Worker::Worker()
	: gasMiner(false)
	, miningBase(nullptr)
	, reservedMinerals(0)
	, reservedGas(0)
{
}

Worker::Worker(BWAPI::Unit unit)
	: gasMiner(false)
	, miningBase(nullptr)
	, reservedMinerals(0)
	, reservedGas(0)
{
	this->unit = unit;
	unitID = unit->getID();
}

Worker::~Worker()
{
	releaseResources();
	unsetMiningBase();
}

void Worker::unsetMiningBase()
{	
	if (miningBase)
		miningBase->removeWorker(this);
	miningBase = nullptr;
	gasMiner = false;
}

void Worker::setMiningBase(ResourceDepot* miningBase, bool gasMiner)
{
	unsetMiningBase();

	if (gasMiner)
		miningBase->addGasWorker(this);		
	else
		miningBase->addMineralWorker(this);

	this->gasMiner = gasMiner;
	this->miningBase = miningBase;
}

void Worker::updateFreeActions()
{
	pollCoalitions();

	if (repair())
		return;

	if (util::game::getSelf()->getRace() != BWAPI::Races::Zerg
		&& DesireHelper::getSupplyDesire() >= 0.6
		&& EconHelper::haveMoney(util::game::getSelf()->getRace().getSupplyProvider()))
	{
		BWAPI::TilePosition* baseLocation = nullptr;

		if (miningBase)
			baseLocation = &miningBase->getBaseLocation()->getTilePosition();

		if (build(util::game::getSelf()->getRace().getSupplyProvider(), baseLocation))
			DesireHelper::updateSupplyDesire(util::game::getSelf()->getRace().getSupplyProvider());			
	}

	if (unit->isIdle())
	{
		if (defend())
			return;

		auto resourceDepots = AgentHelper::getResourceDepots();
		auto base = EconHelper::getLeastSaturatedBase();

		if (base && base->getBaseLocation() && base->getUnit()->exists())
		{
			setMiningBase(base, !base->isGasSaturated());
			return;
		}
	}
}

void Worker::act()
{
	if (!exists())
		return;

	updateActions();

	if (free)
		updateFreeActions();
	else
		updateBoundActions();
}

bool Worker::build(BWAPI::UnitType building, BWAPI::TilePosition* desiredPosition)
{	
	const BuildingPlacer* bp = MapHelper::getBuildingPlacer();	
	if (!BWAPI::Broodwar->canMake(building, unit))
		return false;

	if (EconHelper::haveMoney(building) && !unit->isConstructing())
	{
		if (reservedMinerals + reservedGas == 0)
			reserveResources(building.mineralPrice(), building.gasPrice());
	
		if (!desiredPosition)
		{
			if (miningBase != nullptr)
			{
				desiredPosition = &miningBase->getBaseLocation()->getTilePosition();
				if(!desiredPosition)
					&unit->getTilePosition();
			}
			else
				desiredPosition = &unit->getTilePosition();
		}

		if (!BWAPI::Broodwar->isVisible(*desiredPosition))
			return move((BWAPI::Position)*desiredPosition);		

		BWAPI::TilePosition buildLocation;
		if (!building.isResourceDepot())
			buildLocation = bp->getBuildLocationNear(*desiredPosition, building);
		else
			buildLocation = *desiredPosition;

		if (unit->build(building, buildLocation))
		{		
			if (building == util::game::getSelf()->getRace().getSupplyProvider())
				DesireHelper::updateSupplyDesire(building);
			unsetMiningBase();
			BWAPI::Broodwar->registerEvent([this, building](BWAPI::Game*)
			{
				releaseResources();
				if (this->getUnit()->getOrder() != BWAPI::Orders::ConstructingBuilding
					|| !this->getUnit()->exists())
				{
					if (building == util::game::getSelf()->getRace().getSupplyProvider())
						DesireHelper::updateSupplyDesire(building, true);
					if (task)
					{
						if (task->getType() == EXP)
							task->fail();
					}
				}
				else if (task)
				{
					if (task->getType() == CRU)
						((CreateUnit*)task)->decrementUnitCount();
					if (task->getType() == EXP)
						task->succeed();
				}				
			},
				[this](BWAPI::Game*) {return !getUnit()->exists() || this->getUnit()->getOrder() == BWAPI::Orders::ConstructingBuilding || !this->getUnit()->isConstructing(); },
				1);
		}

		if (unit->isConstructing())
			return true;		
		releaseResources();
	}	
	return false;
}

bool Worker::expand()
{
	BWTA::BaseLocation* bestLocation = DesireHelper::getBestExpansionLocation();
	if (!bestLocation)
	{
		task->fail();
		return false;
	}
	return build(util::game::getSelf()->getRace().getCenter(), &bestLocation->getTilePosition());
}

void Worker::reserveResources(int minerals, int gas)
{
	EconHelper::addDebt(minerals, gas);
	reservedMinerals = minerals;
	reservedGas = gas;
}

void Worker::releaseResources()
{
	EconHelper::subtractDebt(reservedMinerals, reservedGas);
	reservedMinerals = 0;
	reservedGas = 0;
}

bool Worker::repair(BWAPI::Unit damagedUnit)
{
	int costsGas = damagedUnit->getType().gasPrice() > 0;
	if (EconHelper::haveMoney(1, costsGas))
		return unit->repair(damagedUnit, true);
	return false;
}

bool Worker::repair(BWAPI::Region region)
{
	auto repairSet = region->getUnits(BWAPI::Filter::IsOwned && BWAPI::Filter::HP_Percent < 100 && BWAPI::Filter::IsMechanical);
	for (auto &damagedUnit : repairSet)
		return(repair(damagedUnit));
	return false;
}

bool Worker::repair()
{
	auto repairSet = unit->getUnitsInRadius(unit->getType().sightRange(), BWAPI::Filter::IsOwned && BWAPI::Filter::HP_Percent < 100 && BWAPI::Filter::IsMechanical);
	for (auto &damagedUnit : repairSet)
		return(repair(damagedUnit));
	return false;
}

bool Worker::defend(BWAPI::PositionOrUnit target)
{
	if (target.isPosition())
	{
		auto zone = MapHelper::getZone(BWAPI::Broodwar->getRegionAt(target.getPosition()));
		if (zone->getTimesDefended() > 0 && zone->getFriendScore() > 0)
		{
			if (!zone->hasBunkerDefense() && BWAPI::Broodwar->self()->incompleteUnitCount(BWAPI::UnitTypes::Terran_Bunker) == 0)
			{
				return build(BWAPI::UnitTypes::Terran_Bunker, &BWAPI::TilePosition(target.getPosition()));
			}
			else if (!zone->hasTurretDefense() && BWAPI::Broodwar->self()->incompleteUnitCount(BWAPI::UnitTypes::Terran_Missile_Turret) == 0)
			{
				return build(BWAPI::UnitTypes::Terran_Missile_Turret, &BWAPI::TilePosition(target.getPosition()));
			}
			else
			{
				return repair(BWAPI::Broodwar->getRegionAt(target.getPosition()));
			}
		}
	}
	else if(target.getUnit()->getType().isMechanical())
		return repair(target.getUnit());
	return false;
}

bool Worker::defend()
{
	auto defenseZone = DesireHelper::getBunkerDefenseTarget();
	if(!defenseZone)
		defenseZone = DesireHelper::getTurretDefenseTarget();

	if (defenseZone)
		return defend(defenseZone->getRegion()->getCenter());

	return false;
}

void Worker::debugInfo() const
{
	std::cout << "\tID  : " << unitID << "\n";
	std::cout << "\tUnit: " << unit->getType() << "\n";
	if (gasMiner)
		std::cout << "\tgasMiner: true\n";
	else
		std::cout << "\tgasMiner: false\n";
	if (!miningBase)
		std::cout << "\tMiningBase  : " << "nullptr" << "\n";
	else
		std::cout << "\tMiningBase: " << miningBase->getID() << "\n";
}