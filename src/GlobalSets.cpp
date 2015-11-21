#include "GlobalSets.h"

extern std::map<std::pair<int, int>, int, compareUnit> g_AttackUnitSet = std::map<std::pair<int, int>, int, compareUnit>();
extern std::list<Task*> g_Tasks = std::list<Task*>();
extern int g_NextTaskID = 0;
extern int g_NextCoalitionID = 0;

