#pragma once
#include <BWAPI.h>

extern double g_MinIncome;
extern double g_GasIncome;
extern double g_Supply;

extern std::map<BWAPI::UnitType, bool> g_isUnlocked;
extern std::map<BWAPI::UnitType, int> g_TotalCount;