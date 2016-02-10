#pragma once
#include "BWAPI.h"
#include <BWAPI/SetContainer.h>

namespace MapHelper
{
	struct Zone
	{
	private:
		BWTA::Region* region;
		int id;

	public:
		Zone(BWTA::Region* region, int id)
		{
			this->region = region;
			this->id = id;
		}

		BWTA::Region* getRegion() const
		{
			return region;
		}

		int getID() const
		{
			return id;
		}
	};

	void initialiseHelper();
	Zone* getZone(int id);
	Zone* getZone(BWTA::Region* region);

}


//std::unordered_map<MapHelper::Zone*, double> AttackDesireMap;
//std::unordered_map<MapHelper::Zone*, double> DefendDesireMap;