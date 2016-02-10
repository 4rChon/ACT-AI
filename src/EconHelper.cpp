#include "..\include\EconHelper.h"

namespace EconHelper
{
	int EconHelper::haveMoney(BWAPI::UnitType unitType)
	{
		return ((BWAPI::Broodwar->self()->minerals() - mineralDebt) >= unitType.mineralPrice() && (BWAPI::Broodwar->self()->gas() - gasDebt) >= unitType.gasPrice());
	}

	int EconHelper::haveMoney(BWAPI::UpgradeType upgradeType)
	{
		return ((BWAPI::Broodwar->self()->minerals() - mineralDebt) >= upgradeType.mineralPrice() && (BWAPI::Broodwar->self()->gas() - gasDebt) >= upgradeType.gasPrice());
	}

	int EconHelper::haveSupply(BWAPI::UnitType unitType)
	{
		return (BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() >= unitType.supplyRequired());
	}
}
