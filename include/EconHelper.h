#pragma once
#include "BWAPI.h"
namespace EconHelper
{
	void initialiseHelper();
	int haveMoney(BWAPI::UnitType unitType);
	int haveMoney(BWAPI::UpgradeType upgradeType);
	int haveMoney(int minerals, int gas);	
	int haveSupply(BWAPI::UnitType unitType);
	void addDebt(int minerals, int gas);
	void subtractDebt(int minerals, int gas);
}
