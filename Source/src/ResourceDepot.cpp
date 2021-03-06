#include "ResourceDepot.h"
#include "EconHelper.h"
#include "TaskHelper.h"
#include "Task.h"
#include "Expand.h"
#include "CreateUnit.h"
#include "DesireHelper.h"

ResourceDepot::ResourceDepot()
	: baseLocation(nullptr)
	, mineralMiners(0)
	, gasMiners(0)
	, mineralSaturation(0)
	, gasSaturation(0)
	, refineryCount(0)
{
	workers.clear();
}

ResourceDepot::ResourceDepot(BWAPI::Unit unit)
	: baseLocation(BWTA::getNearestBaseLocation(unit->getPosition()))
	, mineralMiners(0)
	, gasMiners(0)
	, mineralSaturation(0)
	, gasSaturation(0)
	, refineryCount(0)
{
	this->unit = unit;
	unitID = unit->getID();
	workers.clear();
}

ResourceDepot::~ResourceDepot()
{
	std::cout << "\t\t~ResourceDepot\n";
	for (auto &worker : workers)
	{
		worker->unsetMiningBase();
		worker->getUnit()->stop();
	}
}

void ResourceDepot::updateFreeActions()
{
	pollCoalitions();
	if (unit->isIdle())
	{
		if (buildAddon(DesireHelper::getMostDesirableAddon(unit->getType())))
			return;
		
		train(DesireHelper::getMostDesirableUnit(unit->getType()));
	}
}

void ResourceDepot::act()
{	
	if (!exists())
		return;

	updateActions();
	updateSaturation();
	updateRefineryCount();
	
	if (free)
		updateFreeActions();
	else
		updateBoundActions();
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

void ResourceDepot::updateSaturation()
{
	if (baseLocation->getMinerals().size() > 0)
		mineralSaturation = (double)mineralMiners / (2.0 * baseLocation->getMinerals().size());
	else
		mineralSaturation = 1;

	updateRefineryCount();
	if (refineryCount > 0)
		gasSaturation = (double)gasMiners / (3.0 * refineryCount);
	else
		gasSaturation = 1;
}

bool ResourceDepot::isMineralSaturated()
{
	if (baseLocation->getMinerals().size() == 0)
		return true;
	return mineralSaturation >= 1;
}

bool ResourceDepot::isGasSaturated()
{	
	if (refineryCount == 0)
		return true;
	return gasSaturation >= 1;
}

void ResourceDepot::addMineralWorker(Worker* const& worker)
{	
	if (baseLocation->getMinerals().size() > 0)
	{
		worker->gather((*baseLocation->getMinerals().begin()));
		workers.insert(worker);
		mineralMiners++;
	}
}

void ResourceDepot::addGasWorker(Worker* const& worker)
{
	if (baseLocation->getGeysers().size() > 0)
	{
		worker->gather((*baseLocation->getGeysers().begin()));
		workers.insert(worker);
		gasMiners++;
	}
}

void ResourceDepot::removeWorker(Worker* const& worker)
{
	if (baseLocation == nullptr)
		return;
	if (workers.count(worker) > 0)
	{
		if (worker->isMiningGas())
			gasMiners--;
		else
			mineralMiners--;
		workers.erase(worker);
	}
}

bool ResourceDepot::addGeyser(Worker* const& worker)
{
	for (auto &geyser : baseLocation->getGeysers())
	{
		if (!geyser->getType().isRefinery())
			return worker->build(util::game::getSelf()->getRace().getRefinery(), &geyser->getTilePosition());
	}
	return false;
}
