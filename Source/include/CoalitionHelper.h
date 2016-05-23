#pragma once

#include "Coalition.h"
#include "Composition.h"
#include "Task.h"

typedef BWAPI::SetContainer < Coalition*, std::hash<void*>> Coalitionset;

namespace CoalitionHelper
{
	void initialiseHelper();

	///getters	
	const Coalitionset& getCoalitionset();	//Return all coalitions	
	int getNextID();						//Get next coalitionID - used for initialising new coalitions

	///helpers	
	Coalition* addCoalition(Composition composition, Task* const& task);	//Add a new coalition with composition and task
	void removeCoalition(Coalition* const& coalition);						//Remove a coalition
};