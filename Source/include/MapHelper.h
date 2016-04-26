#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <BWAPI/SetContainer.h>

class Zone;

namespace MapHelper
{		
	typedef BWAPI::SetContainer<Zone*, std::hash<void*>> Field;

	void initialiseHelper();
	Zone* getZone(int id);
	Zone* getZone(BWAPI::Region region);
	const Field& getRegionField();
}