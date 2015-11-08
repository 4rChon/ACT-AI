#pragma once
#include <BWAPI.h>
#include "Composition.h"
#include "GlobalVariables.h"

namespace std
{
	template<> struct hash < BWAPI::UnitCommandType >
	{
		size_t operator()(const BWAPI::UnitCommandType &a) const
		{
			return (51 * (std::hash<std::string>()(a.toString())));
		}
	};

	template<> struct equal_to < BWAPI::UnitCommandType >
	{
		bool operator()(const BWAPI::UnitCommandType &lhs, const BWAPI::UnitCommandType &rhs) const
		{
			return lhs.getID() == rhs.getID();
		}
	};
}

class Agent
{
private:
	BWAPI::Unit unit;
	std::unordered_map<BWAPI::UnitCommandType, double> commandMap;
	BWAPI::UnitCommandType commandType;
	std::unordered_map<BWAPI::UnitType, double> produceMap;
	BWAPI::UnitType produceType;
public:
	//constructors
	Agent(BWAPI::Unit unit);

	//setters
	void setUnit(BWAPI::Unit unit);

	//getters
	BWAPI::Unit getUnit() const;

	//-
	void Agent::act();
	
};