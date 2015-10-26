#pragma once
#include <BWAPI.h>

class Zone
{
private:	
	float enemyScore, friendScore, resourceScore;
	int lastVisited;
	BWAPI::Region region;
public:
	//constructors
	Zone(BWAPI::Region);

	//setters
	void setEnemyScore(float enemyScore);
	void setFriendScore(float friendScore);
	void setResourceScore(float resourceScore);
	void setConfidence(int lastVisited);
	void setRegion(BWAPI::Region region);

	//getters
	float getEnemyScore() const;
	float getFriendScore();
	float getResourceScore() const;
	float getConfidence();
	int getLastVisit() const;
	BWAPI::Region getRegion() const;

	//-
	void updateZone();

};
