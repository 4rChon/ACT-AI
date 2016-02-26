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
public:
	//constructors
	Composition();
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
	Attributes &getAttributes();

	void updateMaxRange();
	void updateDetection();

	//helpers
	void initAttributes();
	void updateAttributes(BWAPI::UnitType unitType, int unitCount = 1);
	void outAttributes();
	void addType(BWAPI::UnitType unitType, int count);
	void debugInfo() const;
	std::string toString() const;
};