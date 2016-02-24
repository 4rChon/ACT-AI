#include "Worker.h"
#include "ResourceDepot.h"
#include "AgentHelper.h"
#include "EconHelper.h"
#include "DesireHelper.h"
#include "UtilHelper.h"
#include "Task.h"
#include "CreateUnit.h"
#include <BWAPI.h>

Worker::Worker()
{
	gasMiner = false;
	miningBase = nullptr;
}

Worker::Worker(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
	miningBase = nullptr;
	movingToBuild = false;
	gasMiner = false;
}

Worker::~Worker()
{
	/*std::cout << "\t~Worker\n";*/
	unsetMiningBase();
}

void Worker::unsetMiningBase()
{	
	if (miningBase)
		miningBase->removeWorker(this);
	this->miningBase = nullptr;
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

bool Worker::isMiningGas() const
{
	return gasMiner;
}

ResourceDepot* Worker::getMiningBase() const
{
	return miningBase;
}

void Worker::act()
{
	if (BWAPI::Broodwar->self()->getRace() != BWAPI::Races::Zerg
		&& DesireHelper::getSupplyDesire() >= 0.6
		&& EconHelper::haveMoney(BWAPI::Broodwar->self()->getRace().getSupplyProvider()))
	{
		//int amount = DesireHelper::getSupplyDesire() / 0.6;
		if (build(BWAPI::Broodwar->self()->getRace().getSupplyProvider()))
		{
			//for (int i = 0; i < amount; i++)
			DesireHelper::updateSupplyDesire(BWAPI::Broodwar->self()->getRace().getSupplyProvider());
			return;
		}
	}
	//temp contents
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

			if (!base->isMineralSaturated())
			{
				setMiningBase(base, false);
				mining = true;
				return;
			}
		}		
	}
}

bool Worker::build(BWAPI::UnitType building, BWAPI::TilePosition* desiredPosition)
{	
	if (EconHelper::haveMoney(building) && !unit->isConstructing())
	{		
		std::cout << "Building : " << building.getName().c_str() << "\n";		
		if (!desiredPosition)
		{
			if (miningBase != nullptr)
				desiredPosition = &miningBase->getBaseLocation()->getTilePosition();
			else
				desiredPosition = &unit->getTilePosition();
		}

		if (!BWAPI::Broodwar->isVisible(*desiredPosition))
			return move((BWAPI::Position)*desiredPosition);

		auto buildLocation = BWAPI::Broodwar->getBuildLocation(building, *desiredPosition);
		if(!unit->build(building, buildLocation))
			return false;
		unsetMiningBase();
		EconHelper::addDebt(building.mineralPrice(), building.gasPrice());
		BWAPI::Broodwar->registerEvent(
		[this, building](BWAPI::Game*)
		{
			std::cout << "Last Error: " << BWAPI::Broodwar->getLastError().c_str() << "\n";
			EconHelper::subtractDebt(building.mineralPrice(), building.gasPrice());
			if (BWAPI::Broodwar->getLastError() == BWAPI::Errors::Insufficient_Gas
				|| BWAPI::Broodwar->getLastError() == BWAPI::Errors::Insufficient_Minerals
				|| BWAPI::Broodwar->getLastError() == BWAPI::Errors::Unbuildable_Location
				|| BWAPI::Broodwar->getLastError() == BWAPI::Errors::Insufficient_Space
				|| !this->getUnit()->exists())
			{
				BWAPI::Broodwar->setLastError();
				if (building == BWAPI::Broodwar->self()->getRace().getSupplyProvider())
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
		[this, building, desiredPosition](BWAPI::Game*)
		{
			return !this->getUnit()->exists() || !this->getUnit()->isConstructing();
		},
		1);
		return unit->isConstructing();
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
	return build(BWAPI::Broodwar->self()->getRace().getCenter(), &bestLocation->getTilePosition());			
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