#pragma once
#include "BWAPI.h"
#include "UtilHelper.h"
#include "Composition.h"
#include "ResourceDepot.h"

namespace EconHelper
{
	void initialiseHelper();
	int haveMoney(BWAPI::UnitType unitType);
	int haveMoney(BWAPI::UpgradeType upgradeType);
	int haveMoney(BWAPI::TechType techType);
	int haveMoney(int minerals, int gas);	
	int haveSupply(BWAPI::UnitType unitType);
	void addDebt(int minerals, int gas);
	void subtractDebt(int minerals, int gas);
	int getMinerals();
	int getGas();	
	double getMineralIncome();
	double getGasIncome();
	double getUnitMultiplier(Composition composition);
	void doneExpanding();
	void updateEconomy();
	ResourceDepot* getLeastSaturatedBase();
	void expand();
}
