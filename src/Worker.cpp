#include "..\include\Worker.h"
#include "..\include\ResourceDepot.h"
#include "..\include\AgentHelper.h"
#include "..\include\EconHelper.h"
#include "BWAPI.h"

Worker::Worker()
{
	miningBase = nullptr;
	gasMiner = false;
}

Worker::Worker(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
	miningBase = nullptr;
	gasMiner = false;
}

Worker::~Worker()
{
	std::cout << "\t~Worker\n";
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
	if (BWAPI::Broodwar->self()->getRace() != BWAPI::Races::Zerg && BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() <= 2 && BWAPI::Broodwar->self()->minerals() >= 100 && BWAPI::Broodwar->self()->incompleteUnitCount(BWAPI::Broodwar->self()->getRace().getSupplyProvider()) < 1)
	{
		build(BWAPI::Broodwar->self()->getRace().getSupplyProvider(), nullptr);
		return;
	}

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

bool Worker::build(BWAPI::UnitType building, BWAPI::TilePosition* desiredPosition)
{	
	if (EconHelper::haveMoney(building))
	{
		unsetMiningBase();
		if (!desiredPosition)
		{
			if (miningBase != nullptr)
				desiredPosition = &miningBase->getBaseLocation()->getTilePosition();
			else
				desiredPosition = &unit->getTilePosition();
		}

		auto buildLocation = BWAPI::Broodwar->getBuildLocation(building, *desiredPosition);
		return unit->build(building, buildLocation);
	}
	return false;
}

bool Worker::expand()
{	
	if (EconHelper::haveMoney(BWAPI::Broodwar->self()->getRace().getCenter()))
	{
		if (AgentHelper::getCandidateBases().size() > 0)
		{
			unsetMiningBase();
			auto expandLocation = (*AgentHelper::getCandidateBases().begin());
			for (auto base : AgentHelper::getCandidateBases())
				if (expandLocation->getTilePosition().getDistance(unit->getTilePosition()) > (base->getTilePosition().getDistance(unit->getTilePosition())))
					expandLocation = base;

			if (unit->move(expandLocation->getPosition()))
			{
				if (unit->build(unit->getPlayer()->getRace().getCenter(), expandLocation->getTilePosition()))
				{
					AgentHelper::getCandidateBases().erase(expandLocation);
					return true;
				}
			}
		}
	}
	return false;
}

bool Worker::gather(BWAPI::Unit target)
{	
	return unit->gather(target);
}

void Worker::debugInfo()
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