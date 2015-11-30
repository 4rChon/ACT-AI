#pragma once

#include <BWAPI.h>
#include <BWAPI/SetContainer.h>

struct Resource
{
	int unitID = -1;
	int miningCount = 0;
	BWAPI::Unit resourceUnit;
};

class ResourceManager
{
private:
	BWAPI::Unitset resourceSet;
	BWAPI::SetContainer<Resource*, std::hash<void*>> resourceStructSet;
	static ResourceManager* instance;
public:
	//constructors and destructors
	ResourceManager();
	~ResourceManager();

	//getters
	static ResourceManager* getInstance();
	Resource* getResource(int unitID);	
	BWAPI::Unitset getResourceSet();
	BWAPI::Unitset getAllResources();

	//-
	BWAPI::Unit chooseResource(BWAPI::Unit unit);
};