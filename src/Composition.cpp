#include "Composition.h"

Composition::Composition()
{
}

Composition::Composition(BWAPI::Unitset unitSet)
{
	for (auto unit : unitSet)
	{
		this->addUnit(unit);		
	}
}

Composition::Composition(std::unordered_map<BWAPI::UnitType, int> unitMap)
{
	this->unitMap = unitMap;
}

bool Composition::operator==(const Composition& rhs) const
{
	//std::cout << "Composition: Equating\n";
	//return unitMap == rhs.getUnitMap();
	if (unitMap.size() != rhs.getUnitMap().size())
		return false;

	for each(auto unitType in unitMap)
		//std::cout << "UnitType: " << unitType.second << " == " << rhs.getUnitMap()[unitType.first] << "\n";
		if (unitType.second != rhs.getUnitMap()[unitType.first])
			return false;
	return true;
}

Composition Composition::operator+=(const BWAPI::Unit& rhs)
{
	this->unitMap[rhs->getType()]++;
	cost += (rhs->getType().gasPrice() * 1.5) + (rhs->getType().mineralPrice());
	return *this;
}

Composition Composition::operator-=(const BWAPI::Unit& rhs)
{
	this->unitMap[rhs->getType()]--;
	cost -= (rhs->getType().gasPrice() * 1.5) + (rhs->getType().mineralPrice());
	return *this;
}

int Composition::operator[](const BWAPI::UnitType& b)
{
	return this->unitMap[b];
}

std::vector<BWAPI::UnitType> Composition::getTypes() const
{
	std::vector<BWAPI::UnitType> unitTypes;
	for each (auto unitType in this->unitMap)
		unitTypes.push_back(unitType.first);

	return unitTypes;
}

std::unordered_map<BWAPI::UnitType, int> Composition::getUnitMap() const
{
	return this->unitMap;
}

void Composition::addType(BWAPI::UnitType unitType, int count)
{
	unitMap.insert(std::pair<BWAPI::UnitType, int>(unitType, count));
	cost += (unitType.gasPrice() * 1.5) + (unitType.mineralPrice()) * count;
}

void Composition::addUnit(BWAPI::Unit unit)
{
	addType(unit->getType(), 1);
	cost += (unit->getType().gasPrice() * 1.5) + (unit->getType().mineralPrice());
}