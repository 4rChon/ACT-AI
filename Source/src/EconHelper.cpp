#include "EconHelper.h"
#include "TaskHelper.h"
#include "DesireHelper.h"
#include "Expand.h"
#include <algorithm>

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
		return util::game::getSelf()->supplyTotal() - util::game::getSelf()->supplyUsed() >= unitType.supplyRequired();
	}

	int getMinerals()
	{
		return util::game::getSelf()->minerals() - mineralDebt;
	}

	int getGas()
	{
		return util::game::getSelf()->gas() - gasDebt;
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
			mineralIncome = ((util::game::getSelf()->gatheredMinerals() - 50) - lastCheckMinerals) * 12;
			lastCheckMinerals = (util::game::getSelf()->gatheredMinerals() - 50);
			lastCheckMineralsFrame = BWAPI::Broodwar->getFrameCount();
		}
		return mineralIncome;
	}

	double getGasIncome()
	{
		int framesSinceLastCheck = BWAPI::Broodwar->getFrameCount() - lastCheckGasFrame;
		if (framesSinceLastCheck > 24 * 5 || lastCheckGasFrame == 0)
		{
			gasIncome = (util::game::getSelf()->gatheredGas() - lastCheckGas) * 12;
			lastCheckGas = util::game::getSelf()->gatheredGas();
			lastCheckGasFrame = BWAPI::Broodwar->getFrameCount();
		}
		return gasIncome;
	}

	double getUnitMultiplier(Composition composition)
	{
		double gasCostPerMinute = 0;
		double minCostPerMinute = 0;
		for each(auto &unitType in composition.getTypes())
		{
			double minCost = unitType.mineralPrice();
			double gasCost = unitType.gasPrice();
			double buildTimeInMinutes = (double)unitType.buildTime() / (24 * 60);
			minCostPerMinute += (minCost / buildTimeInMinutes);
			gasCostPerMinute += (gasCost / buildTimeInMinutes);
		}

		std::cout << "Mineral Ratio: " << (double)getMineralIncome() / minCostPerMinute  << "\n";
		std::cout << "Gas Ratio: " << (double)getGasIncome() / gasCostPerMinute  << "\n";
		return std::max((getMineralIncome() + getMinerals()) / minCostPerMinute, (getGasIncome() + getGas()) / gasCostPerMinute);
	}

	void doneExpanding()
	{
		expanding = false;
	}

	void updateEconomy()
	{
		DesireHelper::updateExpandDesire();
		if (DesireHelper::getExpandDesire() >= 1 && !expanding)
			expand();
	}

	ResourceDepot* getLeastSaturatedBase()
	{
		
		auto bases = AgentHelper::getResourceDepots();

		if (bases.size() == 0)
			return nullptr;

		ResourceDepot* b = *bases.begin();

		for (auto &base : bases)
		{
			if (b->getGasSaturation() > base->getGasSaturation())
				b = base;
		}

		if (b->getGasSaturation() < 1)
			return b;

		b = *bases.begin();
		for (auto &base : bases)
		{
			if (b->getMineralSaturation() > base->getMineralSaturation())
				b = base;
		}

		return b;
	}

	void expand()
	{
		expanding = true;
		Task* expand = new Expand();
		TaskHelper::addTask(expand, true);
	}
}
