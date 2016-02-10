#pragma once
#include "BWAPI.h"
#include "BWTA.h"
#include <unordered_map>

namespace DesireHelper
{
	void initialiseHelper();
	void updateUnitDesireMap();
	void updateUpgradeDesireMap();
	void updateTechDesireMap();
	void updateExpansionDesireMap();
	void updateDesireMaps();
}