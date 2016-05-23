#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <BWAPI/SetContainer.h>

#include "BuildingPlacer.h"

class Zone;

namespace MapHelper
{		
	typedef BWAPI::SetContainer<Zone*, std::hash<void*>> Field;

	void initialiseHelper();
	BuildingPlacer* getBuildingPlacer();
	Zone* getZone(int id);
	Zone* getZone(BWAPI::Region region);
	const Field& getRegionField();
	void loadMapData();
	void saveMapData();
}