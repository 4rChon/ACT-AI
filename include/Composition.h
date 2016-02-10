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
public:
	//constructors
	Composition(BWAPI::Unitset unitSet);
	Composition(UnitMap unitMap);

	//operator overrides
	bool operator==(const Composition& rhs) const;
	Composition operator+=(const BWAPI::UnitType& rhs);
	Composition operator-=(const BWAPI::UnitType& rhs);
	Composition operator-(const Composition& b);
	int operator[](const BWAPI::UnitType& b);

	//getters
	std::vector<BWAPI::UnitType> getTypes() const;
	UnitMap getUnitMap() const;
	double getCost() const;

	//helpers
	void addType(BWAPI::UnitType unitType, int count);
	void debugInfo() const;
};