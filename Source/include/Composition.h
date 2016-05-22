#pragma once
#include "UtilHelper.h"
#include <BWAPI.h>
#include <unordered_map>
#include <vector>

typedef std::unordered_map<BWAPI::UnitType, int> UnitMap;

class Composition
{	
private:
	UnitMap unitMap;
	double cost;
	struct Attributes
	{
		double groundDPS;
		double airDPS;
		int totalHealth;
		double avgGroundRange;
		double avgAirRange;
		double avgSpeed;
		double totalAirRange;
		double totalGroundRange;
		double totalSpeed;
		double maxGroundRange;
		double maxAirRange;
		int unitCount;
		bool detection;
	} attributes;

	void initAttributes();
	void updateMaxRange();
	void updateDetection();
public:
	///constructors
	Composition();
	Composition(BWAPI::Unitset unitSet);
	Composition(UnitMap unitMap);
	Composition(const Composition & comp);

	///operator overrides
	bool operator==(const Composition& rhs) const;
	bool operator>=(const Composition& rhs) const;	
	Composition operator-(const Composition& b);
	Composition getIntersection(const Composition& b);
	int operator[](const BWAPI::UnitType& b);

	///getters
	std::vector<BWAPI::UnitType> getTypes() const;
	UnitMap getUnitMap() const { return unitMap; }
	double getCost() const { return cost; }
	Attributes &getAttributes() { return attributes; }

	///helpers		
	void removeType(const BWAPI::UnitType& unitType, int count = 1);
	void addType(const BWAPI::UnitType& unitType, int count = 1);
	void updateAttributes(const BWAPI::UnitType& unitType, int unitCount = 1);
	void printAttributes();
	void printDebugInfo() const;
	std::string toString() const;
};