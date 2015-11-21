#pragma once
#include "Task.h"
#include <set>

struct compareUnit
{
	bool operator()(const std::pair<int, int> a, const std::pair<int, int> b)
	{ 
		if (BWAPI::UnitType(a.second).buildScore() == BWAPI::UnitType(b.second).buildScore())
			return a.first > b.first;
		return BWAPI::UnitType(a.second).buildScore() > BWAPI::UnitType(b.second).buildScore();
	}
};

extern std::map<std::pair<int, int>, int, compareUnit> g_AttackUnitSet;
extern std::list<Task*> g_Tasks;
extern int g_NextTaskID;
extern int g_NextCoalitionID;