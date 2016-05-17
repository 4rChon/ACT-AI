#pragma once
#include "MapHelper.h"

class Zone
{
private:
	BWAPI::Region region;
	BWTA::Region* superRegion;
	int id;
	int lastVisited;
	int enemyScore;
	int friendScore;
	int resourceScore;
	int timesDefended;
	bool hasBunker;
	bool hasTurret;
	bool defending;
	
	MapHelper::Field neighbourhood;	
public:
	///constructor
	Zone(BWAPI::Region region, int id);

	void initNeighbourhood();	

	///getters
	BWAPI::Region getRegion() const { return region; }
	BWTA::Region* getSuperRegion() const { return superRegion; }
	const MapHelper::Field& getNeighbourhood() const { return neighbourhood; }
	int getID() const { return id; }
	int getLastVisited() const { return lastVisited; }
	int getEnemyScore() const { return enemyScore; }
	int getFriendScore() const { return friendScore; }
	int getResourceScore() const { return resourceScore; }
	int getTimesDefended() const { return timesDefended; }
	bool hasBunkerDefense() const { return hasBunker; }
	bool hasTurretDefense() const { return hasTurret; }
	bool isDefending() const { return defending; }

	///setters
	void setDefending(bool defending);
	void setTimesDefended(int timesDefended) { this->timesDefended = timesDefended; }
	void setBunkerDefense(bool hasBunker) { this->hasBunker = hasBunker; }
	void setTurretDefense(bool hasTurret) { this->hasTurret = hasTurret; }
	void defend(bool defendOrder);

	///helpers
	void updateLastVisited() { lastVisited = BWAPI::Broodwar->getFrameCount(); }
	void resetScores()
	{
		enemyScore = 0;
		friendScore = 0;
		resourceScore = 0;
	}

	void updateZone();
};