#pragma once
#include <BWAPI.h>

extern double g_MinIncome;
extern double g_GasIncome;
extern double g_Supply;
extern double g_MinReserve;
extern double g_GasReserve;
extern int g_attackTarget;
extern int g_defendTarget;
extern int g_scoutTarget;
extern int g_enemyStart;

extern std::map<BWAPI::UnitType, bool> g_isUnlocked;
extern std::map<BWAPI::UnitType, int> g_TotalCount;
