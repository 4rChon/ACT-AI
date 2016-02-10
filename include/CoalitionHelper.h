#pragma once

#include "Coalition.h"
#include "Composition.h"
#include "Task.h"
#include "Coalitionset.h"

namespace CoalitionHelper
{
	//getters
	Coalition* getCoalition(int coalitionID);
	const Coalitionset& getCoalitionset();
	int getNextID();

	//helpers
	void addCoalition(Composition composition, Task* task);	
	void removeCoalition(Coalition* coalition);
};