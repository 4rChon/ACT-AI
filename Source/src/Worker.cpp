#include "..\include\Worker.h"
#include "..\include\ResourceDepot.h"
#include "..\include\AgentHelper.h"
#include "..\include\EconHelper.h"
#include "..\include\DesireHelper.h"
#include "..\include\UtilHelper.h"
#include "..\include\Task.h"
#include "..\include\CreateUnit.h"
#include "BWAPI.h"

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
	//temp contents
	if (unit->isIdle())
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
	if (EconHelper::haveMoney(building))
	{
		std::cout << "Building : " << building.getName().c_str() << "\n";		
		unsetMiningBase();
		if (!desiredPosition)
		{
			if (miningBase != nullptr)
				desiredPosition = &miningBase->getBaseLocation()->getTilePosition();
			else
				desiredPosition = &unit->getTilePosition();
		}
		
		auto buildLocation = BWAPI::Broodwar->getBuildLocation(building, *desiredPosition);
		if(!unit->build(building, buildLocation))
			return false;
		EconHelper::addDebt(building.mineralPrice(), building.gasPrice());
		BWAPI::Broodwar->registerEvent(
		[this, building](BWAPI::Game*)
		{
			EconHelper::subtractDebt(building.mineralPrice(), building.gasPrice());
			if (BWAPI::Broodwar->getLastError() == BWAPI::Errors::Insufficient_Gas
				|| BWAPI::Broodwar->getLastError() == BWAPI::Errors::Insufficient_Minerals
				|| BWAPI::Broodwar->getLastError() == BWAPI::Errors::Unbuildable_Location
				|| !this->getUnit()->exists())
			{
				BWAPI::Broodwar->setLastError();
				if (building == BWAPI::Broodwar->self()->getRace().getSupplyProvider())
					DesireHelper::updateSupplyDesire(building, true);
			}				
			else if (task)
			{
				std::cout << task->getName() << " Complete : decrementing unitCount\n";				
				((CreateUnit*)task)->decrementUnitCount();
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
	if (EconHelper::haveMoney(BWAPI::Broodwar->self()->getRace().getCenter()))
	{		
		BWTA::BaseLocation* bestLocation = (*DesireHelper::getExpansionDesireMap().begin()).first;
		double bestScore = (*DesireHelper::getExpansionDesireMap().begin()).second;
		for (auto &expansion : DesireHelper::getExpansionDesireMap())
			if (bestScore < expansion.second)
			{
				bestLocation = expansion.first;
				bestScore = expansion.second;
			}

		if (unit->move(bestLocation->getPosition()))
		{
			unsetMiningBase();
			if (unit->build(unit->getPlayer()->getRace().getCenter(), bestLocation->getTilePosition()))
			{								
				DesireHelper::setExpansionDesire(bestLocation, 0.0);
				task->succeed();
				return true;
			}
		}
		else
			return false;
	}
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