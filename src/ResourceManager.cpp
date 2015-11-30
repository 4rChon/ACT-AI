#pragma once

#include "ResourceManager.h"

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager()
{
	this->resourceSet = getAllResources();

	for (auto &r : this->resourceSet)
	{
		Resource* resource = new Resource;
		resource->unitID = r->getID();
		resource->miningCount = 0;
		resource->resourceUnit = r;		
		this->resourceStructSet.insert(resource);
	}
}

ResourceManager::~ResourceManager()
{
	this->instance = nullptr;
}

ResourceManager* ResourceManager::getInstance()
{
	if (!instance)
		instance = new ResourceManager();
	return instance;
}

Resource* ResourceManager::getResource(int unitID)
{
	for (auto &resource : this->resourceStructSet)
		if (resource->unitID == unitID)
			return resource;
	return nullptr;
}

BWAPI::Unitset ResourceManager::getResourceSet()
{
	return this->resourceSet;
}

BWAPI::Unitset ResourceManager::getAllResources()
{
	return BWAPI::Broodwar->getUnitsInRectangle(
		BWAPI::Position(0, 0),
		BWAPI::Position(BWAPI::Broodwar->mapWidth() * BWAPI::TILEPOSITION_SCALE, BWAPI::Broodwar->mapHeight() * BWAPI::TILEPOSITION_SCALE),
		BWAPI::Filter::IsResourceContainer);
}

BWAPI::Unit ResourceManager::chooseResource(BWAPI::Unit unit)
{	
	return unit->getClosestUnit(BWAPI::Filter::IsMineralField || BWAPI::Filter::IsRefinery && BWAPI::Filter::IsOwned && !BWAPI::Filter::IsBeingGathered);
	//TO DO: choose closest resource with less than 2 miners
}