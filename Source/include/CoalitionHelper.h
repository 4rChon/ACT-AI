#pragma once

#include "Coalition.h"
#include "Composition.h"
#include "Task.h"

typedef BWAPI::SetContainer < Coalition*, std::hash<void*>> Coalitionset;

namespace CoalitionHelper
{
	void initialiseHelper();

	//getters
	Coalition* getCoalition(int coalitionID);
	const Coalitionset& getCoalitionset();
	int getNextID();

	//helpers
	Coalition* addCoalition(Composition composition, Task* task);	
	void removeCoalition(Coalition* coalition);
	void updateFreeCoalitions();
};