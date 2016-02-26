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
		Zone(BWAPI::Region region, int id);
		BWAPI::Region getRegion() const;
		int getID() const;
		int getLastVisited() const;
		int getEnemyScore() const;
		int getFriendScore() const;
		int getResourceScore() const;
		void updateLastVisited();
		void resetScores();
		void updateZone();
	};

	typedef BWAPI::SetContainer<Zone*, std::hash<void*>> Field;

	void initialiseHelper();
	Zone* getZone(int id);
	Zone* getZone(BWAPI::Region region);
	const Field& getRegionField();
	/*std::set<BWTA::BaseLocation*>& getCandidateBases();*/
}