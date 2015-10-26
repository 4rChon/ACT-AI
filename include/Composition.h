#pragma once
#include <BWAPI.h>
#include <unordered_map>
#include <vector>

namespace std
{
	template<> struct hash < BWAPI::UnitType >
	{
		size_t operator()(const BWAPI::UnitType &u) const
		{
			return 51 * (std::hash<std::string>()(u.getName()));
		}
	};

	template<> struct equal_to < BWAPI::UnitType >
	{
		size_t operator()(const BWAPI::UnitType &lhs, const BWAPI::UnitType &rhs) const
		{
			return lhs.getID() == rhs.getID();
		}
	};
}

class Composition
{
private:
	std::unordered_map<BWAPI::UnitType, int> unitMap;
public:
	//setters
	Composition();
	Composition(BWAPI::Unitset unitSet);
	Composition(std::unordered_map<BWAPI::UnitType, int> unitMap);

	//operator overrides
	bool operator==(const Composition& rhs) const;
	Composition operator+=(const BWAPI::Unit& rhs);
	Composition operator-=(const BWAPI::Unit& rhs);
	int operator[](const BWAPI::UnitType& b);	

	//getters
	std::vector<BWAPI::UnitType> getTypes() const;
	std::unordered_map<BWAPI::UnitType, int> getUnitMap() const;

	//-
	void addType(BWAPI::UnitType unitType, int count);
	void addUnit(BWAPI::Unit unit);
};