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
		BuildingPlacer* bp;
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
		
		bp = new BuildingPlacer();

		loadMapData();
	}

	BuildingPlacer* getBuildingPlacer()
	{
		return bp;
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

	void loadMapData()
	{		
		auto zoneDefenseMap = util::data::deserializeMapData();
		if (zoneDefenseMap.size() > 0)
		{
			for (auto &zone : regionField)
				zone->setTimesDefended(zoneDefenseMap[zone->getID()]);
		}
	}

	void saveMapData()
	{
		//diminish defence counts by 1
		for (auto &zone : regionField)
		{
			if(zone->getTimesDefended() > 0)
				zone->setTimesDefended(zone->getTimesDefended() - 1);
		}

		util::data::serializeMapData(regionField);
	}
}
