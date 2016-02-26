#include "Composition.h"
#include <string>
#include <sstream>

Composition::Composition()
{
	cost = 0;
}

Composition::Composition(BWAPI::Unitset unitSet)
{
	cost = 0;
	for (auto unit : unitSet)
		*this += unit->getType();
	initAttributes();
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
	unitMap[rhs]++;
	cost += (rhs.gasPrice() * 1.5) + (rhs.mineralPrice());
	updateAttributes(rhs);
	return *this;
}

Composition Composition::operator-=(const BWAPI::UnitType& rhs)
{
	unitMap[rhs]--;
	cost -= (rhs.gasPrice() * 1.5) + (rhs.mineralPrice());
	updateAttributes(rhs, -1);
	return *this;
}

int Composition::operator[](const BWAPI::UnitType& b)
{
	return unitMap[b];
}

Composition Composition::operator-(const Composition& b)
{
	Composition composition = Composition(unitMap);
	for (auto &unit : b.getUnitMap())
		composition -= unit.first;

	return composition;
}

std::vector<BWAPI::UnitType> Composition::getTypes() const
{
	std::vector<BWAPI::UnitType> unitTypes;
	for each (auto &unitType in unitMap)
		unitTypes.push_back(unitType.first);

	return unitTypes;
}

UnitMap Composition::getUnitMap() const
{
	return unitMap;
}

double Composition::getCost() const
{
	return cost;
}

void Composition::addType(BWAPI::UnitType unitType, int count)
{
	unitMap[unitType] += count;
	cost += ((unitType.gasPrice() * 1.5) + (unitType.mineralPrice())) * count;
	updateAttributes(unitType, count);
}

Composition::Attributes & Composition::getAttributes()
{
	return attributes;
}

void Composition::updateMaxRange()
{
	attributes.maxAirRange = 0.0;
	attributes.maxGroundRange = 0.0;
	for each(auto &unitType in getTypes())
	{
		attributes.maxAirRange = unitType.airWeapon().maxRange() > attributes.maxAirRange ? unitType.airWeapon().maxRange() : attributes.maxAirRange;
		attributes.maxGroundRange = unitType.groundWeapon().maxRange() > attributes.maxGroundRange ? unitType.groundWeapon().maxRange() : attributes.maxGroundRange;
	}
}

void Composition::updateDetection()
{
	for each(auto &unitType in getTypes())
		attributes.detection = unitType.isDetector() ? unitType.isDetector() : attributes.detection;
}

void Composition::initAttributes()
{
	attributes.airDPS = 0.0;
	attributes.groundDPS = 0.0;
	attributes.avgAirRange = 0.0;
	attributes.avgGroundRange = 0.0;
	attributes.avgSpeed = 0.0;
	attributes.totalAirRange = 0.0;
	attributes.totalGroundRange = 0.0;
	attributes.totalSpeed = 0.0;
	attributes.detection = false;
	attributes.maxAirRange = 0;
	attributes.maxGroundRange = 0;
	attributes.totalHealth = 0;
	attributes.unitCount = 0;

	for each(auto &unitType in getTypes())
		updateAttributes(unitType, unitMap[unitType]);
}

void Composition::updateAttributes(BWAPI::UnitType unitType, int unitCount)
{
	attributes.airDPS += unitType.airWeapon().damageAmount() ? unitCount * (unitType.airWeapon().damageAmount() * unitType.maxAirHits() * (24 / (double)unitType.airWeapon().damageCooldown())) : 0;
	attributes.groundDPS += unitType.groundWeapon().damageAmount() ? unitCount * (unitType.groundWeapon().damageAmount() * unitType.maxGroundHits() * (24 / (double)unitType.groundWeapon().damageCooldown())) : 0;
	attributes.avgAirRange += unitCount * (unitType.airWeapon().maxRange() / 32);
	attributes.avgGroundRange += unitCount * (unitType.groundWeapon().maxRange() / 32);
	attributes.avgSpeed += unitCount * (unitType.topSpeed());	
	attributes.totalHealth += unitCount * (unitType.maxHitPoints() + unitType.maxShields());
	attributes.unitCount += unitCount;
	attributes.avgAirRange = attributes.totalAirRange / attributes.unitCount;
	attributes.avgGroundRange = attributes.totalGroundRange / attributes.unitCount;
	attributes.avgSpeed = attributes.totalSpeed / attributes.unitCount;
	updateMaxRange();
	updateDetection();
}

void Composition::outAttributes()
{
	std::cout << "\tairDPS: " << attributes.airDPS << "\n";
	std::cout << "\tgroundDPS: " << attributes.groundDPS << "\n";
	std::cout << "\tavgAirRange: " << attributes.avgAirRange << "\n";
	std::cout << "\tavgGroundRange: " << attributes.avgGroundRange << "\n";
	std::cout << "\tavgSpeed: " << attributes.avgSpeed << "\n";
	std::cout << "\tdetection: " << attributes.detection << "\n";
	std::cout << "\tmaxAirRange: " << attributes.maxAirRange << "\n";
	std::cout << "\tmaxGroundRange: " << attributes.maxGroundRange << "\n";
	std::cout << "\ttotalHealth: " << attributes.totalHealth << "\n";
}

void Composition::debugInfo() const
{
	for (auto &unit : unitMap)
		std::cout << unit.first.c_str() << " : " << unit.second << "\n";
	std::cout << "Cost: " << cost << "\n";
}

std::string Composition::toString() const
{
	std::ostringstream oss;
	oss << cost << "\n";
	for (auto &unit : unitMap)
		oss << unit.first.c_str() << ", " << unit.second << "\n";
	return oss.str();
}
