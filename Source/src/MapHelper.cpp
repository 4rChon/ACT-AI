#include "BWTA.h"
#include "BWAPI.h"
#include "MapHelper.h"
#include "DesireHelper.h"
#include "UtilHelper.h"
#include "Zone.h"

namespace MapHelper
{	
	namespace
	{
		Field regionField;
	}		

	void initialiseHelper()
	{
		for (auto &region : BWAPI::Broodwar->getAllRegions())
		{						
			Zone* zone = new Zone(region, region->getID());
			regionField.insert(zone);
		}

		for (auto &zone : regionField)
			zone->initNeighbourhood();
	}

	Zone* getZone(int id)
	{
		for (auto &zone : regionField)
			if ((*zone).getID() == id)
				return zone;
		return nullptr;
	}

	Zone* getZone(BWAPI::Region region)
	{
		for (auto &zone : regionField)
			if ((*zone).getRegion() == region)
				return zone;
		return nullptr;
	}

	const Field& getRegionField()
	{
		return regionField;
	}	
}
