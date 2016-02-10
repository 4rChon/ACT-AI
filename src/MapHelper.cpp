#include "BWTA.h"
#include "..\include\MapHelper.h"

namespace MapHelper
{

	typedef BWAPI::SetContainer<Zone*, std::hash<void*>> Field;

	namespace
	{
		//std::vector<Zone*> field;
		//std::set<BWTA::Region*> regionField;
		Field regionField;

		//std::unordered_map<MapHelper::Zone*, double> AttackDesireMap;
		//std::unordered_map<MapHelper::Zone*, double> DefendDesireMap;
	}

	void initialiseHelper()
	{
		for (auto &region : BWTA::getRegions())
		{			
			Zone* zone = new Zone(region, regionField.size());
			regionField.insert(zone);
		}
	}

	Zone* getZone(int id)
	{
		for (auto &zone : regionField)
			if ((*zone).getID() == id)
				return zone;
		return nullptr;
	}

	Zone* getZone(BWTA::Region* region)
	{
		for (auto &zone : regionField)
			if ((*zone).getRegion() == region)
				return zone;
		return nullptr;
	}
}
