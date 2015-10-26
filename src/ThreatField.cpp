#include "ThreatField.h"
#include "Zone.h"
#include <BWAPI.h>
#include <vector>
#include <cmath>
#include <iostream>

#include <string>
#include <fstream>
#include <sstream>

using namespace BWAPI;

bool zoneCompare(Zone* a, Zone* b) { return a->getRegion()->getID() < b->getRegion()->getID(); }

ThreatField::ThreatField(Regionset regionset)
{	
	std::cout << "ThreatField(regionSet)\n";
	for each (Region region in regionset)
	{
		Zone* z = new Zone(region);
		field.push_back(z);
	}

	std::sort(field.begin(), field.end(), zoneCompare);

	size = field.size();
}

int ThreatField::getSize()
{
	return size;
}

Zone* ThreatField::getZone(int index)
{
	return field[index];
}

std::vector<Zone*> ThreatField::getField()
{
	return field;
}

void ThreatField::setSize(int size)
{
	this->size = size;
}

void ThreatField::setZone(Zone* zone, int index)
{
	field[index] = zone;
}

void ThreatField::setField(std::vector<Zone*> field)
{
	field = field;
}

float ThreatField::getAverageVisitTime()
{
	int total = 0;
	for each(auto zone in field)
	{
		total += BWAPI::Broodwar->elapsedTime() - zone->getLastVisit();
	}
	return (float)total / (float)this->size;
}
