#pragma once

#include "Coalition.h"
#include "Composition.h"
#include "Task.h"

typedef BWAPI::SetContainer < Coalition*, std::hash<void*>> Coalitionset;

namespace CoalitionHelper
{
	void initialiseHelper();

	///getters
	//Return all coalitions
	const Coalitionset& getCoalitionset();

	//Get next coalitionID - used for initialising new coalitions
	int getNextID();

	///helpers
	//Add a new coalition with composition and task
	Coalition* addCoalition(Composition composition, Task* task);	

	//Remove a coalition
	void removeCoalition(Coalition* coalition);
};