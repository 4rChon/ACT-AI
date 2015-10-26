#pragma once
#include "Coalition.h"
#include "Agent.h"
#include "Task.h"

extern std::unordered_set<Coalition*> g_Coalitions;
extern std::unordered_set<Coalition*> g_OpenCoalitions;
extern std::unordered_set<Agent*> g_Agents;
extern std::unordered_set<Agent*> g_FreeAgents;
extern std::list<Task*> g_Tasks;
extern std::map<BWAPI::UnitType, bool> g_isUnlocked;
extern std::map<BWAPI::UnitType, int> g_TotalCount;