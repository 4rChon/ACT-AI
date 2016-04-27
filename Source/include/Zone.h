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
	bool defending;
	
	MapHelper::Field neighbourhood;	
public:
	///constructor
	Zone(BWAPI::Region region, int id);

	void initNeighbourhood();
	const MapHelper::Field& getNeighbourhood();

	///getters
	BWAPI::Region getRegion() const;
	BWTA::Region* getSuperRegion() const;
	int getID() const;
	int getLastVisited() const;
	int getEnemyScore() const;
	int getFriendScore() const;
	int getResourceScore() const;
	int getTimesDefended() const;
	bool hasBunkerDefense() const;
	bool isDefending() const;

	///setters
	void setDefending(bool defending);

	void setTimesDefended(int timesDefended);

	void setBunkerDefense(bool hasBunker);

	void defend(bool defendOrder);

	///helpers
	void updateLastVisited();
	void resetScores();
	void updateZone();
};