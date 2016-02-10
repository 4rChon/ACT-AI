#include "..\include\EconHelper.h"

namespace EconHelper
{
	namespace
	{
		static int mineralDebt;
		static int gasDebt;
	}

	void initialiseHelper()
	{
		gasDebt = 0;
		mineralDebt = 0;
	}

	int haveMoney(BWAPI::UnitType unitType)
	{
		return (BWAPI::Broodwar->self()->minerals() - mineralDebt) >= unitType.mineralPrice() && (BWAPI::Broodwar->self()->gas() - gasDebt) >= unitType.gasPrice();
	}

	int haveMoney(BWAPI::UpgradeType upgradeType)
	{
		return (BWAPI::Broodwar->self()->minerals() - mineralDebt) >= upgradeType.mineralPrice() && (BWAPI::Broodwar->self()->gas() - gasDebt) >= upgradeType.gasPrice();
	}

	int haveMoney(int minerals, int gas)
	{
		return BWAPI::Broodwar->self()->minerals() >= minerals && BWAPI::Broodwar->self()->gas() >= gas;
	}

	int haveSupply(BWAPI::UnitType unitType)
	{
		return BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() >= unitType.supplyRequired();
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
}
