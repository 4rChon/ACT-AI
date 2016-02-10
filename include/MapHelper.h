#pragma once
#include "BWAPI.h"
#include <BWAPI/SetContainer.h>

namespace MapHelper
{
	struct Zone
	{
	private:
		BWAPI::Region region;
		int id;
		int lastVisited;
		int enemyScore;
		int friendScore;
		int resourceScore;

	public:
		Zone(BWAPI::Region region, int id)
		{
			this->region = region;
			this->id = id;
			lastVisited = 0;
			enemyScore = 0;
			friendScore = 0;
			resourceScore = 0;
		}

		BWAPI::Region getRegion() const
		{
			return region;
		}

		int getID() const
		{
			return id;
		}

		int getLastVisited() const
		{
			return lastVisited;
		}

		int getEnemyScore() const
		{
			return enemyScore;
		}

		int getFriendScore() const
		{
			return friendScore;
		}

		int getResourceScore() const
		{
			return resourceScore;
		}

		void updateLastVisited()
		{
			lastVisited = BWAPI::Broodwar->getFrameCount();
		}

		void resetScores()
		{
			enemyScore = 0;
			friendScore = 0;
			resourceScore = 0;
		}

		void updateZone()
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
	};

	typedef BWAPI::SetContainer<Zone*, std::hash<void*>> Field;

	void initialiseHelper();
	Zone* getZone(int id);
	Zone* getZone(BWAPI::Region region);
	const Field& getRegionField();
}


//std::unordered_map<MapHelper::Zone*, double> AttackDesireMap;
//std::unordered_map<MapHelper::Zone*, double> DefendDesireMap;