#include "BWTA.h"
#include "BWAPI.h"
#include "MapHelper.h"

namespace MapHelper
{	
	namespace
	{
		Field regionField;
	}		

	void initialiseHelper()
	{
		for (auto &region : BWAPI::Broodwar->getAllRegions())
		{			
			Zone* zone = new Zone(region, region->getID());
			regionField.insert(zone);
		}
	}

	Zone* getZone(int id)
	{
		for (auto &zone : regionField)
			if ((*zone).getID() == id)
				return zone;
		return nullptr;
	}

	Zone* getZone(BWAPI::Region region)
	{
		for (auto &zone : regionField)
			if ((*zone).getRegion() == region)
				return zone;
		return nullptr;
	}

	const Field& getRegionField()
	{
		return regionField;
	}

	Zone::Zone(BWAPI::Region region, int id)
	{
		this->region = region;
		this->id = id;
		lastVisited = 0;
		enemyScore = 0;
		friendScore = 0;
		resourceScore = 0;
	}

	BWAPI::Region Zone::getRegion() const
	{
		return region;
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

		for each (auto unit in this->region->getUnits())
		{
			if (unit->getPlayer() == BWAPI::Broodwar->enemy())
				this->enemyScore += unit->getType().buildScore() + unit->getType().destroyScore();
			if (unit->getPlayer() == BWAPI::Broodwar->self())
				this->friendScore += unit->getType().buildScore() + unit->getType().destroyScore();
			if (unit->getType().isResourceContainer())
				this->resourceScore += unit->getResources();
		}

		updateLastVisited();
	}
}
