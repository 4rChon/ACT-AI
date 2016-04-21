#include "Composition.h"
#include "Task.h"
#include <string>
#include <sstream>

Composition::Composition()
{
	cost = 0;
	initAttributes();
}

Composition::Composition(BWAPI::Unitset unitSet)
{
	cost = 0;
	initAttributes();
	for (auto unit : unitSet)
		addType(unit->getType());
	
}

Composition::Composition(UnitMap unitMap)
{	
	cost = 0;
	initAttributes();
	for (auto &unitType : unitMap)
		addType(unitType.first, unitType.second);
	
}

bool Composition::operator==(const Composition& rhs) const
{
	if (cost != rhs.cost)
		return false;

	for each(auto &unitType in unitMap)
	{
		if (unitType.second != rhs.getUnitMap()[unitType.first])
			return false;
	}

	for each(auto &unitType in rhs.getUnitMap())
	{
		if (unitType.second != getUnitMap()[unitType.first])
			return false;
	}
	return true;
}

bool Composition::operator>=(const Composition& rhs) const
{	
	if (unitMap.size() < rhs.getUnitMap().size())
		return false;

	for each(auto &unitType in unitMap)
		if (unitType.second < rhs.getUnitMap()[unitType.first])
			return false;
	return true;
}

int Composition::operator[](const BWAPI::UnitType& b)
{
	if (unitMap[b] == 0)
	{		
		unitMap.erase(b);
		return 0;
	}

	return unitMap[b];
}

Composition Composition::operator-(const Composition& b)
{
	Composition composition = Composition(unitMap);
	
	for (auto &unit : b.getUnitMap())
		composition.removeType(unit.first, unit.second);

	return composition;
}

Composition Composition::getIntersection(const Composition& b)
{
	Composition c;
	for each(auto unitType in b.getTypes())
	{
		if (unitMap[unitType] > 0)
			c.addType(unitType);
	}

	return c;
}

std::vector<BWAPI::UnitType> Composition::getTypes() const
{
	std::vector<BWAPI::UnitType> unitTypes;
	for (auto &unit : unitMap)
	{
		if(unit.second > 0)
			unitTypes.push_back(unit.first);
	}

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

void Composition::removeType(const BWAPI::UnitType& unitType, int count)
{
	/*std::cout << "Removing " << count << unitType.c_str() << " from composition\n";*/
	unitMap[unitType] -= count;
	cost -= ((unitType.gasPrice() * 1.5) + (unitType.mineralPrice())) * count;
	updateAttributes(unitType, -count);
}

void Composition::addType(const BWAPI::UnitType& unitType, int count)
{
	/*std::cout << "Adding " << count << unitType.c_str() << " from composition\n";*/
	unitMap[unitType] += count;
	cost += ((unitType.gasPrice() * 1.5) + (unitType.mineralPrice())) * count;
	updateAttributes(unitType, count);
}

Composition::Attributes& Composition::getAttributes()
{
	return attributes;
}

//Composition::Parameters& Composition::getParameters()
//{
//	return parameters;
//}

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
	attributes.detection = false;
	for each(auto &unitType in getTypes())
	{
		if (unitType.isDetector())
		{
			attributes.detection = true;
			return;
		}
	}
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

void Composition::updateAttributes(const BWAPI::UnitType& unitType, int unitCount)
{
	if (unitType.canAttack())
	{				
		if (unitType.airWeapon() != BWAPI::WeaponTypes::None)
		{
			auto airDPS = util::game::getSelf()->damage(unitType.airWeapon()) * unitType.maxAirHits() * (24 / (double)util::game::getSelf()->weaponDamageCooldown(unitType));
			auto maxAirRange = util::game::getSelf()->weaponMaxRange(unitType.airWeapon()) / 32;
			attributes.totalAirRange += unitCount * maxAirRange;
			attributes.airDPS += unitCount * airDPS;
		}

		if (unitType.groundWeapon() != BWAPI::WeaponTypes::None)
		{
			auto groundDPS = util::game::getSelf()->damage(unitType.groundWeapon()) * unitType.maxGroundHits() * (24 / (double)util::game::getSelf()->weaponDamageCooldown(unitType));
			auto maxGroundRange = util::game::getSelf()->weaponMaxRange(unitType.groundWeapon()) / 32;
			attributes.totalGroundRange += unitCount * maxGroundRange;
			attributes.groundDPS += unitCount * groundDPS;
		}
	}
	
	if (unitType.canMove())
		attributes.totalSpeed += unitCount * (util::game::getSelf()->topSpeed(unitType));
	
	if (attributes.groundDPS < 1)
		attributes.groundDPS = 0;
	if (attributes.airDPS < 1)
		attributes.airDPS = 0;

	attributes.totalHealth += unitCount * (unitType.maxHitPoints() + unitType.maxShields());
	attributes.unitCount += unitCount;

	attributes.avgAirRange = attributes.totalAirRange / attributes.unitCount;
	attributes.avgGroundRange = attributes.totalGroundRange / attributes.unitCount;
	attributes.avgSpeed = attributes.totalSpeed / attributes.unitCount;

	updateMaxRange();
	updateDetection();
}

void Composition::printAttributes()
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

void Composition::printDebugInfo() const
{
	for (auto &unit : unitMap)
	{
		if(unit.second > 0)
			std::cout << unit.first.c_str() << " : " << unit.second << "\n";
	}
	std::cout << "Cost: " << cost << "\n";
}

std::string Composition::toString() const
{
	std::ostringstream oss;
	oss << cost << "\n";
	for (auto &unit : unitMap)
	{
		if (unit.second > 0)
			oss << unit.first.c_str() << ", " << unit.second << "\n";
	}
	return oss.str();
}
