#include "..\include\ResourceDepot.h"
#include "..\include\EconHelper.h"
#include "..\include\TaskHelper.h"
#include "..\include\Task.h"
#include "..\include\Expand.h"

ResourceDepot::ResourceDepot()
{	
	baseLocation = nullptr;
	workers.clear();
	mineralMiners = 0;
	gasMiners = 0;
	expandDesire = 1.0;
	geyserCount = 0;
}

ResourceDepot::ResourceDepot(BWAPI::Unit unit)
{
	this->unit = unit;
	unitID = unit->getID();
	baseLocation = BWTA::getNearestBaseLocation(unit->getPosition());
	workers.clear();
	mineralMiners = 0;
	gasMiners = 0;
	expandDesire = 1.0;
	/*baseLocation->getGeysers()*/
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

void ResourceDepot::act()
{		
	updateExpandDesire();

	//temp contents
	if (unit->isIdle())
		train((*unit->getType().buildsWhat().begin()));	
	/*if (!train(unit->getPlayer()->getRace().getWorker()))
		unit->train(unit->getPlayer()->getRace().getSupplyProvider());*/
}

void ResourceDepot::updateExpandDesire()
{	
	//temp contents
	if (isMineralSaturated() && expandDesire == 1.0)
	{		
		Task* expand = new Expand();
		TaskHelper::addTask(expand, true);
		expandDesire = 0.0;
	}
}

bool ResourceDepot::isMineralSaturated()
{
	return mineralMiners >= 2.5 * baseLocation->getMinerals().size();
}

bool ResourceDepot::isGasSaturated()
{
	return gasMiners >= 3.0 * baseLocation->getGeysers().size();
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

//void ResourceDepot::addGeyser()
//{
//	baseLocation()
//}
