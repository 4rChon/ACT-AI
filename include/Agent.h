#pragma once
#include <BWAPI.h>
#include <random>
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
protected:
	bool freeAgent;
	int unitID;
	int coalitionID;
	BWAPI::Unit unit;
	BWAPI::UnitCommandType commandType;
	std::unordered_map<BWAPI::UnitCommandType, double> commandMap;	
	std::vector<double> commandWeights;
	std::default_random_engine generator;
	double freewill;	

public:
	//constructors
	Agent();
	Agent(BWAPI::Unit unit, double freewill);

	//setters
	void setFree(bool free);
	void setUnit(BWAPI::Unit unit);

	//getters
	bool isFree() const;
	int getID() const;
	BWAPI::Unit getUnit() const;	

	//-
	virtual void act();

	//helpers
	virtual void updateUnlocked();
	void updateCommandWeights();
	void chooseCommand();
	
};

namespace std
{
	template<> struct hash < Agent >
	{
		size_t operator()(const Agent &a) const
		{
			return (51 * (std::hash<int>()(a.getUnit()->getID())));
		}
	};

	template<> struct equal_to < Agent >
	{
		bool operator()(const Agent &lhs, const Agent &rhs) const
		{
			return lhs.getUnit()->getID() == rhs.getUnit()->getID();
		}
	};
}