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
		//double gasCostPerMinute = 0;
		double minCostPerMinute = 0;
		auto unitMap = composition.getUnitMap();
		int min = 0;
		for each(auto &unit in composition.getUnitMap())
		{
			if (unit.second > 0 && (min == 0 || unit.second < min))
				min = unit.second;
		}

		for each(auto &unitType in composition.getTypes())
		{
			double unitCount = ((double)composition[unitType] / min);
			double minCost = unitType.mineralPrice() + unitType.whatBuilds().first.mineralPrice();
			//double gasCost = unitType.gasPrice() + unitType.whatBuilds().first.gasPrice();
			double buildTimeInMinutes = ((double)(unitType.buildTime() + unitType.whatBuilds().first.buildTime()) / (24 * 60)) * unitCount;
			minCostPerMinute += (minCost / buildTimeInMinutes) * unitCount;
			//gasCostPerMinute += (gasCost / buildTimeInMinutes) * unitCount;
		}
		
		double mineralRatio = double(getMinerals() + getMineralIncome()) / minCostPerMinute;		
		//double gasRatio = double(getGas() + getGasIncome()) / gasCostPerMinute;
		std::cout << "Mineral Ratio: " << mineralRatio << " : " << getMinerals() << " + " << getMineralIncome() << " / " << minCostPerMinute << "\n";
		//std::cout << "Gas Ratio: " << gasRatio << "\n";

		//if (mineralRatio > 40) mineralRatio = 40;
		//if (gasRatio > 40) gasRatio = 40;

		//double ratio = std::max(mineralRatio, gasRatio);

		//if (getGasIncome() == 0 && getGas() == 0)
		double ratio = mineralRatio;

		std::cout << "Ratio: " << ratio << "\n";
		return ratio;
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
