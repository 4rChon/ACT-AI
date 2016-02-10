#pragma once
#include "BWAPI.h"
namespace EconHelper
{
	int haveMoney(BWAPI::UnitType unitType);
	int haveMoney(BWAPI::UpgradeType upgradeType);
	int haveMoney(int minerals, int gas);	
	int haveSupply(BWAPI::UnitType unitType);
	static int mineralDebt;
	static int gasDebt;
}
