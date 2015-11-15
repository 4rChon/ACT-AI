#include "Zone.h"
#include <BWAPI.h>
#include <cmath>

Zone::Zone(BWAPI::Region region)
{
	//std::cout << "Zone(region)\n";
	this->region = region;
	this->enemyScore = 0;
	this->friendScore = 0;
	this->resourceScore = 0;
	this->lastVisited = 0;
}

float Zone::getEnemyScore() const
{
	return this->enemyScore;
}
float Zone::getFriendScore()
{
	if (this->region->getUnits().size() == 0)
		this->friendScore = 0;
	
	return this->friendScore;
}
float Zone::getResourceScore() const
{
	return this->resourceScore;
}
float Zone::getConfidence()
{
	if (lastVisited == 0)
		return 0;
	if (BWAPI::Broodwar->elapsedTime() == this->lastVisited)
		return 1;
	return (powf(0.99f, float(BWAPI::Broodwar->elapsedTime() - this->lastVisited)));
}
int Zone::getLastVisit() const
{
	return this->lastVisited;
}

BWAPI::Region Zone::getRegion() const
{
	return this->region;
}

void Zone::setEnemyScore(float enemyScore)
{
	this->enemyScore = enemyScore;
}

void Zone::setFriendScore(float friendScore)
{	
	this->friendScore = friendScore;
}

void Zone::setResourceScore(float resourceScore)
{
	this->resourceScore = resourceScore;
}

void Zone::setConfidence(int lastVisited)
{
	this->lastVisited = lastVisited;
}

void Zone::setRegion(BWAPI::Region region)
{
	this->region = region;
}

void Zone::updateZone()
{
	/* get all friendly units in zone*/
	auto unitSet = this->region->getUnits(BWAPI::Filter::IsOwned);	
	if (BWAPI::Broodwar->elapsedTime() == this->lastVisited)
		return;
	/* get all opposing units in zone and adjust threat*/	
	this->friendScore = 0;
	this->enemyScore = 0;
	this->resourceScore = 0;

	for each (auto unit in this->region->getUnits())
	{
		if (unit->getPlayer() == BWAPI::Broodwar->enemy())
			this->enemyScore += unit->getType().buildScore() + unit->getType().destroyScore();				
		if (unit->getPlayer() == BWAPI::Broodwar->self())
			this->friendScore += unit->getType().buildScore() + unit->getType().destroyScore();
		if (unit->getType().isResourceContainer())
			this->resourceScore += unit->getResources();
	}
	this->lastVisited = BWAPI::Broodwar->elapsedTime();
}