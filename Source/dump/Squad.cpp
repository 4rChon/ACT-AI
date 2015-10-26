#include "Squad.h"
#include "ThreatField.h"
#include "_Unit.h"
#include "Zone.h"
#include <vector>
#include <BWAPI.h>

Squad::Squad(BWAPI::Unitset unitSet)
{
	for each(auto unit in unitSet)
	{
		_Unit u(unit);
		this->unitSet.push_back(u);
	}
	this->target = nullptr;
}

Squad::Squad(std::vector<_Unit> unitSet)
{
	this->unitSet = unitSet;
	this->target = nullptr;
}

Squad::~Squad()
{
	delete this->target;
}
//bool act(std::vector<Order> orderSet)
Zone* Squad::getZone()
{
	return this->target;
}

std::vector<_Unit> Squad::getUnitSet()
{
	return this->unitSet;
}

void Squad::setActive(bool activity)
{
	this->active = activity;
}

void Squad::setZone(Zone* z)
{
	this->target = z;
}

void Squad::setUnitSet(BWAPI::Unitset unitSet)
{
	for each(auto unit in unitSet)
	{
		_Unit u(unit);
		this->unitSet.push_back(u);
	}
}

void Squad::setUnitSet(std::vector<_Unit> unitSet)
{
	this->unitSet = unitSet;
}

void Squad::attack(std::vector<Zone*> field)
{
	int index = rand() % field.size();
	target = field[index];

	if (BWAPI::Broodwar->elapsedTime() - target->getLastVisit() > 20)
	{
		for each(auto unit in this->unitSet)
		{
			if (unit.unit->isIdle())
				//if (unit.unit->getUnitsInRadius(50, BWAPI::Filter::IsOwned).size() < 2)
				//	unit.rally(field);
				//else 
				unit.attack(target);
		}
	}
}
