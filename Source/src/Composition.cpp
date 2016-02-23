#include "Composition.h"

Composition::Composition()
{
	cost = 0;
}

Composition::Composition(BWAPI::Unitset unitSet)
{
	cost = 0;
	for (auto unit : unitSet)
		*this += unit->getType();
}

Composition::Composition(UnitMap unitMap)
{	
	for (auto &unitType : unitMap)
		addType(unitType.first, unitType.second);
}

bool Composition::operator==(const Composition& rhs) const
{
	if (unitMap.size() != rhs.getUnitMap().size())
		return false;

	for each(auto &unitType in unitMap)
		if (unitType.second != rhs.getUnitMap()[unitType.first])
			return false;
	return true;
}

Composition Composition::operator+=(const BWAPI::UnitType& rhs)
{
	this->unitMap[rhs]++;
	this->cost += (rhs.gasPrice() * 1.5) + (rhs.mineralPrice());
	return *this;
}

Composition Composition::operator-=(const BWAPI::UnitType& rhs)
{
	this->unitMap[rhs]--;
	this->cost -= (rhs.gasPrice() * 1.5) + (rhs.mineralPrice());
	return *this;
}

int Composition::operator[](const BWAPI::UnitType& b)
{
	return this->unitMap[b];
}

Composition Composition::operator-(const Composition& b)
{
	Composition composition = Composition(this->unitMap);
	for (auto &unit : b.getUnitMap())
		composition -= unit.first;

	return composition;
}

std::vector<BWAPI::UnitType> Composition::getTypes() const
{
	std::vector<BWAPI::UnitType> unitTypes;
	for each (auto &unitType in this->unitMap)
		unitTypes.push_back(unitType.first);

	return unitTypes;
}

UnitMap Composition::getUnitMap() const
{
	return this->unitMap;
}

double Composition::getCost() const
{
	return this->cost;
}

void Composition::addType(BWAPI::UnitType unitType, int count)
{
	unitMap[unitType] += count;
	this->cost += ((unitType.gasPrice() * 1.5) + (unitType.mineralPrice())) * count;
}

void Composition::debugInfo() const
{
	for (auto &unit : unitMap)
		std::cout << unit.first.c_str() << " : " << unit.second << "\n";
	std::cout << "Cost: " << cost << "\n";
}
