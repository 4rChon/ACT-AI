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

bool Zone::isDefending() const
{
	return defending;
}

void Zone::setDefending(bool defending)
{
	this->defending = defending;	

	if (defending)
		DesireHelper::updateDefendDesire(this, getEnemyScore());
	else
		DesireHelper::updateDefendDesire(this, 0.0);
}

void Zone::setTimesDefended(int timesDefended)
{
	this->timesDefended = timesDefended;
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

	for (auto& unit : this->region->getUnits())
	{
		if (unit->getPlayer() == util::game::getEnemy())
			enemyScore++;// unit->getType().buildScore() + unit->getType().destroyScore();
		if (unit->getPlayer() == util::game::getSelf())
			friendScore++;// = unit->getType().buildScore() + unit->getType().destroyScore();
		if (unit->getType().isResourceContainer())
			resourceScore++;// unit->getResources();
	}

	updateLastVisited();
}