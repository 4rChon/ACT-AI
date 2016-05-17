#include "Zone.h"
#include "DesireHelper.h"
#include "MapHelper.h"
#include <BWTA.h>

Zone::Zone(BWAPI::Region region, int id)
	: region(region)
	, id(id)
	, superRegion(BWTA::getRegion(region->getCenter()))
	, lastVisited(0)
	, enemyScore(0)
	, friendScore(0)
	, resourceScore(0)
	, timesDefended(0)
	, hasBunker(false)
	, hasTurret(false)
	, defending(false)
{
}

void Zone::initNeighbourhood()
{
	for (auto &zone : MapHelper::getRegionField())
	{
		if (zone->getSuperRegion() == superRegion)
			neighbourhood.insert(zone);			
	}
}

void Zone::setDefending(bool defending)
{
	this->defending = defending;	

	DesireHelper::setDefendDesire(this, getEnemyScore());
}

void Zone::defend(bool defendOrder)
{
	for (auto &zone : neighbourhood)
		zone->setDefending(defendOrder);

	timesDefended++;
}

void Zone::updateZone()
{
	resetScores();

	for (auto& unit : region->getUnits())
	{
		if (unit->getPlayer() == util::game::getEnemy())
			enemyScore += 100 + unit->getType().buildScore() + unit->getType().destroyScore();
		if (unit->getPlayer() == util::game::getSelf() && (!unit->getType().isBuilding() || unit->getType() == BWAPI::UnitTypes::Terran_Bunker || unit->getType() == BWAPI::UnitTypes::Terran_Missile_Turret))
			friendScore += 100 + unit->getType().buildScore() + unit->getType().destroyScore();
		if (unit->getType().isResourceContainer())
			resourceScore += unit->getResources();
	}

	updateLastVisited();
}