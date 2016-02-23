#include "BWTA.h"
#include "BWAPI.h"
#include "MapHelper.h"

namespace MapHelper
{	
	namespace
	{
		Field regionField;
		//static std::set<BWTA::BaseLocation*> candidateBases;
	}		

	void initialiseHelper()
	{
		/*std::cout << BWTA::getRegions().size() << "\n";
		std::cout << BWAPI::Broodwar->getAllRegions().size() << "\n";*/
		/*candidateBases = BWTA::getBaseLocations();
		candidateBases.erase(BWTA::getStartLocation(BWAPI::Broodwar->self()));*/

		for (auto &region : BWAPI::Broodwar->getAllRegions())
		{			
			Zone* zone = new Zone(region, region->getID());
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

	//std::set<BWTA::BaseLocation*>& getCandidateBases()
	//{
	//	return candidateBases;
	//}
}
