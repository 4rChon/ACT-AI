#include "GlobalVariables.h"

extern double g_MinIncome = 0;
extern double g_GasIncome = 0;
extern double g_Supply = 0;
extern double g_MinReserve = 0;
extern double g_GasReserve = 0;
extern int g_attackTarget = -1;
extern int g_defendTarget = -1;
extern int g_scoutTarget = -1;
extern int g_enemyStart = -1;

extern std::map<BWAPI::UnitType, bool> g_isUnlocked = std::map<BWAPI::UnitType, bool>();
extern std::map<BWAPI::UnitType, int> g_TotalCount = std::map<BWAPI::UnitType, int>();