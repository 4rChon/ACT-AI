#pragma once
#include "Coalition.h"
#include "Agent.h"
#include "Task.h"

extern std::unordered_set<Coalition*> g_Coalitions;
extern std::unordered_set<Coalition*> g_OpenCoalitions;
extern std::list<Task*> g_Tasks;
extern int g_NextTaskID;
extern int g_NextCoalitionID;