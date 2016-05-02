#include "Zone.h"
#include "DesireHelper.h"
#include "MapHelper.h"
#include <BWTA.h>

Zone::Zone(BWAPI::Region region, int id)
{
	this->region = region;
	this->id = id;
	superRegion = BWTA::getRegion(region->getCenter());
	lastVisited = 0;
	enemyScore = 0;
	friendScore = 0;
	resourceScore = 0;
	timesDefended = 0;
	hasBunker = false;
	defending = false;
}

void Zone::initNeighbourhood()
{
	for (auto &zone : MapHelper::getRegionField())
	{
		if (zone->getSuperRegion() == superRegion)
			neighbourhood.insert(zone);			
	}
}

const MapHelper::Field & Zone::getNeighbourhood()
{
	return neighbourhood;
}

BWAPI::Region Zone::getRegion() const
{
	return region;
}

BWTA::Region * Zone::getSuperRegion() const
{
	return superRegion;
}

int Zone::getID() const
{
	return id;
}

int Zone::getLastVisited() const
{
	return lastVisited;
}

int Zone::getEnemyScore() const
{
	return enemyScore;
}

int Zone::getFriendScore() const
{
	return friendScore;
}

int Zone::getResourceScore() const
{
	return resourceScore;
}

int Zone::getTimesDefended() const
{
	return timesDefended;
}

bool Zone::hasBunkerDefense() const
{
	return hasBunker;
}

bool Zone::isDefending() const
{
	return defending;
}

void Zone::setDefending(bool defending)
{
	this->defending = defending;	

	DesireHelper::setDefendDesire(this, getEnemyScore());
}

void Zone::setTimesDefended(int timesDefended)
{
	this->timesDefended = timesDefended;
}

void Zone::setBunkerDefense(bool hasBunker)
{
	this->hasBunker = hasBunker;
}

void Zone::defend(bool defendOrder)
{
	for (auto &zone : neighbourhood)
		zone->setDefending(defendOrder);

	timesDefended++;
}

void Zone::updateLastVisited()
{
	lastVisited = BWAPI::Broodwar->getFrameCount();
}

void Zone::resetScores()
{
	enemyScore = 0;
	friendScore = 0;
	resourceScore = 0;
}

void Zone::updateZone()
{
	resetScores();

	for (auto& unit : region->getUnits())
	{
		if (unit->getPlayer() == util::game::getEnemy())
			enemyScore = 100 + unit->getType().buildScore() + unit->getType().destroyScore();
		if (unit->getPlayer() == util::game::getSelf() && (!unit->getType().isBuilding() || unit->getType() == BWAPI::UnitTypes::Terran_Bunker || unit->getType() == BWAPI::UnitTypes::Terran_Missile_Turret))
			friendScore = 100 + unit->getType().buildScore() + unit->getType().destroyScore();
		if (unit->getType().isResourceContainer())
			resourceScore += unit->getResources();
	}

	updateLastVisited();
}