#include "EconHelper.h"
#include "TaskHelper.h"
#include "DesireHelper.h"
#include "Expand.h"

namespace EconHelper
{
	namespace
	{
		static int mineralDebt;
		static int gasDebt;

		static double mineralIncome;
		static double gasIncome;

		static int lastCheckMineralsFrame;
		static int lastCheckGasFrame;
		static int lastCheckMinerals;
		static int lastCheckGas;

		static double unitMultiplier;

		static bool expanding;
	}

	void initialiseHelper()
	{	
		lastCheckMineralsFrame = 0;
		lastCheckGasFrame = 0;
		lastCheckMinerals = 0;
		lastCheckGas = 0;
		mineralDebt = 0;
		gasDebt = 0;
		mineralIncome = 0.0;
		gasIncome = 0.0;
		unitMultiplier = 0.0;
	}

	int haveMoney(BWAPI::UnitType unitType)
	{
		return haveMoney(unitType.mineralPrice(), unitType.gasPrice());
	}

	int haveMoney(BWAPI::UpgradeType upgradeType)
	{
		return haveMoney(upgradeType.mineralPrice(), upgradeType.gasPrice());
	}

	int haveMoney(BWAPI::TechType techType)
	{
		return haveMoney(techType.mineralPrice(), techType.gasPrice());
	}

	int haveMoney(int minerals, int gas)
	{
		return getMinerals() >= minerals && getGas() >= gas;
	}

	int haveSupply(BWAPI::UnitType unitType)
	{
		return BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() >= unitType.supplyRequired();
	}

	int getMinerals()
	{
		return BWAPI::Broodwar->self()->minerals() - mineralDebt;
	}

	int getGas()
	{
		return BWAPI::Broodwar->self()->gas() - gasDebt;
	}

	void addDebt(int minerals, int gas)
	{
		mineralDebt += minerals;
		gasDebt += gas;
	}

	void subtractDebt(int minerals, int gas)
	{
		mineralDebt -= minerals;
		gasDebt -= gas;
	}
	double getMineralIncome()
	{
		int framesSinceLastCheck = BWAPI::Broodwar->getFrameCount() - lastCheckMineralsFrame;
		if (framesSinceLastCheck > 24 * 5 || lastCheckMineralsFrame == 0)
		{
			mineralIncome = ((BWAPI::Broodwar->self()->gatheredMinerals() - 50) - lastCheckMinerals) * 12;
			lastCheckMinerals = (BWAPI::Broodwar->self()->gatheredMinerals() - 50);
			lastCheckMineralsFrame = BWAPI::Broodwar->getFrameCount();
		}
		return mineralIncome;
	}

	double getGasIncome()
	{
		int framesSinceLastCheck = BWAPI::Broodwar->getFrameCount() - lastCheckGasFrame;
		if (framesSinceLastCheck > 24 * 5 || lastCheckGasFrame == 0)
		{
			gasIncome = (BWAPI::Broodwar->self()->gatheredGas() - lastCheckGas) * 12;
			lastCheckGas = BWAPI::Broodwar->self()->gatheredGas();
			lastCheckGasFrame = BWAPI::Broodwar->getFrameCount();
		}
		return gasIncome;
	}

	double getUnitMultiplier()
	{
		return unitMultiplier;
	}

	void updateUnitMultiplier()
	{
		unitMultiplier = mineralIncome / 1000;
	}

	void doneExpanding()
	{
		expanding = false;
	}

	void updateEconomy()
	{
		DesireHelper::updateExpandDesire();
		updateUnitMultiplier();
		if (DesireHelper::getExpandDesire() >= 1 && !expanding)
			expand();
	}

	void expand()
	{
		expanding = true;
		Task* expand = new Expand();
		TaskHelper::addTask(expand, true);
	}
}
