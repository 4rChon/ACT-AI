#include "ResourceDepot.h"
#include "EconHelper.h"
#include "TaskHelper.h"
#include "Task.h"
#include "Expand.h"
#include "CreateUnit.h"
#include "DesireHelper.h"

ResourceDepot::ResourceDepot()
{	
	baseLocation = nullptr;
	workers.clear();
	mineralMiners = 0;
	gasMiners = 0;
	expandDesire = 1.0;
	refineryCount = 0;
}

ResourceDepot::ResourceDepot(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
	baseLocation = BWTA::getNearestBaseLocation(unit->getPosition());
	workers.clear();
	mineralMiners = 0;
	gasMiners = 0;
	expandDesire = 0;
	refineryCount = 0;
}

ResourceDepot::~ResourceDepot()
{
	std::cout << "\t\t~ResourceDepot\n";
	for (auto &worker : workers)
	{
		worker->unsetMiningBase();
		worker->getUnit()->stop();
	}
	workers.clear();
	baseLocation = nullptr;	
}

BWTA::BaseLocation* ResourceDepot::getBaseLocation()
{
	return this->baseLocation;
}

Workerset& ResourceDepot::getWorkers()
{
	return workers;
}

int ResourceDepot::getMineralMiners()
{
	return mineralMiners;
}

int ResourceDepot::getGasMiners()
{
	return gasMiners;
}

int ResourceDepot::getRefineryCount()
{
	return refineryCount;
}

void ResourceDepot::act()
{		
	updateExpandDesire();
	updateRefineryCount();
	//temp contents
	if (free)
	{
		if (unit->isIdle())
		{
			if (unit->canBuildAddon())
				unit->buildAddon(BWAPI::UnitTypes::Terran_Comsat_Station);
			train(DesireHelper::getMostDesirableUnit(unit->getType()));
		}
	}
	else
		updateCoalitionStatus();
}

void ResourceDepot::updateRefineryCount()
{
	refineryCount = 0;
	for each (auto &geyser in baseLocation->getGeysers())
	{
		if (geyser->getType().isRefinery() && !geyser->isBeingConstructed())
			refineryCount++;
	}
}

void ResourceDepot::updateExpandDesire()
{
	if (baseLocation->getMinerals().size() > 0)
	{
		if ((double)(mineralMiners) / (2.0 * baseLocation->getMinerals().size()) >= 1)
			expandDesire = 1;
	}
	else
		expandDesire = 1;
}

double ResourceDepot::getExpandDesire()
{
	return expandDesire;
}

bool ResourceDepot::isMineralSaturated()
{
	return mineralMiners >= 2.5 * baseLocation->getMinerals().size();
}

bool ResourceDepot::isGasSaturated()
{	
	return gasMiners >= 3.0 * refineryCount;
}

void ResourceDepot::addMineralWorker(Worker* worker)
{	
	worker->gather((*baseLocation->getMinerals().begin()));
	workers.insert(worker);
	mineralMiners++;
}

void ResourceDepot::addGasWorker(Worker* worker)
{
	worker->gather((*baseLocation->getGeysers().begin()));
	workers.insert(worker);
	gasMiners++;
}

void ResourceDepot::removeWorker(Worker* worker)
{
	if (workers.count(worker) > 0)
	{
		if (worker->isMiningGas())
			gasMiners--;
		else
			mineralMiners--;
		workers.erase(worker);
	}
}

bool ResourceDepot::addGeyser(Worker* worker)
{
	for each (auto &geyser in baseLocation->getGeysers())
	{
		if (!geyser->getType().isRefinery())
			return worker->build(BWAPI::Broodwar->self()->getRace().getRefinery(), &geyser->getTilePosition());
	}
	return false;
}
