#pragma once
#include "Zone.h"
#include <BWAPI.h>
#include <vector>

class ThreatField
{
private:
	std::vector<Zone*> field;
	static ThreatField* instance;
	int size;
public:
	//Constructors
	ThreatField(BWAPI::Regionset regionSet);

	//setters
	void setSize(int size);
	void setZone(Zone* zone, int index);
	void setField(std::vector<Zone*> field);

	//getters
	static ThreatField* getInstance();
	int getSize();
	Zone* getZone(int index);
	Zone* getRandomZone();
	std::vector<Zone*> getField();
	float getAverageVisitTime();
};